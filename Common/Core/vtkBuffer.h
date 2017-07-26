/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBuffer.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkBuffer
 * @brief   internal storage class used by vtkSOADataArrayTemplate,
 * vtkAOSDataArrayTemplate, and others.
 *
 * vtkBuffer makes it easier to keep data pointers in vtkDataArray subclasses.
 * This is an internal class and not intended for direct use expect when writing
 * new types of vtkDataArray subclasses.
*/

#ifndef vtkBuffer_h
#define vtkBuffer_h

#include "vtkObject.h"
#include "vtkObjectFactory.h" // New() implementation

template <class ScalarTypeT>
class vtkBuffer : public vtkObject
{
public:
  vtkTemplateTypeMacro(vtkBuffer<ScalarTypeT>, vtkObject)
  typedef ScalarTypeT ScalarType;

  static vtkBuffer<ScalarTypeT>* New();

  /**
   * Access the buffer as a scalar pointer.
   */
  inline ScalarType* GetBuffer() { return this->Pointer; }
  inline const ScalarType* GetBuffer() const { return this->Pointer; }

  /**
   * Set the memory buffer that this vtkBuffer object will manage. @a array
   * is a pointer to the buffer data and @a size is the size of the bufffer (in
   * number of elements). If @a save is true, the buffer will not be freed when
   * this vtkBuffer object is deleted or resize -- otherwise, @a deleteMethod
   * is a function that will be called to free the buffer
   */
  void SetBuffer(ScalarType* array, vtkIdType size, bool save=false,
                 void (*deleteFunction)(void*)=free);

  /**
   * Return the number of elements the current buffer can hold.
   */
  inline vtkIdType GetSize() const { return this->Size; }

  /**
   * Allocate a new buffer that holds @a size elements. Old data is not saved.
   */
  bool Allocate(vtkIdType size);

  /**
   * Allocate a new buffer that holds @a newsize elements. Old data is
   * preserved.
   */
  bool Reallocate(vtkIdType newsize);

protected:
  vtkBuffer()
    : Pointer(nullptr),
      Size(0),
      Save(false),
      DeleteFunction(free)
  {
  }

  ~vtkBuffer() VTK_OVERRIDE
  {
    this->SetBuffer(nullptr, 0);
  }

  ScalarType *Pointer;
  vtkIdType Size;
  bool Save;
  void (*DeleteFunction)(void*);

private:
  vtkBuffer(const vtkBuffer&) VTK_DELETE_FUNCTION;
  void operator=(const vtkBuffer&) VTK_DELETE_FUNCTION;
};

template <class ScalarT>
inline vtkBuffer<ScalarT> *vtkBuffer<ScalarT>::New()
{
  VTK_STANDARD_NEW_BODY(vtkBuffer<ScalarT>)
}

//------------------------------------------------------------------------------
template <typename ScalarT>
void vtkBuffer<ScalarT>::SetBuffer(
    typename vtkBuffer<ScalarT>::ScalarType *array,
    vtkIdType size, bool save, void (*deleteFunction)(void*))
{
  if (this->Pointer != array)
  {
    if (!this->Save)
    {
      this->DeleteFunction(this->Pointer);
    }
    this->Pointer = array;
  }
  this->Size = size;
  this->Save = save;
  this->DeleteFunction = deleteFunction;
}

//------------------------------------------------------------------------------
template <typename ScalarT>
bool vtkBuffer<ScalarT>::Allocate(vtkIdType size)
{
  // release old memory.
  this->SetBuffer(nullptr, 0);
  if (size > 0)
  {
    ScalarType* newArray =
        static_cast<ScalarType*>(malloc(size * sizeof(ScalarType)));
    if (newArray)
    {
      this->SetBuffer(newArray, size, false, free);
      return true;
    }
    return false;
  }
  return true; // size == 0
}

//------------------------------------------------------------------------------
template <typename ScalarT>
bool vtkBuffer<ScalarT>::Reallocate(vtkIdType newsize)
{
  if (newsize == 0) { return this->Allocate(0); }

  if (this->Pointer &&
      (this->Save || this->DeleteFunction != free))
  {
    ScalarType* newArray =
        static_cast<ScalarType*>(malloc(newsize * sizeof(ScalarType)));
    if (!newArray)
    {
      return false;
    }
    std::copy(this->Pointer, this->Pointer + std::min(this->Size, newsize),
              newArray);
    // now save the new array and release the old one too.
    this->SetBuffer(newArray, newsize, false, free);
  }
  else
  {
    // Try to reallocate with minimal memory usage and possibly avoid
    // copying.
    ScalarType* newArray = static_cast<ScalarType*>(
          realloc(this->Pointer, newsize * sizeof(ScalarType)));
    if (!newArray)
    {
      return false;
    }
    this->Pointer = newArray;
    this->Size = newsize;
  }
  return true;
}

#endif
// VTK-HeaderTest-Exclude: vtkBuffer.h
