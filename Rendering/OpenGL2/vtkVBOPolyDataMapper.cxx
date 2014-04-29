/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkVBOPolyDataMapper.h"

#include <GL/glew.h>

#include "vtkCommand.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkPointData.h"
#include "vtkCellArray.h"
#include "vtkVector.h"
#include "vtkProperty.h"
#include "vtkMatrix3x3.h"
#include "vtkMatrix4x4.h"
#include "vtkLookupTable.h"
#include "vtkCellData.h"

#include "vtkglBufferObject.h"
#include "vtkglShader.h"
#include "vtkglShaderProgram.h"

#include "vtkLight.h"
#include "vtkLightCollection.h"

#include <vector>

// Bring in our shader symbols.
#include "vtkglPolyDataVSLightKit.h"
#include "vtkglPolyDataVSHeadlight.h"
#include "vtkglPolyDataVSPositionalLights.h"
#include "vtkglPolyDataFS.h"

class vtkVBOPolyDataMapper::Private
{
public:
  vtkgl::BufferObject vbo;
  vtkgl::BufferObject ibo;
  size_t numberOfVertices;
  size_t numberOfIndices;

  const char *vertexShaderFile;
  const char *fragmentShaderFile;

  vtkgl::Shader vertexShader;
  vtkgl::Shader fragmentShader;
  vtkgl::ShaderProgram program;

  // Array of colors, along with the number of components.
  std::vector<unsigned char> colors;
  unsigned char colorComponents;
  bool colorAttributes;

  bool buidNormals;
  int interpolation;

  vtkTimeStamp propertiesTime;
  vtkTimeStamp shaderBuildTime;

  Private() : colorAttributes(false), buidNormals(true) { }
};

// Process the string, and return a version with replacements.
std::string replace(std::string source, const std::string &search,
                    const std::string replace, bool all = true)
{
  std::string::size_type pos = 0;
  bool first = true;
  while ((pos = source.find(search, 0)) != std::string::npos)
    {
    source.replace(pos, search.length(), replace);
    pos += search.length();
    if (first)
      {
      first = false;
      if (!all)
        {
        return source;
        }
      }
    }
  return source;
}

//-----------------------------------------------------------------------------
vtkStandardNewMacro(vtkVBOPolyDataMapper)

//-----------------------------------------------------------------------------
vtkVBOPolyDataMapper::vtkVBOPolyDataMapper()
  : Internal(new Private), UsingScalarColoring(false), Initialized(false)
{
}

