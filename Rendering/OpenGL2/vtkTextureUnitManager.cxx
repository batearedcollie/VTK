/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTextureUnitManager.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTextureUnitManager.h"
#include "vtk_glew.h"

#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderWindow.h"

#include <cassert>

vtkStandardNewMacro(vtkTextureUnitManager);

// ----------------------------------------------------------------------------
vtkTextureUnitManager::vtkTextureUnitManager()
{
  this->NumberOfTextureUnits = 0;
  this->TextureUnits = nullptr;
}

// ----------------------------------------------------------------------------
vtkTextureUnitManager::~vtkTextureUnitManager()
{
  this->DeleteTable();
}

// ----------------------------------------------------------------------------
// Description:
// Delete the allocation table and check if it is not called before
// all the texture units have been released.
void vtkTextureUnitManager::DeleteTable()
{
  if (this->TextureUnits != nullptr)
  {
    size_t i = 0;
    size_t c = this->NumberOfTextureUnits;
    bool valid = true;
    while (valid && i < c)
    {
      valid = !this->TextureUnits[i];
      ++i;
    }
    if (!valid)
    {
      vtkErrorMacro(
        << "the texture unit is deleted but some texture units have not been released: Id=" << i);
    }
    delete[] this->TextureUnits;
    this->TextureUnits = nullptr;
    this->NumberOfTextureUnits = 0;
  }
}

// ----------------------------------------------------------------------------
void vtkTextureUnitManager::Initialize()
{
  // this->DeleteTable();
  if (!this->NumberOfTextureUnits)
  {
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->NumberOfTextureUnits);
    if (this->NumberOfTextureUnits > 0)
    {
      this->TextureUnits = new bool[this->NumberOfTextureUnits];
      size_t i = 0;
      size_t c = this->NumberOfTextureUnits;
      while (i < c)
      {
        this->TextureUnits[i] = false;
        ++i;
      }
    }
  }
}

// ----------------------------------------------------------------------------
// Description:
// Number of texture units supported by the OpenGL context.
int vtkTextureUnitManager::GetNumberOfTextureUnits()
{
  return this->NumberOfTextureUnits;
}

// ----------------------------------------------------------------------------
// Description:
// Reserve a texture unit. It returns its number.
// It returns -1 if the allocation failed (because there is no more
// texture unit left).
// \post valid_result: result==-1 || result>=0 && result<this->GetNumberOfTextureUnits())
// \post allocated: result==-1 || this->IsAllocated(result)
int vtkTextureUnitManager::Allocate()
{
  bool found = false;
  size_t i = 0;
  size_t c = this->NumberOfTextureUnits;
  while (!found && i < c)
  {
    found = !this->TextureUnits[i];
    ++i;
  }

  int result;
  if (found)
  {
    result = static_cast<int>(i - 1);
    this->TextureUnits[result] = true;
  }
  else
  {
    result = -1;
  }

  assert("post: valid_result" &&
    (result == -1 || (result >= 0 && result < this->GetNumberOfTextureUnits())));
  assert("post: allocated" && (result == -1 || this->IsAllocated(result)));
  return result;
}

int vtkTextureUnitManager::Allocate(int unit)
{
  if (this->IsAllocated(unit))
  {
    return -1;
  }

  this->TextureUnits[unit] = true;

  return unit;
}

// ----------------------------------------------------------------------------
// Description:
// Tell if texture unit `textureUnitId' is already allocated.
// \pre valid_id_range : textureUnitId>=0 && textureUnitId<this->GetNumberOfTextureUnits()
bool vtkTextureUnitManager::IsAllocated(int textureUnitId)
{
  assert("pre: valid_textureUnitId_range" && textureUnitId >= 0 &&
    textureUnitId < this->GetNumberOfTextureUnits());
  return (this->TextureUnits[textureUnitId] ? true : false);
}

// ----------------------------------------------------------------------------
// Description:
// Release a texture unit.
// \pre valid_id: textureUnitId>=0 && textureUnitId<this->GetNumberOfTextureUnits()
// \pre allocated_id: this->IsAllocated(textureUnitId)
void vtkTextureUnitManager::Free(int textureUnitId)
{
  assert("pre: valid_textureUnitId" &&
    (textureUnitId >= 0 && textureUnitId < this->GetNumberOfTextureUnits()));
  //  assert("pre: allocated_textureUnitId" && this->IsAllocated(textureUnitId));

  this->TextureUnits[textureUnitId] = false;
}

// ----------------------------------------------------------------------------
void vtkTextureUnitManager::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
