/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSoADataArrayTemplate.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include <cassert>

//-----------------------------------------------------------------------------
template<class ValueType>
vtkSoADataArrayTemplate<ValueType>*
vtkSoADataArrayTemplate<ValueType>::New()
{
  VTK_STANDARD_NEW_BODY(vtkSoADataArrayTemplate<ValueType>);
}

//-----------------------------------------------------------------------------
template<class ValueType>
vtkSoADataArrayTemplate<ValueType>::vtkSoADataArrayTemplate()
  : Resizeable(true),
  NumberOfComponentsReciprocal(1.0)
{
}

//-----------------------------------------------------------------------------
template<class ValueType>
vtkSoADataArrayTemplate<ValueType>::~vtkSoADataArrayTemplate()
{
  for (int cc=0; cc < this->Data.size(); ++cc)
    {
    this->Data[cc].SetBuffer(NULL, 0);
    }
}

//-----------------------------------------------------------------------------
template<class ValueType>
void vtkSoADataArrayTemplate<ValueType>::SetNumberOfComponents(int val)
{
  this->GenericDataArrayType::SetNumberOfComponents(val);
  size_t numComps = static_cast<size_t>(this->GetNumberOfComponents());
  assert(numComps >= 1);
  while (this->Data.size() > numComps)
    {
    this->Data.back().SetBuffer(NULL, 0);
    this->Data.pop_back();
    }
  this->Data.resize(numComps);
  this->NumberOfComponentsReciprocal = 1.0 / this->NumberOfComponents;
}

//-----------------------------------------------------------------------------
template<class ValueType>
void vtkSoADataArrayTemplate<ValueType>::SetArray(
  int comp, ValueType* array, vtkIdType size, bool save, int deleteMethod)
{
  const int numComps = this->GetNumberOfComponents();
  if (comp >= numComps || comp < 0)
    {
    vtkErrorMacro("Invalid component number '" << comp << "' specified. "
      "Use `SetNumberOfComponents` first to set the number of components.");
    return;
    }

  this->Data[comp].SetBuffer(array, size, save, deleteMethod);
  this->DataChanged();
  // FIXME: Should we update MaxId like vtkDataArrayTemplate does? If so, how?
}

//-----------------------------------------------------------------------------
template<class ValueType>
bool vtkSoADataArrayTemplate<ValueType>::AllocateTuples(vtkIdType numTuples)
{
  if (!this->Resizeable)
    {
    vtkIdType minTuples = VTK_ID_MAX;
    for (int cc=0, max=this->Data.size(); cc < max; cc++)
      {
      minTuples = std::min(minTuples, this->Data[cc].GetSize());
      }
    if (numTuples <= minTuples)
      {
      return true;
      }
    else
      {
      vtkErrorMacro("AllocateTuples cannot be called on a non-resizeable "
                    "array!");
      return false;
      }
    }

  for (int cc=0, max=this->Data.size(); cc < max; ++cc)
    {
    if (!this->Data[cc].Allocate(numTuples))
      {
      return false;
      }
    }
  return true;
}

//-----------------------------------------------------------------------------
template<class ValueType>
bool vtkSoADataArrayTemplate<ValueType>::ReallocateTuples(vtkIdType numTuples)
{
  if (!this->Resizeable)
    {
    vtkIdType minTuples = VTK_ID_MAX;
    for (int cc=0, max=this->Data.size(); cc < max; cc++)
      {
      minTuples = std::min(minTuples, this->Data[cc].GetSize());
      }
    if (numTuples <= minTuples)
      {
      return true;
      }
    vtkErrorMacro("Resize attempted on a non-resizable array!");
    return false;
    }

  for (int cc=0, max=this->Data.size(); cc < max; ++cc)
    {
    if (!this->Data[cc].Reallocate(numTuples))
      {
      return false;
      }
    }
  return true;
}