//-----------------------------------------------------------------------------
vtkVBOPolyDataMapper::~vtkVBOPolyDataMapper()
{
  delete this->Internal;
}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::ReleaseGraphicsResources(vtkWindow*)
{
  // FIXME: Implement resource release.
}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::UpdateShader(vtkRenderer* ren, vtkActor *vtkNotUsed(actor))
{
  // first see if anything has changed, if not, just return
  // do this by checking lightcollection mtime

  // consider the lighting complexity to determine which case applies
  // simple headlight, Light Kit, the whole feature set of VTK
  int lightComplexity = 1;
  int numberOfLights = 0;
  vtkLightCollection *lc = ren->GetLights();
  vtkLight *light;

  vtkCollectionSimpleIterator sit;
  for(lc->InitTraversal(sit);
      (light = lc->GetNextLight(sit)); )
    {
    float status = light->GetSwitch();
    if (status > 0.0)
      {
      numberOfLights++;
      }

    if (lightComplexity == 1
        && (numberOfLights > 1
          || light->GetIntensity() != 1.0
          || light->GetLightType() != VTK_LIGHT_TYPE_HEADLIGHT))
      {
        lightComplexity = 2;
      }
    if (lightComplexity < 3
        && (light->GetPositional()))
      {
        lightComplexity = 3;
        break;
      }
    }

  // pick which shader code to use based on above factors
  switch (lightComplexity)
    {
    case 1:
        this->Internal->fragmentShaderFile = vtkglPolyDataFS;
        this->Internal->vertexShaderFile = vtkglPolyDataVSHeadlight;
//        this->Internal->vertexShaderFile = vtkglPolyDataVSPositionalLights;
      break;
    case 2:
        this->Internal->fragmentShaderFile = vtkglPolyDataFS;
//        this->Internal->vertexShaderFile = vtkglPolyDataVSHeadlight;
        this->Internal->vertexShaderFile = vtkglPolyDataVSLightKit;
//        this->Internal->vertexShaderFile = vtkglPolyDataVSPositionalLights;
      break;
    case 3:
        this->Internal->fragmentShaderFile = vtkglPolyDataFS;
        this->Internal->vertexShaderFile = vtkglPolyDataVSPositionalLights;
      break;
    }

  // compile and link the shader program if it has changed
  // eventually use some sort of caching here
  if (this->Internal->vertexShader.type() == vtkgl::Shader::Unknown ||
      this->Internal->propertiesTime > this->Internal->shaderBuildTime)
    {
    // Build our shader if necessary.
    std::string vertexShaderSource = this->Internal->vertexShaderFile;
    if (this->Internal->colorAttributes)
      {
      vertexShaderSource = replace(vertexShaderSource,
                                   "//VTK::Color::Dec",
                                   "attribute vec4 color;");
      }
    else
      {
      vertexShaderSource = replace(vertexShaderSource,
                                   "//VTK::Color::Dec",
                                   "uniform vec3 color;");
      }
    cout << "VS: " << vertexShaderSource << endl;

    this->Internal->vertexShader.setType(vtkgl::Shader::Vertex);
    this->Internal->vertexShader.setSource(vertexShaderSource);
    this->Internal->fragmentShader.setType(vtkgl::Shader::Fragment);
    this->Internal->fragmentShader.setSource(this->Internal->fragmentShaderFile);
    if (!this->Internal->vertexShader.compile())
      {
      vtkErrorMacro(<< this->Internal->vertexShader.error());
      }
    if (!this->Internal->fragmentShader.compile())
      {
      vtkErrorMacro(<< this->Internal->fragmentShader.error());
      }
    if (!this->Internal->program.attachShader(this->Internal->vertexShader))
      {
      vtkErrorMacro(<< this->Internal->program.error());
      }
    if (!this->Internal->program.attachShader(this->Internal->fragmentShader))
      {
      vtkErrorMacro(<< this->Internal->program.error());
      }
    if (!this->Internal->program.link())
      {
      vtkErrorMacro(<< this->Internal->program.error());
      }
    this->Internal->shaderBuildTime.Modified();
    }
}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::SetLightingShaderParameters(vtkRenderer* ren, vtkActor *vtkNotUsed(actor))
{
  // for headlight there are no lighting parameters
  if (this->Internal->vertexShaderFile == vtkglPolyDataVSHeadlight)
    {
      return;
    }

  // for lightkit case there are some parameters to set
  vtkCamera *cam = ren->GetActiveCamera();
  vtkTransform* viewTF = cam->GetModelViewTransformObject();

  // bind some light settings
  int numberOfLights = 0;
  vtkLightCollection *lc = ren->GetLights();
  vtkLight *light;

  vtkCollectionSimpleIterator sit;
  float lightColor[6][3];
  float lightDirection[6][3];
  for(lc->InitTraversal(sit);
      (light = lc->GetNextLight(sit)); )
    {
    float status = light->GetSwitch();
    if (status > 0.0)
      {
      double *dColor = light->GetDiffuseColor();
      double intensity = light->GetIntensity();
      lightColor[numberOfLights][0] = dColor[0] * intensity;
      lightColor[numberOfLights][1] = dColor[1] * intensity;
      lightColor[numberOfLights][2] = dColor[2] * intensity;
      // get required info from light
      double *lfp = light->GetTransformedFocalPoint();
      double *lp = light->GetTransformedPosition();
      double lightDir[3];
      vtkMath::Subtract(lfp,lp,lightDir);
      vtkMath::Normalize(lightDir);
      double *tDir = viewTF->TransformNormal(lightDir);
      lightDirection[numberOfLights][0] = tDir[0];
      lightDirection[numberOfLights][1] = tDir[1];
      lightDirection[numberOfLights][2] = tDir[2];
      numberOfLights++;
      }
    }
  this->Internal->program.setUniformValue("lightColor", numberOfLights, lightColor);
  this->Internal->program.setUniformValue("lightDirectionVC", numberOfLights, lightDirection);
  this->Internal->program.setUniformValue("numberOfLights", numberOfLights);

  if (this->Internal->vertexShaderFile == vtkglPolyDataVSLightKit)
    {
      return;
    }

  // if positional lights pass down more parameters
  float lightAttenuation[6][3];
  float lightPosition[6][3];
  float lightConeAngle[6];
  float lightExponent[6];
  int lightPositional[6];
  numberOfLights = 0;
  for(lc->InitTraversal(sit);
      (light = lc->GetNextLight(sit)); )
    {
    float status = light->GetSwitch();
    if (status > 0.0)
      {
      double *attn = light->GetAttenuationValues();
      lightAttenuation[numberOfLights][0] = attn[0];
      lightAttenuation[numberOfLights][1] = attn[1];
      lightAttenuation[numberOfLights][2] = attn[2];
      lightExponent[numberOfLights] = light->GetExponent();
      lightConeAngle[numberOfLights] = light->GetConeAngle();
      double *lp = light->GetTransformedPosition();
      lightPosition[numberOfLights][0] = lp[0];
      lightPosition[numberOfLights][1] = lp[1];
      lightPosition[numberOfLights][2] = lp[2];
      lightPositional[numberOfLights] = light->GetPositional();
      numberOfLights++;
      }
    }
  this->Internal->program.setUniformValue("lightAttenuation", numberOfLights, lightAttenuation);
  this->Internal->program.setUniformValue("lightPositional", numberOfLights, lightPositional);
  this->Internal->program.setUniformValue("lightPositionWC", numberOfLights, lightPosition);
  this->Internal->program.setUniformValue("lightExponent", numberOfLights, lightExponent);
  this->Internal->program.setUniformValue("lightConeAngle", numberOfLights, lightConeAngle);

}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::SetCameraShaderParameters(vtkRenderer* ren, vtkActor *actor)
{
  // pass down the various model and camera transformations
  vtkCamera *cam = ren->GetActiveCamera();
  // really just view  matrix in spite of it's name
  vtkTransform* viewTF = cam->GetModelViewTransformObject();
  this->Internal->program.setUniformValue("WCVCMatrix", viewTF->GetMatrix());

  // set the MCWC matrix
  this->Internal->program.setUniformValue("MCWCMatrix", actor->GetMatrix());

  // compute the combined ModelView matrix and send it down to save time in the shader
  vtkMatrix4x4 *tmpMat = vtkMatrix4x4::New();
  vtkMatrix4x4::Multiply4x4(viewTF->GetMatrix(), actor->GetMatrix(), tmpMat);
  tmpMat->Transpose();
  this->Internal->program.setUniformValue("MCVCMatrix", tmpMat);

  // set the normal matrix and send it down
  // (make this a function in camera at some point returning a 3x3)
  tmpMat->DeepCopy(cam->GetCameraLightTransformMatrix());
  vtkMatrix3x3 *tmpMat3d = vtkMatrix3x3::New();
  for(int i = 0; i < 3; ++i)
    {
    for (int j = 0; j < 3; ++j)
      {
        tmpMat3d->SetElement(i,j,tmpMat->GetElement(i,j));
      }
    }
  this->Internal->program.setUniformValue("normalMatrix", tmpMat3d);


  tmpMat->DeepCopy(cam->GetProjectionTransformMatrix(ren));
  this->Internal->program.setUniformValue("VCDCMatrix", tmpMat);

  tmpMat->Delete();
  tmpMat3d->Delete();
}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::SetPropertyShaderParameters(vtkRenderer* vtkNotUsed(ren), vtkActor *actor)
{
  // Query the actor for some of the properties that can be applied.
  float opacity = static_cast<float>(actor->GetProperty()->GetOpacity());
  double *dColor = actor->GetProperty()->GetDiffuseColor();
  double dIntensity = actor->GetProperty()->GetDiffuse();
  vtkgl::Vector3ub diffuseColor(static_cast<unsigned char>(dColor[0] * dIntensity * 255.0),
                         static_cast<unsigned char>(dColor[1] * dIntensity * 255.0),
                         static_cast<unsigned char>(dColor[2] * dIntensity * 255.0));
  double *sColor = actor->GetProperty()->GetSpecularColor();
  double sIntensity = actor->GetProperty()->GetSpecular();
  vtkgl::Vector3ub specularColor(static_cast<unsigned char>(sColor[0] * sIntensity * 255.0),
                         static_cast<unsigned char>(sColor[1] * sIntensity * 255.0),
                         static_cast<unsigned char>(sColor[2] * sIntensity * 255.0));
  float specularPower = actor->GetProperty()->GetSpecularPower();

  this->Internal->program.setUniformValue("opacity", opacity);
  this->Internal->program.setUniformValue("diffuseColor", diffuseColor);
  this->Internal->program.setUniformValue("specularColor", specularColor);
  this->Internal->program.setUniformValue("specularPower", specularPower);
}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::RenderPiece(vtkRenderer* ren, vtkActor *actor)
{
  vtkDataObject *input= this->GetInputDataObject(0, 0);

  // Make sure that we have been properly initialized.
  if (ren->GetRenderWindow()->CheckAbortStatus())
    {
    return;
    }

  if (input == NULL)
    {
    vtkErrorMacro(<< "No input!");
    return;
    }
  else
    {
    this->InvokeEvent(vtkCommand::StartEvent,NULL);
    if (!this->Static)
      {
      this->GetInputAlgorithm()->Update();
      }
    this->InvokeEvent(vtkCommand::EndEvent,NULL);
    }

  this->TimeToDraw = 0.0;

  // FIXME: This should be moved to the renderer, render window or similar.
  if (!this->Initialized)
    {
    GLenum result = glewInit();
    bool m_valid = (result == GLEW_OK);
    if (!m_valid)
      {
      vtkErrorMacro("GLEW could not be initialized.");
      return;
      }

    if (!GLEW_VERSION_2_1)
      {
      vtkErrorMacro("GL version 2.1 is not supported by your graphics driver.");
      //m_valid = false;
      return;
      }
    this->Initialized = true;
    }

  // Update the VBO if needed.
  if (this->VBOUpdateTime < this->GetMTime())
    {
    this->UpdateVBO(actor);
    this->VBOUpdateTime.Modified();
    }

  // Figure out and build the appropriate shader for the mapped geometry.
  this->UpdateShader(ren, actor);

  if (!this->Internal->program.bind())
    {
    vtkErrorMacro(<< this->Internal->program.error());
    return;
    }

  this->SetLightingShaderParameters(ren, actor);
  this->SetPropertyShaderParameters(ren, actor);
  this->SetCameraShaderParameters(ren, actor);

  this->Internal->vbo.bind();
  this->Internal->ibo.bind();

  size_t stride = sizeof(float) * 6;
  if (this->Internal->colorAttributes)
    {
    stride += sizeof(float);
    }

  this->Internal->program.enableAttributeArray("vertexMC");
  this->Internal->program.useAttributeArray("vertexMC", 0,
                                            stride,
                                            VTK_FLOAT, 3,
                                            vtkgl::ShaderProgram::NoNormalize);
  this->Internal->program.enableAttributeArray("normalMC");
  this->Internal->program.useAttributeArray("normalMC", sizeof(float) * 3,
                                            stride,
                                            VTK_FLOAT, 3,
                                            vtkgl::ShaderProgram::NoNormalize);
  if (this->Internal->colorAttributes)
    {
    if (!this->Internal->program.enableAttributeArray("color"))
      {
      vtkErrorMacro(<< this->Internal->program.error());
      }
    this->Internal->program.useAttributeArray("color", sizeof(float) * 6,
                                              stride,
                                              VTK_UNSIGNED_CHAR,
                                              this->Internal->colorComponents,
                                              vtkgl::ShaderProgram::NoNormalize);
    }

  // Render the loaded spheres using the shader and bound VBO.
  glDrawRangeElements(GL_TRIANGLES, 0,
                      static_cast<GLuint>(this->Internal->numberOfVertices - 1),
                      static_cast<GLsizei>(this->Internal->numberOfIndices),
                      GL_UNSIGNED_INT,
                      reinterpret_cast<const GLvoid *>(NULL));

  this->Internal->vbo.release();
  this->Internal->ibo.release();
  this->Internal->program.disableAttributeArray("vertexMC");
  this->Internal->program.disableAttributeArray("normalMC");
  if (this->Internal->colorAttributes)
    {
    this->Internal->program.disableAttributeArray("color");
    }
  this->Internal->program.release();

  // If the timer is not accurate enough, set it to a small
  // time so that it is not zero
  if (this->TimeToDraw == 0.0)
    {
    this->TimeToDraw = 0.0001;
    }

  this->UpdateProgress(1.0);
}

