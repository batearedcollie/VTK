/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSPHKernel.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSPHKernel.h"
#include "vtkAbstractPointLocator.h"
#include "vtkObjectFactory.h"
#include "vtkIdList.h"
#include "vtkDoubleArray.h"

vtkStandardNewMacro(vtkSPHKernel);

//----------------------------------------------------------------------------
vtkSPHKernel::vtkSPHKernel()
{
}


//----------------------------------------------------------------------------
vtkSPHKernel::~vtkSPHKernel()
{
}


//----------------------------------------------------------------------------
vtkIdType vtkSPHKernel::
ComputeWeights(double x[3], vtkIdList *pIds, vtkDoubleArray *weights)
{
  pIds->SetNumberOfIds(1);
  vtkIdType pId = this->Locator->FindClosestPoint(x);
  pIds->SetId(0,pId);
  weights->SetNumberOfTuples(1);
  weights->SetValue(0,1.0);

  return 1;
}

//----------------------------------------------------------------------------
void vtkSPHKernel::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

}
