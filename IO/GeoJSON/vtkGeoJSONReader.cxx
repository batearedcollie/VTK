/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGeoJSONReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkGeoJSONReader.h"
#include "vtkGeoJSONFeature.h"

// VTK Includes
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPolyData.h"
#include "vtkStringArray.h"
#include "vtkTriangleFilter.h"

// C++ includes
#include <fstream>
#include <iostream>

vtkStandardNewMacro(vtkGeoJSONReader);

//----------------------------------------------------------------------------
vtkGeoJSONReader::vtkGeoJSONReader()
{
  this->FileName = NULL;
  this->StringInput = NULL;
  this->StringInputMode = false;
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkGeoJSONReader::~vtkGeoJSONReader()
{
  delete[] FileName;
}

//----------------------------------------------------------------------------
int vtkGeoJSONReader::CanParseFile(const char *filename, Json::Value &root)
{
  if (!filename)
    {
    vtkErrorMacro(<< "Input filename not specified");
    return VTK_ERROR;
    }

  ifstream file;
  file.open( filename );

  if ( ! file.is_open() )
    {
    vtkErrorMacro(<< "Unable to Open File " << this->FileName);
    return VTK_ERROR;
    }

  Json::Reader reader;

  //parse the entire geoJSON data into the Json::Value root
  bool parsedSuccess = reader.parse(file, root, false);

  if ( ! parsedSuccess )
    {
    // Report failures and their locations in the document
    vtkErrorMacro(<<"Failed to parse JSON" << endl << reader.getFormatedErrorMessages());
    return VTK_ERROR;
    }

  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkGeoJSONReader::CanParseString(char *input, Json::Value &root)
{
  if (!input)
    {
    vtkErrorMacro(<< "Input string is empty");
    return VTK_ERROR;
    }

  Json::Reader reader;

  //parse the entire geoJSON data into the Json::Value root
  bool parsedSuccess = reader.parse(input, root, false);

  if ( ! parsedSuccess )
    {
    // Report failures and their locations in the document
    vtkErrorMacro(<<"Failed to parse JSON" << endl << reader.getFormatedErrorMessages());
    return VTK_ERROR;
    }

  return VTK_OK;
}

//----------------------------------------------------------------------------
int vtkGeoJSONReader::RequestData(vtkInformation* vtkNotUsed(request),
                                   vtkInformationVector** vtkNotUsed(request),
                                   vtkInformationVector* outputVector)
{
  // Get the info object
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Get the ouptut
  vtkPolyData* output = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  // Parse either string input of file, depeding on mode
  Json::Value root;
  int parseResult = 0;
  if (this->StringInputMode)
    {
    parseResult = this->CanParseString(this->StringInput, root);
    }
  else
    {
    parseResult = this->CanParseFile(this->FileName, root);
    }

  if (parseResult != VTK_OK)
    {
    return VTK_ERROR;
    }

  // If parsed successfully into Json parser Values and Arrays, then convert it
  // into appropriate vtkPolyData
  if ( root.isObject() )
    {
    ParseRoot(root, output);

    //Convert Concave Polygons to convex polygons using triangulation
    vtkTriangleFilter *filter = vtkTriangleFilter::New();
    filter->SetInputData(output);
    filter->Update();

    output->ShallowCopy(filter->GetOutput());
    }
  return VTK_OK;
}

//----------------------------------------------------------------------------
void vtkGeoJSONReader::ParseRoot(Json::Value root, vtkPolyData *output)
{
  // Initialize geometry containers
  output->SetPoints(vtkPoints::New());
  output->SetVerts(vtkCellArray::New());
  output->SetLines(vtkCellArray::New());
  output->SetPolys(vtkCellArray::New());

  // Initialize feature-id array
  vtkStringArray *featureIdArray = vtkStringArray::New();
  featureIdArray->SetName("feature-id");
  output->GetCellData()->AddArray(featureIdArray);
  featureIdArray->Delete();

  Json::Value rootType = root["type"];
  if (rootType.isNull())
    {
    vtkErrorMacro(<<"ParseRoot: Missing type node");
    return;
    }

  Json::Value rootFeatures;
  std::string strRootType = rootType.asString();
  if ("FeatureCollection" == strRootType)
    {
    rootFeatures = root["features"];
    if (rootFeatures.isNull())
      {
      vtkErrorMacro(<<"ParseRoot: Missing \"features\" node");
      return;
      }

    if (!rootFeatures.isArray())
      {
      vtkErrorMacro(<< "ParseRoot: features node is not an array");
      return;
      }

    // Process features
    for (int i = 0; i < rootFeatures.size(); i++)
      {
      // Append extracted geometry to existing outputData
      Json::Value child = rootFeatures[i];
      vtkGeoJSONFeature *feature = vtkGeoJSONFeature::New();
      feature->ExtractGeoJSONFeature(child, output);
      }
    }
  else if ("Feature" == strRootType)
    {
    // Process single feature
    vtkGeoJSONFeature *feature = vtkGeoJSONFeature::New();
    feature->ExtractGeoJSONFeature(root, output);
    }
  else
    {
    vtkErrorMacro(<< "ParseRoot: do not support root type \""
                  << strRootType << "\"");
    }
}

//----------------------------------------------------------------------------
void vtkGeoJSONReader::PrintSelf(ostream &os, vtkIndent indent)
{
  Superclass::PrintSelf(os, indent);
  os << "vtkGeoJSONReader" << std::endl;
  os << "Filename: " << this->FileName << std::endl;
}