//-------------------------------------------------------------------------
void vtkVBOPolyDataMapper::ComputeBounds()
{
  if (!this->GetInput())
    {
    vtkMath::UninitializeBounds(this->Bounds);
    return;
    }
  this->GetInput()->GetBounds(this->Bounds);
}

//-------------------------------------------------------------------------
void vtkVBOPolyDataMapper::UpdateVBO(vtkActor *act)
{
  vtkPolyData *poly = this->GetInput();
  if (poly == NULL)// || !poly->GetPointData()->GetNormals())
    {
    return;
    }

  // This replicates how the painter decided on normal generation.
  int interpolation = act->GetProperty()->GetInterpolation();
  bool buildNormals = this->Internal->buidNormals;
  if (buildNormals)
    {
    buildNormals = ((poly->GetPointData()->GetNormals() && interpolation != VTK_FLAT) ||
                    poly->GetCellData()->GetNormals()) ? false : true;

    if (buildNormals)
      {
      // FIXME: Add implementation for normal generation.
      vtkErrorMacro("Asked to generate normals, needs to be implemented!");
      return;
      }
    }

  // Mark our properties as updated.
  this->Internal->propertiesTime.Modified();

  // Due to the requirement to use derived classes rather than typedefs for
  // the vtkVector types, it is simpler to add a few convenience typedefs here
  // than use the classes which are then harder for the compiler to interpret.
  typedef vtkVector<float,  3> Vector3f;
  typedef vtkVector<double, 3> Vector3d;

  bool colorAttributes = false;
  if (this->ScalarVisibility)
    {
    // We must figure out how the scalars should be mapped to the polydata.
    this->MapScalars(NULL, 1.0, false, poly);
    if (this->Internal->colorComponents == 3 ||
        this->Internal->colorComponents == 4)
      {
      this->Internal->colorAttributes = colorAttributes = true;
      cout << "Scalar colors: "
           << this->Internal->colors.size() / this->Internal->colorComponents
           << " with " << int(this->Internal->colorComponents) << " components." <<  endl;
      }
    }

  // Create a mesh packed with two vector 3f for vertex then normal.
  vtkDataArray* normals = poly->GetPointData()->GetNormals();
  if (normals->GetNumberOfTuples() != poly->GetNumberOfPoints())
    {
    vtkErrorMacro(<< "Polydata with bad normals.");
    }

  std::vector<float> packedMesh;
  packedMesh.reserve(poly->GetNumberOfPoints() * (colorAttributes ? 7 : 6));
  Vector3d tmp;
  for (vtkIdType i = 0; i < poly->GetNumberOfPoints(); ++i)
    {
    poly->GetPoint(i, tmp.GetData());
    for (int j = 0; j < 3; ++j)
      {
      packedMesh.push_back(tmp.Cast<float>()[j]);
      }
    normals->GetTuple(i, tmp.GetData());
    for (int j = 0; j < 3; ++j)
      {
      packedMesh.push_back(tmp.Cast<float>()[j]);
      }
    if (colorAttributes)
      {
      if (this->Internal->colorComponents == 4)
        {
        packedMesh.push_back(
              *reinterpret_cast<float *>(&this->Internal->colors[i * 4]));
        }
      else if (this->Internal->colorComponents == 3)
        {
        unsigned char c[4] = { this->Internal->colors[i * 4],
                               this->Internal->colors[i * 4 + 1],
                               this->Internal->colors[i * 4 + 2], 255 };
        packedMesh.push_back(*reinterpret_cast<float *>(&c));
        }
      }
    }

  vtkCellArray* polys = poly->GetPolys();
  std::vector<unsigned int> indexArray;
  indexArray.reserve(polys->GetNumberOfCells() * 3);
  for (vtkIdType i = 0; i < polys->GetNumberOfCells(); ++i)
    {
    vtkIdType* indices(NULL);
    vtkIdType num(0);
    // Each triangle cell is of length 4 (count, tri1, tri2, tri3).
    polys->GetCell(4 * i, num, indices);
    assert(num == 3);
    indexArray.push_back(static_cast<unsigned int>(indices[0]));
    indexArray.push_back(static_cast<unsigned int>(indices[1]));
    indexArray.push_back(static_cast<unsigned int>(indices[2]));
    }

  // Now we need to upload the two arrays to the GPU.
  this->Internal->vbo.upload(packedMesh, vtkgl::BufferObject::ArrayBuffer);
  this->Internal->ibo.upload(indexArray, vtkgl::BufferObject::ElementArrayBuffer);
  this->Internal->numberOfVertices = packedMesh.size() / 2;
  this->Internal->numberOfIndices = indexArray.size();

}

