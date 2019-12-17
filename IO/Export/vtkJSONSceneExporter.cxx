/*=========================================================================

  Program:   VisualizationJSONlkit
  Module:    vtkJSONSceneExporter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkJSONSceneExporter.h"

#include "vtkCamera.h"
#include "vtkCompositeDataIterator.h"
#include "vtkCompositeDataSet.h"
#include "vtkDataObject.h"
#include "vtkDataSet.h"
#include "vtkDiscretizableColorTransferFunction.h"
#include "vtkExporter.h"
#include "vtkImageData.h"
#include "vtkImageResize.h"
#include "vtkJPEGWriter.h"
#include "vtkJSONDataSetWriter.h"
#include "vtkMapper.h"
#include "vtkNew.h"
#include "vtkObjectFactory.h"
#include "vtkProp.h"
#include "vtkPropCollection.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkRendererCollection.h"
#include "vtkScalarsToColors.h"
#include "vtkSmartPointer.h"
#include "vtkTexture.h"
#include "vtksys/FStream.hxx"
#include "vtksys/SystemTools.hxx"

#include <fstream>
#include <sstream>
#include <string>

vtkStandardNewMacro(vtkJSONSceneExporter);

// ----------------------------------------------------------------------------

vtkJSONSceneExporter::vtkJSONSceneExporter()
{
  this->FileName = nullptr;
  this->WriteTextures = false;
  this->WriteTextureLODs = false;
  this->TextureLODsBaseSize = 100000;
  this->TextureLODsBaseUrl = nullptr;
}

// ----------------------------------------------------------------------------

vtkJSONSceneExporter::~vtkJSONSceneExporter()
{
  delete[] this->FileName;
}

// ----------------------------------------------------------------------------

void vtkJSONSceneExporter::WriteDataObject(ostream& os, vtkDataObject* dataObject, vtkActor* actor)
{
  // Skip if nothing to process
  if (dataObject == nullptr)
  {
    return;
  }

  // Handle Dataset
  if (dataObject->IsA("vtkDataSet"))
  {
    std::string texturesString;
    if (this->WriteTextures && actor->GetTexture())
    {
      // Write out the textures, add it to the textures string
      texturesString += this->WriteTexture(actor->GetTexture());
    }

    if (this->WriteTextureLODs && actor->GetTexture())
    {
      // Write out the texture LODs, add it to the textures string
      texturesString += this->WriteTextureLODSeries(actor->GetTexture());
    }

    std::string renderingSetup = this->ExtractRenderingSetup(actor);
    std::string addOnMeta = renderingSetup + texturesString + "\n";
    std::string dsMeta =
      this->WriteDataSet(vtkDataSet::SafeDownCast(dataObject), addOnMeta.c_str());
    if (!dsMeta.empty())
    {
      os << dsMeta;
    }
    return;
  }

  // Handle composite
  if (dataObject->IsA("vtkCompositeDataSet"))
  {
    vtkCompositeDataSet* composite = vtkCompositeDataSet::SafeDownCast(dataObject);
    vtkSmartPointer<vtkCompositeDataIterator> iter = composite->NewIterator();
    iter->SkipEmptyNodesOn();
    iter->InitTraversal();
    while (!iter->IsDoneWithTraversal())
    {
      this->WriteDataObject(os, iter->GetCurrentDataObject(), actor);
      iter->GoToNextItem();
    }
  }
}

// ----------------------------------------------------------------------------

std::string vtkJSONSceneExporter::ExtractRenderingSetup(vtkActor* actor)
{
  vtkMapper* mapper = actor->GetMapper();
  // int scalarVisibility = mapper->GetScalarVisibility();
  const char* colorArrayName = mapper->GetArrayName();
  int colorMode = mapper->GetColorMode();
  int scalarMode = mapper->GetScalarMode();

  vtkProperty* property = actor->GetProperty();
  int representation = property->GetRepresentation();
  double* colorToUse = property->GetDiffuseColor();
  if (representation == 1)
  {
    colorToUse = property->GetColor();
  }
  int pointSize = property->GetPointSize();
  float opacity = property->GetOpacity();
  int edgeVisibility = property->GetEdgeVisibility();

  double* p3dPosition = actor->GetPosition();
  double* p3dScale = actor->GetScale();
  double* p3dOrigin = actor->GetOrigin();
  double* p3dRotateWXYZ = actor->GetOrientationWXYZ();

  const char* INDENT = "      ";
  std::stringstream renderingConfig;
  renderingConfig << ",\n"
                  << INDENT << "\"actor\": {\n"
                  << INDENT << "  \"origin\": [" << p3dOrigin[0] << ", " << p3dOrigin[1] << ", "
                  << p3dOrigin[2] << "],\n"
                  << INDENT << "  \"scale\": [" << p3dScale[0] << ", " << p3dScale[1] << ", "
                  << p3dScale[2] << "],\n"
                  << INDENT << "  \"position\": [" << p3dPosition[0] << ", " << p3dPosition[1]
                  << ", " << p3dPosition[2] << "]\n"
                  << INDENT << "},\n"
                  << INDENT << "\"actorRotation\": [" << p3dRotateWXYZ[0] << ", "
                  << p3dRotateWXYZ[1] << ", " << p3dRotateWXYZ[2] << ", " << p3dRotateWXYZ[3]
                  << "],\n"
                  << INDENT << "\"mapper\": {\n"
                  << INDENT << "  \"colorByArrayName\": \"" << colorArrayName << "\",\n"
                  << INDENT << "  \"colorMode\": " << colorMode << ",\n"
                  << INDENT << "  \"scalarMode\": " << scalarMode << "\n"
                  << INDENT << "},\n"
                  << INDENT << "\"property\": {\n"
                  << INDENT << "  \"representation\": " << representation << ",\n"
                  << INDENT << "  \"edgeVisibility\": " << edgeVisibility << ",\n"
                  << INDENT << "  \"diffuseColor\": [" << colorToUse[0] << ", " << colorToUse[1]
                  << ", " << colorToUse[2] << "],\n"
                  << INDENT << "  \"pointSize\": " << pointSize << ",\n"
                  << INDENT << "  \"opacity\": " << opacity << "\n"
                  << INDENT << "}";

  return renderingConfig.str();
}

// ----------------------------------------------------------------------------

std::string vtkJSONSceneExporter::CurrentDataSetPath() const
{
  std::stringstream path;
  path << this->FileName << "/" << this->DatasetCount + 1;
  return vtksys::SystemTools::ConvertToOutputPath(path.str());
}

// ----------------------------------------------------------------------------

std::string vtkJSONSceneExporter::WriteDataSet(vtkDataSet* dataset, const char* addOnMeta = nullptr)
{
  if (!dataset)
  {
    return "";
  }

  std::string dsPath = this->CurrentDataSetPath();
  ++this->DatasetCount;

  vtkNew<vtkJSONDataSetWriter> dsWriter;
  dsWriter->SetInputData(dataset);
  dsWriter->SetFileName(dsPath.c_str());
  dsWriter->Write();

  if (!dsWriter->IsDataSetValid())
  {
    this->DatasetCount--;
    return "";
  }

  std::stringstream meta;
  if (this->DatasetCount > 1)
  {
    meta << ",\n";
  }
  else
  {
    meta << "\n";
  }
  const char* INDENT = "    ";
  meta << INDENT << "{\n"
       << INDENT << "  \"name\": \"" << this->DatasetCount << "\",\n"
       << INDENT << "  \"type\": \"httpDataSetReader\",\n"
       << INDENT << "  \"httpDataSetReader\": { \"url\": \"" << this->DatasetCount << "\" }";

  if (addOnMeta != nullptr)
  {
    meta << addOnMeta;
  }

  meta << INDENT << "}";

  return meta.str();
}

// ----------------------------------------------------------------------------

void vtkJSONSceneExporter::WriteLookupTable(const char* name, vtkScalarsToColors* lookupTable)
{
  if (lookupTable == nullptr)
  {
    return;
  }

  vtkDiscretizableColorTransferFunction* dctfn =
    vtkDiscretizableColorTransferFunction::SafeDownCast(lookupTable);
  if (dctfn != nullptr)
  {
    const char* INDENT = "    ";
    std::stringstream lutJSON;
    lutJSON << "{\n"
            << INDENT << "  \"clamping\": " << (dctfn->GetClamping() ? "true" : "false") << ",\n"
            << INDENT << "  \"colorSpace\": " << dctfn->GetColorSpace() << ",\n"
            << INDENT << "  \"hSVWrap\": " << (dctfn->GetHSVWrap() ? "true" : "false") << ",\n"
            << INDENT << "  \"alpha\": " << dctfn->GetAlpha() << ",\n"
            << INDENT << "  \"vectorComponent\": " << dctfn->GetVectorComponent() << ",\n"
            << INDENT << "  \"vectorSize\": " << dctfn->GetVectorSize() << ",\n"
            << INDENT << "  \"vectorMode\": " << dctfn->GetVectorMode() << ",\n"
            << INDENT << "  \"indexedLookup\": " << dctfn->GetIndexedLookup() << ",\n"
            << INDENT << "  \"nodes\": [";

    // Fill nodes
    vtkIdType nbNodes = dctfn->GetSize();
    double node[6];
    for (vtkIdType i = 0; i < nbNodes; i++)
    {
      dctfn->GetNodeValue(i, node);
      if (i > 0)
      {
        lutJSON << ",";
      }

      lutJSON << "\n"
              << INDENT << INDENT << "[" << node[0] << ", " << node[1] << ", " << node[2] << ", "
              << node[3] << ", " << node[4] << ", " << node[5] << "]";
    }

    // Close node list
    lutJSON << "\n      ]\n    }";

    // Store in map...
    this->LookupTables[name] = lutJSON.str();
  }
}

// ----------------------------------------------------------------------------

void vtkJSONSceneExporter::WriteData()
{
  this->DatasetCount = 0;
  this->TextureStrings.clear();
  this->TextureLODStrings.clear();

  // make sure the user specified a FileName or FilePointer
  if (this->FileName == nullptr)
  {
    vtkErrorMacro(<< "Please specify FileName to use");
    return;
  }

  if (!vtksys::SystemTools::MakeDirectory(this->FileName))
  {
    vtkErrorMacro(<< "Can not create directory " << this->FileName);
    return;
  }

  vtkRenderer* renderer = this->GetActiveRenderer();
  if (!renderer)
  {
    renderer = this->RenderWindow->GetRenderers()->GetFirstRenderer();
  }
  vtkCamera* cam = renderer->GetActiveCamera();

  std::stringstream sceneComponents;
  vtkPropCollection* renProps = renderer->GetViewProps();
  vtkIdType nbProps = renProps->GetNumberOfItems();
  for (vtkIdType rpIdx = 0; rpIdx < nbProps; rpIdx++)
  {
    vtkProp* renProp = vtkProp::SafeDownCast(renProps->GetItemAsObject(rpIdx));

    // Skip non-visible actors
    if (!renProp || !renProp->GetVisibility())
    {
      continue;
    }

    // Skip actors with no geometry
    vtkActor* actor = vtkActor::SafeDownCast(renProp);
    if (!actor)
    {
      continue;
    }

    vtkMapper* mapper = actor->GetMapper();
    vtkDataObject* dataObject = mapper->GetInputDataObject(0, 0);
    this->WriteDataObject(sceneComponents, dataObject, actor);
    this->WriteLookupTable(mapper->GetArrayName(), mapper->GetLookupTable());
  }

  std::stringstream sceneJsonFile;
  sceneJsonFile << "{\n"
                << "  \"version\": 1.0,\n"
                << "  \"background\": [" << renderer->GetBackground()[0] << ", "
                << renderer->GetBackground()[1] << ", " << renderer->GetBackground()[2] << "],\n"
                << "  \"camera\": {\n"
                << "    \"focalPoint\": [" << cam->GetFocalPoint()[0] << ", "
                << cam->GetFocalPoint()[1] << ", " << cam->GetFocalPoint()[2] << "],\n"
                << "    \"position\": [" << cam->GetPosition()[0] << ", " << cam->GetPosition()[1]
                << ", " << cam->GetPosition()[2] << "],\n"
                << "    \"viewUp\": [" << cam->GetViewUp()[0] << ", " << cam->GetViewUp()[1] << ", "
                << cam->GetViewUp()[2] << "]\n"
                << "  },\n"
                << "  \"centerOfRotation\": [" << cam->GetFocalPoint()[0] << ", "
                << cam->GetFocalPoint()[1] << ", " << cam->GetFocalPoint()[2] << "],\n"
                << "  \"scene\": [" << sceneComponents.str() << "\n  ],\n"
                << "  \"lookupTables\": {\n";

  // Inject lookup table
  size_t nbLuts = this->LookupTables.size();
  for (auto const& lut : this->LookupTables)
  {
    sceneJsonFile << "    \"" << lut.first.c_str() << "\": " << lut.second.c_str()
                  << (--nbLuts ? "," : "") << "\n";
  }

  sceneJsonFile << "  }\n"
                << "}\n";

  // Write meta-data file
  std::stringstream scenePath;
  scenePath << this->FileName << "/index.json";

  vtksys::ofstream file;
  file.open(scenePath.str().c_str(), ios::out);
  file << sceneJsonFile.str().c_str();
  file.close();
}

namespace
{

// ----------------------------------------------------------------------------

size_t getFileSize(const std::string& path)
{
  // TODO: This function gives me slightly different sizes than what my
  // filesystem gives me. Find out why.
  // For instance, I get 240MB for a 230MB file.
  // Maybe we can say "it's close enough" for now, though...
  vtksys::SystemTools::Stat_t stat_buf;
  int res = vtksys::SystemTools::Stat(path, &stat_buf);
  if (res < 0)
  {
    std::cerr << "Failed to get size of file " << path.c_str() << std::endl;
    return 0;
  }

  return stat_buf.st_size;
}

} // end anon namespace

// ----------------------------------------------------------------------------

std::string vtkJSONSceneExporter::WriteTexture(vtkTexture* texture)
{
  // If this texture has already been written, just re-use the one
  // we have.
  if (this->TextureStrings.find(texture) != this->TextureStrings.end())
  {
    return this->TextureStrings[texture];
  }

  std::string path = this->CurrentDataSetPath();

  // Make sure it exists
  if (!vtksys::SystemTools::MakeDirectory(path))
  {
    vtkErrorMacro(<< "Cannot create directory " << path);
    return "";
  }

  path += "/texture.jpg";
  path = vtksys::SystemTools::ConvertToOutputPath(path);

  vtkSmartPointer<vtkImageData> image = texture->GetInput();

  vtkNew<vtkJPEGWriter> writer;
  writer->SetFileName(path.c_str());
  writer->SetInputDataObject(image);
  writer->Write();

  const char* INDENT = "      ";
  std::stringstream config;
  config << ",\n" << INDENT << "\"texture\": \"" << this->DatasetCount + 1 << "/texture.jpg\"";
  this->TextureStrings[texture] = config.str();
  return config.str();
}

// ----------------------------------------------------------------------------

std::string vtkJSONSceneExporter::WriteTextureLODSeries(vtkTexture* texture)
{
  // If this texture has already been written, just re-use the one
  // we have.
  if (this->TextureLODStrings.find(texture) != this->TextureLODStrings.end())
  {
    return this->TextureLODStrings[texture];
  }

  std::vector<std::string> files;

  std::string name = "texture";
  std::string ext = ".jpg";

  vtkSmartPointer<vtkImageData> image = texture->GetInput();
  int dims[3];
  image->GetDimensions(dims);

  // Write these into the parent directory of our file.
  // This next line also converts the path to unix slashes.
  std::string path = vtksys::SystemTools::GetParentDirectory(this->FileName);
  path += "/";
  path = vtksys::SystemTools::ConvertToOutputPath(path);

  while (true)
  {
    // Name is "<name>_<dataset_number>-<width>x<height><ext>"
    // For example, "texture_1-256x256.jpg"
    std::stringstream full_name;
    full_name << name << "_" << std::to_string(this->DatasetCount + 1) << "-"
              << std::to_string(dims[0]) << "x" << std::to_string(dims[1]) << ext;
    std::string full_path = path + full_name.str();

    vtkNew<vtkJPEGWriter> writer;
    writer->SetFileName(full_path.c_str());
    writer->SetInputDataObject(image);
    writer->Write();

    files.push_back(full_name.str());

    size_t size = getFileSize(full_path);
    if (size <= this->TextureLODsBaseSize || (dims[0] == 1 && dims[1] == 1))
    {
      // We are done...
      break;
    }

    // Shrink the image and go again
    vtkNew<vtkImageResize> shrink;
    shrink->SetInputData(image);
    dims[0] = dims[0] > 1 ? dims[0] / 2 : 1;
    dims[1] = dims[1] > 1 ? dims[1] / 2 : 1;
    shrink->SetOutputDimensions(dims[0], dims[1], 1);
    shrink->Update();
    image = shrink->GetOutput();
  }

  const char* url = this->TextureLODsBaseUrl;
  std::string baseUrl = url ? url : "";

  // Now, write out the config
  const char* INDENT = "      ";
  std::stringstream config;
  config << ",\n"
         << INDENT << "\"textureLODs\": {\n"
         << INDENT << "  \"baseUrl\": \"" << baseUrl << "\",\n"
         << INDENT << "  \"files\": [\n";

  // Reverse the order of the files so the smallest comes first
  std::reverse(files.begin(), files.end());
  for (size_t i = 0; i < files.size(); ++i)
  {
    config << INDENT << "    \"" << files[i] << "\"";
    if (i != files.size() - 1)
    {
      config << ",\n";
    }
    else
    {
      config << "\n";
    }
  }

  config << INDENT << "  ]\n" << INDENT << "}";

  this->TextureLODStrings[texture] = config.str();
  return config.str();
}

// ----------------------------------------------------------------------------

void vtkJSONSceneExporter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
