/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkArchiver.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkArchiver.h"

#include <vtkObjectFactory.h>
#include <vtksys/SystemTools.hxx>

#include <fstream>
#include <sstream>

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkArchiver);

//----------------------------------------------------------------------------
vtkArchiver::vtkArchiver()
{
  this->ArchiveName = nullptr;
}

//----------------------------------------------------------------------------
vtkArchiver::~vtkArchiver()
{
  this->SetArchiveName(nullptr);
}

//----------------------------------------------------------------------------
void vtkArchiver::OpenArchive()
{
  if (this->ArchiveName == nullptr)
  {
    vtkErrorMacro(<< "Please specify ArchiveName to use");
    return;
  }

  if (!vtksys::SystemTools::MakeDirectory(this->ArchiveName))
  {
    vtkErrorMacro(<< "Can not create directory " << this->ArchiveName);
    return;
  }
}

//----------------------------------------------------------------------------
void vtkArchiver::CloseArchive() {}

//----------------------------------------------------------------------------
void vtkArchiver::InsertIntoArchive(
  const std::string& relativePath, const char* data, std::streamsize size)
{
  std::stringstream path;
  path << this->ArchiveName << "/" << relativePath;

  vtksys::SystemTools::MakeDirectory(vtksys::SystemTools::GetFilenamePath(path.str()));

  std::ofstream out(path.str().c_str(), std::ios::out | std::ios::binary);
  out.write(data, size);
  out.close();
}

//----------------------------------------------------------------------------
void vtkArchiver::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
