/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperOctreeCursor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkHyperOctreeCursor.h"


//-----------------------------------------------------------------------------
// Constructor.
vtkHyperOctreeCursor::vtkHyperOctreeCursor()
{
  VTK_LEGACY_BODY(vtkHyperOctreeCursor, "VTK 8.1");
}

//-----------------------------------------------------------------------------
vtkHyperOctreeCursor::~vtkHyperOctreeCursor()
{
}
//-----------------------------------------------------------------------------
void vtkHyperOctreeCursor::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
