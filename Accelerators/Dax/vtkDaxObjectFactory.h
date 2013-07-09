/*=========================================================================

  Program:   Visualization Toolkit
  Module:    ObjectFactory.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkDaxObjectFactory_h
#define __vtkDaxObjectFactory_h

#include "vtkObjectFactory.h"
#include "vtkObjectFactoryCollection.h" //required to make a factory
#include "vtkOverrideInformation.h" //required to make a factory
#include "vtkOverrideInformationCollection.h" //required to make a factory
#include "vtkVersion.h" //required to make a factory

#include "vtkDaxThreshold.h" //required to overload Threshold
VTK_CREATE_CREATE_FUNCTION(vtkDaxThreshold)

#include "vtkDaxMarchingCubes.h" //required to overload Marching Cubes
VTK_CREATE_CREATE_FUNCTION(vtkDaxMarchingCubes)

class VTK_EXPORT vtkDaxObjectFactory : public vtkObjectFactory
{
public:
  static vtkDaxObjectFactory* New();
  vtkDaxObjectFactory();
  virtual const char* GetVTKSourceVersion() { return VTK_SOURCE_VERSION; }
  const char* GetDescription() { return "Dax Object Factory"; }
  vtkTypeMacro(vtkDaxObjectFactory,vtkObjectFactory)

protected:
  vtkDaxObjectFactory(const vtkDaxObjectFactory&);
  void operator=(const vtkDaxObjectFactory&);
};


vtkDaxObjectFactory::vtkDaxObjectFactory()
{
  this->RegisterOverride("vtkThreshold",
                         "vtkDaxThreshold",
                         "Override threshold with Dax threshold version",
                         1,
                         vtkObjectFactoryCreatevtkDaxThreshold);

  this->RegisterOverride("vtkMarchingCubes",
                         "vtkDaxMarchingCubes",
                         "Override marching cubes Dax threshold version",
                         1,
                         vtkObjectFactoryCreatevtkDaxThreshold);
}

vtkStandardNewMacro(vtkDaxObjectFactory)
#endif