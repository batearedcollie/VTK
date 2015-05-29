/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef vtkGLVBOHelpher_h
#define vtkGLVBOHelpher_h

#include "vtkRenderingOpenGL2Module.h" // for export macro
#include "vtkTimeStamp.h"
#include <vector> // used for struct ivars

class vtkCellArray;
class vtkDataArray;
class vtkOpenGLBufferObject;
class vtkOpenGLShaderCache;
class vtkOpenGLVertexArrayObject;
class vtkPoints;
class vtkPolyData;
class vtkShaderProgram;
class vtkWindow;

namespace vtkgl
{

// useful union for stuffing colors into a float
union vtkucfloat
{
  unsigned char c[4];
  float f;
};


// Process the string, and return a version with replacements.
std::string VTKRENDERINGOPENGL2_EXPORT replace(std::string source,
  const std::string &search,
  const std::string replace, bool all = true);

// perform in place string substitutions, indicate if a substitution was done
bool VTKRENDERINGOPENGL2_EXPORT substitute(std::string &source,
  const std::string &search,
  const std::string replace, bool all = true);

// used to create an IBO for triangle primatives
size_t CreateTriangleIndexBuffer(vtkCellArray *cells,
  vtkOpenGLBufferObject *indexBuffer,
  vtkPoints *points);

// used to create an IBO for triangle primatives
void AppendTriangleIndexBuffer(
  std::vector<unsigned int> &indexArray,
  vtkCellArray *cells,
  vtkPoints *points,
  vtkIdType vertexOffset);

// create a IBO for wireframe polys/tris
size_t CreateTriangleLineIndexBuffer(vtkCellArray *cells,
  vtkOpenGLBufferObject *indexBuffer);

// used to create an IBO for line primatives
void AppendLineIndexBuffer(
  std::vector<unsigned int> &indexArray,
  vtkCellArray *cells,
  vtkPoints *points,
  vtkIdType vertexOffset);

// create a IBO for wireframe polys/tris
size_t CreateLineIndexBuffer(vtkCellArray *cells,
  vtkOpenGLBufferObject *indexBuffer);

// create a IBO for wireframe polys/tris
void AppendTriangleLineIndexBuffer(
  std::vector<unsigned int> &indexArray,
  vtkCellArray *cells,
  vtkIdType vertexOffset);

// used to create an IBO for primatives as points
size_t CreatePointIndexBuffer(vtkCellArray *cells,
  vtkOpenGLBufferObject *indexBuffer);

// used to create an IBO for primatives as points
void AppendPointIndexBuffer(
  std::vector<unsigned int> &indexArray,
  vtkCellArray *cells,
  vtkIdType vertexOffset);

// used to create an IBO for line strips and triangle strips
size_t CreateStripIndexBuffer(vtkCellArray *cells,
  vtkOpenGLBufferObject *indexBuffer,
  bool wireframeTriStrips);

// special index buffer for polys wireframe with edge visibilityflags
size_t CreateEdgeFlagIndexBuffer(vtkCellArray *cells,
  vtkOpenGLBufferObject *indexBuffer,
  vtkDataArray *edgeflags);

// Store the shaders, program, and ibo in a common struct.
class VTKRENDERINGOPENGL2_EXPORT CellBO
{
public:
  vtkShaderProgram *Program;
  vtkOpenGLBufferObject *IBO;
  vtkOpenGLVertexArrayObject *VAO;
  vtkTimeStamp ShaderSourceTime;
  size_t IndexCount;
  vtkTimeStamp AttributeUpdateTime;

  CellBO();
  ~CellBO();
  void ReleaseGraphicsResources(vtkWindow *win);
};

// Sizes/offsets are all in bytes as OpenGL API expects them.
struct VBOLayout
{
  size_t VertexCount; // Number of vertices in the VBO
  int Stride;       // The size of a complete vertex + attributes
  int VertexOffset; // Offset of the vertex
  int NormalOffset; // Offset of the normal
  int TCoordOffset; // Offset of the texture coordinates
  int TCoordComponents; // Number of texture dimensions
  int ColorOffset;  // Offset of the color
  int ColorComponents; // Number of color components
  std::vector<float> PackedVBO; // the data
};

// Take the points, and pack them into the VBO object supplied. This currently
// takes whatever the input type might be and packs them into a VBO using
// floats for the vertices and normals, and unsigned char for the colors (if
// the array is non-null).
VBOLayout CreateVBO(vtkPoints *points, unsigned int numPoints,
    vtkDataArray *normals,
    vtkDataArray *tcoords,
    unsigned char *colors, int colorComponents,
    vtkOpenGLBufferObject *vertexBuffer);
void AppendVBO(VBOLayout &layout, vtkPoints *points, unsigned int numPoints,
    vtkDataArray *normals,
    vtkDataArray *tcoords,
    unsigned char *colors, int colorComponents);

// used to create an IBO for stripped primatives such as lines and strips
void CreateCellSupportArrays(vtkCellArray *[4],
                             std::vector<unsigned int> &cellCellMap,
                             int representation);

} // End namespace

#endif // vtkGLVBOHelpher_h

// VTK-HeaderTest-Exclude: vtkglVBOHelper.h
