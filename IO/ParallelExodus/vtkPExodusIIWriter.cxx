/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkExodusIIWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/*----------------------------------------------------------------------------
 Copyright (c) Sandia Corporation
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

#include "vtkPExodusIIWriter.h"
#include "vtkObjectFactory.h"
#include "vtkModelMetadata.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkDoubleArray.h"
#include "vtkDataObject.h"
#include "vtkFieldData.h"
#include "vtkCompositeDataSet.h"
#include "vtkCompositeDataIterator.h"
#include "vtkUnstructuredGrid.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkIdList.h"
#include "vtkThreshold.h"
#include "vtkIntArray.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkArrayIteratorIncludes.h"

#include "vtkMultiProcessController.h"

#include "vtk_exodusII.h"
#include <time.h>
#include <ctype.h>

vtkStandardNewMacro (vtkPExodusIIWriter);

//----------------------------------------------------------------------------

vtkPExodusIIWriter::vtkPExodusIIWriter ()
{
}

vtkPExodusIIWriter::~vtkPExodusIIWriter ()
{
}

void vtkPExodusIIWriter::PrintSelf (ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

//----------------------------------------------------------------------------
int vtkPExodusIIWriter::CheckParameters ()
{
  vtkMultiProcessController *c = vtkMultiProcessController::GetGlobalController();
  int NumberOfProcesses = c->GetNumberOfProcesses();
  int MyRank = c->GetLocalProcessId();

  if (this->GhostLevel > 0)
    {
    vtkWarningMacro(<< "ExodusIIWriter ignores ghost level request");
    }

  return this->Superclass::CheckParametersInternal(NumberOfProcesses, MyRank);
}

void vtkPExodusIIWriter::CheckBlockInfoMap ()
{
  // if we're multiprocess we need to make sure the block info map matches
  int maxId = -1;
  std::map<int, Block>::const_iterator iter;
  for (iter = this->BlockInfoMap.begin (); iter != this->BlockInfoMap.end (); iter ++)
    {
    if (iter->first > maxId)
      {
      maxId = iter->first;
      }
    }
  vtkMultiProcessController *c = vtkMultiProcessController::GetGlobalController();
  int globalMaxId;
  c->AllReduce (&maxId, &globalMaxId, 1, vtkCommunicator::MAX_OP);
  maxId = globalMaxId;
  for (int i = 1; i <= maxId; i ++)
    {
    Block &b = this->BlockInfoMap[i]; // ctor called (init all to 0/-1) if not preset
    int globalType;
    c->AllReduce (&b.Type, &globalType, 1, vtkCommunicator::MAX_OP);
    if (b.Type != 0 && b.Type != globalType)
      {
      vtkWarningMacro (
        << "The type associated with ID's across processors doesn't match");
      }
    else
      {
      b.Type = globalType;
      }
    int globalNodes;
    c->AllReduce (&b.NodesPerElement, &globalNodes, 1, vtkCommunicator::MAX_OP);
    if (b.NodesPerElement != globalNodes)
      {
      vtkWarningMacro (
        << "NodesPerElement associated with ID's across processors doesn't match");
      }
    else
      {
      b.NodesPerElement = globalNodes;
      }
    }
}