//-----------------------------------------------------------------------------
bool vtkVBOPolyDataMapper::GetIsOpaque()
{
  // Straight copy of what the vtkPainterPolyDataMapper was doing.
  if (this->ScalarVisibility &&
    this->ColorMode == VTK_COLOR_MODE_DEFAULT)
    {
    vtkPolyData* input =
      vtkPolyData::SafeDownCast(this->GetInputDataObject(0, 0));
    if (input)
      {
      int cellFlag;
      vtkDataArray* scalars = this->GetScalars(input,
        this->ScalarMode, this->ArrayAccessMode, this->ArrayId,
        this->ArrayName, cellFlag);
      if (scalars && scalars->IsA("vtkUnsignedCharArray") &&
        (scalars->GetNumberOfComponents() ==  4 /*(RGBA)*/ ||
         scalars->GetNumberOfComponents() == 2 /*(LuminanceAlpha)*/))
        {
        vtkUnsignedCharArray* colors =
          static_cast<vtkUnsignedCharArray*>(scalars);
        if ((colors->GetNumberOfComponents() == 4 && colors->GetValueRange(3)[0] < 255) ||
          (colors->GetNumberOfComponents() == 2 && colors->GetValueRange(1)[0] < 255))
          {
          // If the opacity is 255, despite the fact that the user specified
          // RGBA, we know that the Alpha is 100% opaque. So treat as opaque.
          return false;
          }
        }
      }
    }
  return this->Superclass::GetIsOpaque();
}

