/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkParticlePathFilter.cxx

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkParticlePathFilter.h"
#include "vtkObjectFactory.h"
#include "vtkSetGet.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkIntArray.h"
#include "vtkSmartPointer.h"
#include "vtkFloatArray.h"

#include <vector>
#include <assert.h>

vtkObjectFactoryNewMacro(vtkParticlePathFilter)

ParticlePathFilterInternal::ParticlePathFilterInternal(vtkParticleTracerBase* filter)
:Filter(filter)
{
  this->Filter->SetForceReinjectionEveryNSteps(0);
  this->Filter->SetIgnorePipelineTime(1);
}

void ParticlePathFilterInternal::Reset()
{
  this->Filter->vtkParticleTracerBase::ResetCache();
  this->Paths.clear();
}

int ParticlePathFilterInternal::OutputParticles(vtkPolyData* particles)
{
  if(!this->Filter->Output)
    {
    this->Filter->Output = vtkSmartPointer<vtkPolyData>::New();
    this->Filter->Output->SetPoints(vtkSmartPointer<vtkPoints>::New());
    this->Filter->Output->GetPointData()->CopyAllocate(particles->GetPointData());
    }
  vtkPoints* pts = particles->GetPoints();
  if(!pts || pts->GetNumberOfPoints()==0)
    {
    return 0;
    }

  assert(particles->GetPointData()->GetNumberOfArrays()==this->Filter->Output->GetPointData()->GetNumberOfArrays());
  vtkPointData* outPd = this->Filter->Output->GetPointData();
  vtkFloatArray* outParticleAge = vtkFloatArray::SafeDownCast(outPd->GetArray("ParticleAge"));
  vtkPoints* outPoints = this->Filter->Output->GetPoints();

  //Get the input arrays
  vtkPointData* pd = particles->GetPointData();
  vtkIntArray* particleIds = vtkIntArray::SafeDownCast(pd->GetArray("ParticleId"));

  //Append the input arrays to the output arrays
  int begin = outPoints->GetNumberOfPoints();
  for(int i=0; i<pts->GetNumberOfPoints(); i++)
    {
    outPoints->InsertNextPoint(pts->GetPoint(i));
    }
  vtkDataSetAttributes::FieldList ptList(1);
  ptList.InitializeFieldList(pd);
  for(int i=0, j = begin; i<pts->GetNumberOfPoints(); i++,j++)
    {
    outPd->CopyData(ptList,pd,0,i,j);
    }
  assert(outPoints->GetNumberOfPoints()==outParticleAge->GetNumberOfTuples());


  //Augment the paths
  for(vtkIdType i=0; i<pts->GetNumberOfPoints(); i++)
    {
    int outId =  i+begin;

    int pid = particleIds->GetValue(i);
    for(int j=this->Paths.size(); j<=pid; j++)
      {
      this->Paths.push_back( vtkSmartPointer<vtkIdList>::New());
      }

    vtkIdList* path = this->Paths[pid];

    if(path->GetNumberOfIds()>0)
      {
      float lastAge = outParticleAge->GetValue(path->GetId(path->GetNumberOfIds()-1));
      float thisAge = outParticleAge->GetValue(outId);
      assert(thisAge>=lastAge); //if not, we will have to sort
      }
    path->InsertNextId(outId);
    }

  return 1;
}
void ParticlePathFilterInternal::Finalize()
{
  this->Filter->Output->SetLines(vtkSmartPointer<vtkCellArray>::New());
  vtkCellArray* outLines = this->Filter->Output->GetLines();
  assert(outLines);

  for(unsigned int i=0; i<this->Paths.size(); i++)
    {
    if(this->Paths[i]->GetNumberOfIds()>1)
      {
      outLines->InsertNextCell(this->Paths[i]);
      }
    }
}

vtkParticlePathFilter::vtkParticlePathFilter():It(this)
{
}

void vtkParticlePathFilter::ResetCache()
{
  Superclass::ResetCache();
  this->It.Reset();
}

void vtkParticlePathFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);
}

int vtkParticlePathFilter::OutputParticles(vtkPolyData* particles)
{
  return this->It.OutputParticles(particles);
}

void vtkParticlePathFilter::Finalize()
{
  this->It.Finalize();
}
