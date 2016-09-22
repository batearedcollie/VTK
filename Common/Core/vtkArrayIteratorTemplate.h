/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkArrayIteratorTemplate.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkArrayIteratorTemplate - Implementation template for a array
// iterator.
//
// .SECTION Description
// This is implementation template for a array iterator. It only works
// with arrays that have a contiguous internal storage of values (as in
// vtkDataArray, vtkStringArray).

#ifndef vtkArrayIteratorTemplate_h
#define vtkArrayIteratorTemplate_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkArrayIterator.h"

#include "vtkStdString.h" // For template instantiation
#include "vtkUnicodeString.h" // For template instantiation
#include "vtkVariant.h" // For template instantiation

template <class T>
class VTKCOMMONCORE_EXPORT vtkArrayIteratorTemplate : public vtkArrayIterator
{
public:
  static vtkArrayIteratorTemplate<T>* New();
  vtkTemplateTypeMacro(vtkArrayIteratorTemplate<T>, vtkArrayIterator)
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  // Description:
  // Set the array this iterator will iterate over.
  // After Initialize() has been called, the iterator is valid
  // so long as the Array has not been modified
  // (except using the iterator itself).
  // If the array is modified, the iterator must be re-intialized.
  void Initialize(vtkAbstractArray* array) VTK_OVERRIDE;

  // Description:
  // Get the array.
  vtkAbstractArray* GetArray(){ return this->Array; }


  // Description:
  // Must be called only after Initialize.
  T* GetTuple(vtkIdType id);

  // Description:
  // Must be called only after Initialize.
  T& GetValue(vtkIdType id)
    { return this->Pointer[id]; }

  // Description:
  // Sets the value at the index. This does not verify if the index is
  // valid.  The caller must ensure that id is less than the maximum
  // number of values.
  void SetValue(vtkIdType id, T value)
    {
    this->Pointer[id] = value;
    }

  // Description:
  // Must be called only after Initialize.
  vtkIdType GetNumberOfTuples();

  // Description:
  // Must be called only after Initialize.
  vtkIdType GetNumberOfValues();

  // Description:
  // Must be called only after Initialize.
  int GetNumberOfComponents();

  // Description:
  // Get the data type from the underlying array.
  int GetDataType() VTK_OVERRIDE;

  // Description:
  // Get the data type size from the underlying array.
  int GetDataTypeSize();

  // Description:
  // This is the data type for the value.
  typedef T ValueType;
protected:
  vtkArrayIteratorTemplate();
  ~vtkArrayIteratorTemplate() VTK_OVERRIDE;

  T* Pointer;
private:
  vtkArrayIteratorTemplate(const vtkArrayIteratorTemplate&) VTK_DELETE_FUNCTION;
  void operator=(const vtkArrayIteratorTemplate&) VTK_DELETE_FUNCTION;

 void SetArray(vtkAbstractArray*);
 vtkAbstractArray* Array;
};

#ifdef VTK_USE_EXTERN_TEMPLATE
#ifndef vtkArrayIteratorTemplateInstantiate_cxx
#ifdef _MSC_VER
#pragma warning (push)
// The following is needed when the vtkArrayIteratorTemplate is declared
// dllexport and is used from another class in vtkCommonCore
#pragma warning (disable: 4910) // extern and dllexport incompatible
#endif
vtkInstantiateTemplateMacro(
  extern template class VTKCOMMONCORE_EXPORT vtkArrayIteratorTemplate)
extern template class VTKCOMMONCORE_EXPORT
  vtkArrayIteratorTemplate<vtkStdString>;
extern template class VTKCOMMONCORE_EXPORT
  vtkArrayIteratorTemplate<vtkUnicodeString>;
extern template class VTKCOMMONCORE_EXPORT
  vtkArrayIteratorTemplate<vtkVariant>;
#ifdef _MSC_VER
#pragma warning (pop)
#endif
#endif
#endif // VTK_USE_EXTERN_TEMPLATE

#endif

// VTK-HeaderTest-Exclude: vtkArrayIteratorTemplate.h
