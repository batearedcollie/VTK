/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExtractEnclosedPoints.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkExtractEnclosedPoints.h"

#include "vtkSelectEnclosedPoints.h"
#include "vtkDataSet.h"
#include "vtkIdList.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkPolyData.h"
#include "vtkUnsignedCharArray.h"
#include "vtkExecutive.h"
#include "vtkFeatureEdges.h"
#include "vtkStaticCellLocator.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkGarbageCollector.h"
#include "vtkSMPTools.h"
#include "vtkSMPThreadLocalObject.h"
#include "vtkRandomPool.h"

vtkStandardNewMacro(vtkExtractEnclosedPoints);

//----------------------------------------------------------------------------
// Classes support threading. Each point can be processed separately, so the
// in/out containment check is threaded.
namespace {

//----------------------------------------------------------------------------
// The threaded core of the algorithm. Thread on point type.
template <typename T>
struct ExtractInOutCheck
{
  vtkIdType NumPts;
  T *Points;
  vtkPolyData *Surface;
  double Bounds[6];
  double Length;
  double Tolerance;
  vtkStaticCellLocator *Locator;
  vtkIdType *PointMap;
  vtkRandomPool *Sequence;

  // Don't want to allocate working arrays on every thread invocation. Thread local
  // storage eliminates lots of new/delete.
  vtkSMPThreadLocalObject<vtkIdList> CellIds;
  vtkSMPThreadLocalObject<vtkGenericCell> Cell;

  ExtractInOutCheck(vtkIdType numPts, T *pts, vtkPolyData *surface, double bds[6],
                    double tol, vtkStaticCellLocator *loc, vtkIdType *map) :
    NumPts(numPts), Points(pts), Surface(surface), Tolerance(tol), Locator(loc), PointMap(map)
  {
    this->Bounds[0] = bds[0];
    this->Bounds[1] = bds[1];
    this->Bounds[2] = bds[2];
    this->Bounds[3] = bds[3];
    this->Bounds[4] = bds[4];
    this->Bounds[5] = bds[5];
    this->Length = sqrt( (bds[1]-bds[0])*(bds[1]-bds[0]) + (bds[3]-bds[2])*(bds[3]-bds[2]) +
                         (bds[5]-bds[4])*(bds[5]-bds[4]) );

    this->Sequence = vtkRandomPool::New();
    this->Sequence->SetSize(numPts);
    this->Sequence->GeneratePool();
  }

  ~ExtractInOutCheck()
  {
    this->Sequence->Delete();
  }

  void Initialize()
  {
    vtkIdList*& cellIds = this->CellIds.Local();
    cellIds->Allocate(512);
  }

  void operator() (vtkIdType ptId, vtkIdType endPtId)
  {
    double x[3];
    const T* pts = this->Points + 3*ptId;
    vtkIdType *map = this->PointMap + ptId;
    vtkGenericCell*& cell = this->Cell.Local();
    vtkIdList*& cellIds = this->CellIds.Local();
    vtkIdType hit;

    for ( ; ptId < endPtId; ++ptId, pts+=3 )
    {
      x[0] = static_cast<double>(pts[0]);
      x[1] = static_cast<double>(pts[1]);
      x[2] = static_cast<double>(pts[2]);

      hit = vtkSelectEnclosedPoints::
        IsInsideSurface(x, this->Surface, this->Bounds, this->Length,
                        this->Tolerance, this->Locator, cellIds, cell,
                        this->Sequence, ptId);
      *map++ = (hit ? 1 : -1);
    }
  }

  void Reduce()
  {
  }

  static void Execute(vtkIdType numPts, T *pts, vtkPolyData *surface,
                      double bds[6], double tol, vtkStaticCellLocator *loc,
                      vtkIdType *hits)
  {
    ExtractInOutCheck inOut(numPts, pts, surface, bds, tol, loc, hits);
    vtkSMPTools::For(0, numPts, inOut);
  }
}; //ExtractInOutCheck

} //anonymous namespace


//----------------------------------------------------------------------------
// Construct object.
vtkExtractEnclosedPoints::vtkExtractEnclosedPoints()
{
  this->SetNumberOfInputPorts(2);

  this->CheckSurface = false;
  this->Tolerance = 0.0001;
}

