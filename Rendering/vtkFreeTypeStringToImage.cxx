/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkFreeTypeStringToImage.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkFreeTypeStringToImage.h"

#include "vtkStdString.h"
#include "vtkUnicodeString.h"
#include "vtkTextProperty.h"
#include "vtkVector.h"
#include "vtkImageData.h"

#include "vtkFreeTypeUtilities.h"

#include "vtkObjectFactory.h"

class vtkFreeTypeStringToImage::Internals
{
public:
  Internals()
    {
    this->FreeType = vtkSmartPointer<vtkFreeTypeUtilities>::New();
    }
  vtkSmartPointer<vtkFreeTypeUtilities> FreeType;
};

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkFreeTypeStringToImage);

//-----------------------------------------------------------------------------
vtkFreeTypeStringToImage::vtkFreeTypeStringToImage()
{
  this->Implementation = new Internals;
}

//-----------------------------------------------------------------------------
vtkFreeTypeStringToImage::~vtkFreeTypeStringToImage()
{
  delete this->Implementation;
}

//-----------------------------------------------------------------------------
vtkVector2i vtkFreeTypeStringToImage::GetBounds(vtkTextProperty *property,
                                                const vtkUnicodeString& string)
{
  vtkVector2i recti;
  int tmp[4];
  if (!property)
    {
    return recti;
    }

  this->Implementation->FreeType->GetBoundingBox(property, string.utf8_str(),
                                                 tmp);

  recti.Set(tmp[1] - tmp[0],
            tmp[3] - tmp[2]);

  return recti;
}

int vtkFreeTypeStringToImage::RenderString(vtkTextProperty *property,
                                           const vtkUnicodeString& string,
                                           vtkImageData *data)
{
  // Get the required size, and initialize a new QImage to draw on.
  vtkVector2i box = this->GetBounds(property, string);
  if (box.GetX() == 0 || box.GetY() == 0)
    {
    return 0;
    }

  this->Implementation->FreeType->RenderString(property, string.utf8_str(),0,0,
                                               data);

  return 1;
}

//-----------------------------------------------------------------------------
void vtkFreeTypeStringToImage::DeepCopy(vtkFreeTypeStringToImage *)
{
}

//-----------------------------------------------------------------------------
void vtkFreeTypeStringToImage::PrintSelf(ostream &os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
