/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTransmitRectilinearGridPiece.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkTransmitRectilinearGridPiece.h"

#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkTransmitRectilinearGridPiece);

//----------------------------------------------------------------------------
vtkTransmitRectilinearGridPiece::vtkTransmitRectilinearGridPiece() = default;

//----------------------------------------------------------------------------
vtkTransmitRectilinearGridPiece::~vtkTransmitRectilinearGridPiece() = default;

//----------------------------------------------------------------------------
void vtkTransmitRectilinearGridPiece::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