//----------------------------------------------------------------------------
vtkExtractEnclosedPoints::~vtkExtractEnclosedPoints()
{
}

//----------------------------------------------------------------------------
// Partial implementation invokes vtkPointCloudFilter::RequestData(). This is
// necessary to grab the seconf input.
//
int vtkExtractEnclosedPoints::
RequestData(vtkInformation *request, vtkInformationVector **inputVector,
            vtkInformationVector *outputVector)
{
  // get the info objects
  vtkInformation *in2Info = inputVector[1]->GetInformationObject(0);

  // get the second input
  vtkPolyData *surface = vtkPolyData::SafeDownCast(
    in2Info->Get(vtkDataObject::DATA_OBJECT()));
  this->Surface = surface;

  vtkDebugMacro("Extracting enclosed points");

  // If requested, check that the surface is closed
  if ( this->Surface == nullptr ||
       (this->CheckSurface && !vtkSelectEnclosedPoints::IsSurfaceClosed(surface)) )
  {
    vtkDebugMacro("Bad enclosing surface");
    return 0;
  }

  // Okay take advantage of superclasses' RequestData() method. This provides
  // This provides a lot of the point mapping, attribute copying, etc.
  // capabilities.
  return this->Superclass::RequestData(request, inputVector, outputVector);

}

//----------------------------------------------------------------------------
// Traverse all the input points and extract points that are contained within
// the enclosing surface.
int vtkExtractEnclosedPoints::FilterPoints(vtkPointSet *input)
{
  // Initiailize search structures
  vtkStaticCellLocator *locator = vtkStaticCellLocator::New();

  vtkPolyData *surface = this->Surface;
  double bds[6];
  surface->GetBounds(bds);

  // Set up structures for acceleration ray casting
  locator->SetDataSet(surface);
  locator->BuildLocator();

  // Loop over all input points determining inside/outside
  vtkIdType numPts = input->GetNumberOfPoints();
  void *inPtr = input->GetPoints()->GetVoidPointer(0);
  switch (input->GetPoints()->GetDataType())
  {
    vtkTemplateMacro(ExtractInOutCheck<VTK_TT>::
                     Execute(numPts, (VTK_TT *)inPtr, surface, bds,
                             this->Tolerance, locator, this->PointMap));
  }

  // Clean up and get out
  locator->Delete();
  return 1;
}

//----------------------------------------------------------------------------
// Specify the second enclosing surface input via a connection
void vtkExtractEnclosedPoints::
SetSurfaceConnection(vtkAlgorithmOutput* algOutput)
{
  this->SetInputConnection(1, algOutput);
}

//----------------------------------------------------------------------------
// Specify the second enclosing surface input data
void vtkExtractEnclosedPoints::SetSurfaceData(vtkPolyData *pd)
{
  this->SetInputData(1, pd);
}

//----------------------------------------------------------------------------
// Return the enclosing surface
vtkPolyData *vtkExtractEnclosedPoints::GetSurface()
{
  return vtkPolyData::SafeDownCast(
    this->GetExecutive()->GetInputData(1, 0));
}

//----------------------------------------------------------------------------
vtkPolyData* vtkExtractEnclosedPoints::
GetSurface(vtkInformationVector *sourceInfo)
{
  vtkInformation *info = sourceInfo->GetInformationObject(1);
  if (!info)
  {
    return nullptr;
  }
  return vtkPolyData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
}

//----------------------------------------------------------------------------
int vtkExtractEnclosedPoints::
FillInputPortInformation(int port, vtkInformation *info)
{
  if (port == 0)
  {
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  }
  else if (port == 1)
  {
    info->Set(vtkAlgorithm::INPUT_IS_REPEATABLE(), 0);
    info->Set(vtkAlgorithm::INPUT_IS_OPTIONAL(), 0);
    info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
  }

  return 1;
}

//----------------------------------------------------------------------------
void vtkExtractEnclosedPoints::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Check Surface: "
     << (this->CheckSurface ? "On\n" : "Off\n");

  os << indent << "Tolerance: " << this->Tolerance << "\n";
}