namespace
{
inline void vtkMultiplyColorsWithAlpha(vtkDataArray* array)
{
  vtkUnsignedCharArray* colors = vtkUnsignedCharArray::SafeDownCast(array);
  if (!colors || colors->GetNumberOfComponents() != 4)
    {
    return;
    }
  unsigned char* ptr = colors->GetPointer(0);
  vtkIdType numValues =
      colors->GetNumberOfTuples() * colors->GetNumberOfComponents();
  if (numValues <= 4)
    {
    return;
    }
  for (vtkIdType cc = 0; cc < numValues; cc += 4, ptr += 4)
    {
    double alpha = (0x0ff & ptr[3]) / 255.0;
    ptr[0] = static_cast<unsigned char>(0x0ff &
                                        static_cast<int>((0x0ff &
                                                          ptr[0]) * alpha));
    ptr[1] = static_cast<unsigned char>(0x0ff &
                                        static_cast<int>((0x0ff &
                                                          ptr[1]) * alpha));
    ptr[2] = static_cast<unsigned char>(0x0ff &
                                        static_cast<int>((0x0ff &
                                                          ptr[2]) * alpha));
    }
}
}

//-----------------------------------------------------------------------------
// This method has the same functionality as the old vtkMapper::MapScalars.
void vtkVBOPolyDataMapper::MapScalars(vtkDataSet*, double alpha,
                                      bool multiplyWithAlpha, vtkDataSet* input)
{
  int cellFlag;
  double origAlpha;
  vtkDataArray* scalars = vtkAbstractMapper::GetScalars(input,
    this->ScalarMode, this->ArrayAccessMode, this->ArrayId,
    this->ArrayName, cellFlag);

  int arraycomponent = this->ArrayComponent;
  // This is for a legacy feature: selection of the array component to color by
  // from the mapper.  It is now in the lookuptable.  When this feature
  // is removed, we can remove this condition.
  if (scalars == 0 || scalars->GetNumberOfComponents() <= this->ArrayComponent)
    {
    arraycomponent = 0;
    }

  if (!this->ScalarVisibility || scalars == 0 || input == 0)
    {
    return;
    }

  // Let subclasses know that scalar coloring was employed in the current pass.
  this->UsingScalarColoring = true;
  if (this->ColorTextureMap)
    {
    /// FIXME: Implement, or move this.
    // Implies that we have verified that we must use texture map for scalar
    // coloring. Just create texture coordinates for the input dataset.
    //this->MapScalarsToTexture(output, scalars, input);
    return;
    }

  vtkScalarsToColors* lut = 0;
  // Get the lookup table.
  if (scalars->GetLookupTable())
    {
    lut = scalars->GetLookupTable();
    }
  else
    {
    lut = this->GetLookupTable();
    lut->Build();
    }

  if (!this->UseLookupTableScalarRange)
    {
    lut->SetRange(this->ScalarRange);
    }

  // The LastUsedAlpha checks ensures that opacity changes are reflected
  // correctly when this->MapScalars(..) is called when iterating over a
  // composite dataset.
  /*if (colors &&
    this->LastUsedAlpha == alpha &&
    this->LastUsedMultiplyWithAlpha == multiplyWithAlpha)
    {
    if (this->GetMTime() < colors->GetMTime() &&
      input->GetMTime() < colors->GetMTime() &&
      lut->GetMTime() < colors->GetMTime())
      {
      // using old colors.
      return;
      }
    }*/

  // Get rid of old colors.
  vtkDataArray *colors = 0;
  origAlpha = lut->GetAlpha();
  lut->SetAlpha(alpha);
  colors = lut->MapScalars(scalars, this->ColorMode, arraycomponent);
  lut->SetAlpha(origAlpha);
  if (multiplyWithAlpha)
    {
    // It is possible that the LUT simply returns the scalars as the
    // colors. In which case, we allocate a new array to ensure
    // that we don't modify the array in the input.
    if (scalars == colors)
      {
      // Since we will be changing the colors array
      // we create a copy.
      colors->Delete();
      colors = scalars->NewInstance();
      colors->DeepCopy(scalars);
      }
    vtkMultiplyColorsWithAlpha(colors);
    }

  vtkUnsignedCharArray* colorArray = vtkUnsignedCharArray::SafeDownCast(colors);
  if (!colorArray)
    {
    vtkErrorMacro("Error: color array not of type unsigned char...");
    return;
    }
  unsigned char* ptr = colorArray->GetPointer(0);
  vtkIdType numValues =
      colorArray->GetNumberOfTuples() * colorArray->GetNumberOfComponents();
  this->Internal->colorComponents = colorArray->GetNumberOfComponents();
  this->Internal->colors.reserve(numValues);
  this->Internal->colors.assign(ptr, ptr + numValues);

  colors->Delete();
}

//-----------------------------------------------------------------------------
void vtkVBOPolyDataMapper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
