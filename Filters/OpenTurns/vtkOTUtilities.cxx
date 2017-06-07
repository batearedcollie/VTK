/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOTUtilities.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOTUtilities.h"

#include "vtkDataArrayCollection.h"
#include "vtkDoubleArray.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"

#if (OPENTURNS_VERSION_MAJOR == 1 && OPENTURNS_VERSION_MINOR == 8)
#include "openturns/NumericalSample.hxx"
#else
#include "openturns/Sample.hxx"
#endif

using namespace OT;

#if (OPENTURNS_VERSION_MAJOR == 1 && OPENTURNS_VERSION_MINOR == 8)
typedef NumericalPoint Point;
#endif

//-----------------------------------------------------------------------------
Sample* vtkOTUtilities::SingleDimArraysToSample(vtkDataArrayCollection* arrays)
{
  if (arrays == NULL)
  {
    return NULL;
  }

  int numComp = arrays->GetNumberOfItems();
  if (numComp == 0)
  {
    vtkWarningWithObjectMacro(arrays, "Collection is empty");
    return NULL;
  }
  int numTuples = arrays->GetItem(0)->GetNumberOfTuples();
  Sample* ns = new Sample(numTuples, numComp);

  int j = 0;
  arrays->InitTraversal();
  vtkDataArray* array = arrays->GetNextItem();
  while (array != NULL)
  {
    if (numTuples != array->GetNumberOfTuples())
    {
      // TODO NULL Object
      vtkErrorWithObjectMacro(arrays,
        "An array has not the expected number of tuples. Expecting: " << numTuples << " , got: "
                                                                      << array->GetNumberOfTuples()
                                                                      << " , dropping it");
      continue;
    }
    for (int i = 0; i < numTuples; ++i)
    {
      ns->at(i, j) = array->GetComponent(i, 0);
    }
    array = arrays->GetNextItem();
    j++;
  }
  return ns;
}

//-----------------------------------------------------------------------------
Sample* vtkOTUtilities::ArrayToSample(vtkDataArray* arr)
{
  if (arr == NULL)
  {
    return NULL;
  }

  int numTuples = arr->GetNumberOfTuples();
  int numComp = arr->GetNumberOfComponents();
  Sample* ns = new Sample(numTuples, numComp);

  for (int i = 0; i < numTuples; ++i)
  {
    for (int j = 0; j < ns->getDimension(); j++)
    {
      ns->at(i, j) = arr->GetComponent(i, j);
    }
  }
  return ns;
}

//-----------------------------------------------------------------------------
vtkDataArray* vtkOTUtilities::SampleToArray(Sample* ns)
{
  if (ns == NULL)
  {
    return NULL;
  }

  int numTuples = ns->getSize();
  int numComp = ns->getDimension();

  vtkDoubleArray* arr = vtkDoubleArray::New();
  arr->SetNumberOfTuples(numTuples);
  arr->SetNumberOfComponents(numComp);

  for (int i = 0; i < numTuples; i++)
  {
    for (int j = 0; j < numComp; j++)
    {
      arr->SetComponent(i, j, ns->at(i, j));
    }
  }
  return arr;
}
