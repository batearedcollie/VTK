/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPLYReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPLYReader.h"

#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkPointData.h"
#include "vtkFloatArray.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkPLY.h"
#include "vtkPolyData.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include <vtksys/SystemTools.hxx>

#include <cctype>
#include <cstddef>

vtkStandardNewMacro(vtkPLYReader);


// Construct object with merging set to true.
vtkPLYReader::vtkPLYReader()
{
  this->Comments = vtkStringArray::New();
}

vtkPLYReader::~vtkPLYReader()
{
  this->Comments->Delete();
  this->Comments = nullptr;
}

namespace { //required so we don't violate ODR
typedef struct _plyVertex {
  float x[3];             // the usual 3-space position of a vertex
  float tex[2];
  float normal[3];
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
} plyVertex;

typedef struct _plyFace {
  unsigned char intensity; // optional face attributes
  unsigned char red;
  unsigned char green;
  unsigned char blue;
  unsigned char alpha;
  unsigned char nverts;   // number of vertex indices in list
  int *verts;             // vertex index list
  unsigned char ntexcoord;   // number of texcoord in list
  float *texcoord;             // texcoord list
} plyFace;
}

int vtkPLYReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **vtkNotUsed(inputVector),
  vtkInformationVector *outputVector)
{
  // get the info object
  vtkInformation *outInfo = outputVector->GetInformationObject(0);

  // get the output
  vtkPolyData *output = vtkPolyData::SafeDownCast(
    outInfo->Get(vtkDataObject::DATA_OBJECT()));

  PlyProperty vertProps[] = {
    { "x", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, x)), 0, 0, 0, 0 },
    { "y", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, x) + sizeof(float)), 0, 0, 0,
      0 },
    { "z", PLY_FLOAT, PLY_FLOAT,
      static_cast<int>(offsetof(plyVertex, x) + sizeof(float) + sizeof(float)), 0, 0, 0, 0 },
    { "u", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, tex)), 0, 0, 0, 0 },
    { "v", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, tex) + sizeof(float)), 0, 0,
      0, 0 },
    { "nx", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, normal)), 0, 0, 0, 0 },
    { "ny", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, normal) + sizeof(float)), 0,
      0, 0, 0 },
    { "nz", PLY_FLOAT, PLY_FLOAT, static_cast<int>(offsetof(plyVertex, normal) + 2 * sizeof(float)),
      0, 0, 0, 0 },
    { "red", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex, red)), 0, 0, 0, 0 },
    { "green", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex, green)), 0, 0, 0, 0 },
    { "blue", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex, blue)), 0, 0, 0, 0 },
    { "alpha", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyVertex, alpha)), 0, 0, 0, 0 },
  };
  PlyProperty faceProps[] = {
    { "vertex_indices", PLY_INT, PLY_INT, static_cast<int>(offsetof(plyFace, verts)), 1, PLY_UCHAR,
      PLY_UCHAR, static_cast<int>(offsetof(plyFace, nverts)) },
    { "intensity", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace, intensity)), 0, 0, 0,
      0 },
    { "red", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace, red)), 0, 0, 0, 0 },
    { "green", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace, green)), 0, 0, 0, 0 },
    { "blue", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace, blue)), 0, 0, 0, 0 },
    { "alpha", PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace, alpha)), 0, 0, 0, 0 },
    { "texcoord", PLY_FLOAT, PLY_FLOAT,
     static_cast<int>(offsetof(plyFace,texcoord)),
     1, PLY_UCHAR, PLY_UCHAR, static_cast<int>(offsetof(plyFace,ntexcoord))},
  };

  if (!this->FileName)
  {
    vtkErrorMacro(<<"A File Name must be specified.");
    return 0;
  }

  // open a PLY file for reading
  PlyFile *ply;
  int nelems, fileType, numElems, nprops;
  char **elist, *elemName;
  float version;

  if ( !(ply = vtkPLY::ply_open_for_reading(this->FileName, &nelems, &elist,
                                            &fileType, &version)) )
  {
    vtkWarningMacro(<<"Could not open PLY file");
    return 0;
  }

  int numberOfComments = 0;
  char** comments = vtkPLY::ply_get_comments(ply, &numberOfComments);
  this->Comments->Reset();
  for (int i = 0; i < numberOfComments; i++)
  {
    this->Comments->InsertNextValue(comments[i]);
  }

  // Check to make sure that we can read geometry
  PlyElement *elem;
  int index;
  if ( (elem = vtkPLY::find_element (ply, "vertex")) == nullptr ||
       vtkPLY::find_property (elem, "x", &index) == nullptr ||
       vtkPLY::find_property (elem, "y", &index) == nullptr ||
       vtkPLY::find_property (elem, "z", &index) == nullptr)
  {
    vtkErrorMacro(<<"Cannot read geometry");
    vtkPLY::ply_close (ply);
  }

  // Check for optional attribute data. We can handle intensity; and the
  // triplet red, green, blue.
  bool intensityAvailable = false;
  vtkSmartPointer<vtkUnsignedCharArray> intensity = nullptr;
  if ( (elem = vtkPLY::find_element (ply, "face")) != nullptr &&
       vtkPLY::find_property (elem, "intensity", &index) != nullptr )
  {
    intensity = vtkSmartPointer<vtkUnsignedCharArray>::New();
    intensity->SetName("intensity");
    intensityAvailable = true;
    output->GetCellData()->AddArray(intensity);
    output->GetCellData()->SetActiveScalars("intensity");
  }

  bool RGBCellsAvailable = false;
  bool RGBCellsHaveAlpha = false;
  vtkSmartPointer<vtkUnsignedCharArray> RGBCells = nullptr;
  if ( (elem = vtkPLY::find_element (ply, "face")) != nullptr &&
       vtkPLY::find_property (elem, "red", &index) != nullptr &&
       vtkPLY::find_property (elem, "green", &index) != nullptr &&
       vtkPLY::find_property (elem, "blue", &index) != nullptr )
  {
    RGBCellsAvailable = true;
    RGBCells = vtkSmartPointer<vtkUnsignedCharArray>::New();
    if (vtkPLY::find_property(elem, "alpha", &index) != nullptr)
    {
      RGBCells->SetName("RGBA");
      RGBCells->SetNumberOfComponents(4);
      RGBCellsHaveAlpha = true;
    }
    else
    {
      RGBCells->SetName("RGB");
      RGBCells->SetNumberOfComponents(3);
    }
    output->GetCellData()->AddArray(RGBCells);
    output->GetCellData()->SetActiveScalars("RGB");
  }

  bool RGBPointsAvailable = false;
  bool RGBPointsHaveAlpha = false;
  vtkSmartPointer<vtkUnsignedCharArray> RGBPoints = nullptr;
  if ((elem = vtkPLY::find_element(ply, "vertex")) != nullptr)
  {
    if (vtkPLY::find_property(elem, "red", &index) != nullptr &&
        vtkPLY::find_property(elem, "green", &index) != nullptr &&
        vtkPLY::find_property(elem, "blue", &index) != nullptr)
    {
      RGBPointsAvailable = true;
    }
    else if (vtkPLY::find_property(elem, "diffuse_red", &index) != nullptr &&
             vtkPLY::find_property(elem, "diffuse_green", &index) != nullptr &&
             vtkPLY::find_property(elem, "diffuse_blue", &index) != nullptr)
    {
      RGBPointsAvailable = true;
      vertProps[8].name = "diffuse_red";
      vertProps[9].name = "diffuse_green";
      vertProps[10].name = "diffuse_blue";
    }
    if (RGBPointsAvailable)
    {
      RGBPoints = vtkSmartPointer<vtkUnsignedCharArray>::New();
      if (vtkPLY::find_property(elem, "alpha", &index) != nullptr)
      {
        RGBPoints->SetName("RGBA");
        RGBPoints->SetNumberOfComponents(4);
        RGBPointsHaveAlpha = true;
      }
      else
      {
        RGBPoints->SetName("RGB");
        RGBPoints->SetNumberOfComponents(3);
      }
      output->GetPointData()->SetScalars(RGBPoints);
    }
  }

  bool NormalPointsAvailable=false;
  vtkSmartPointer<vtkFloatArray> Normals = nullptr;
  if ( (elem = vtkPLY::find_element (ply, "vertex")) != nullptr &&
       vtkPLY::find_property (elem, "nx", &index) != nullptr &&
       vtkPLY::find_property (elem, "ny", &index) != nullptr &&
       vtkPLY::find_property (elem, "nz", &index) != nullptr )
  {
    Normals = vtkSmartPointer<vtkFloatArray>::New();
    NormalPointsAvailable = true;
    Normals->SetName("Normals");
    Normals->SetNumberOfComponents(3);
    output->GetPointData()->SetNormals(Normals);
  }

  bool TexCoordsPointsAvailable = false;
  vtkSmartPointer<vtkFloatArray> TexCoordsPoints = nullptr;
  if ( (elem = vtkPLY::find_element(ply, "vertex")) != nullptr )
  {
    if ( vtkPLY::find_property(elem, "u", &index) != nullptr &&
         vtkPLY::find_property(elem, "v", &index) != nullptr )
    {
      TexCoordsPointsAvailable = true;
    }
    else if ( vtkPLY::find_property(elem, "texture_u", &index) != nullptr &&
              vtkPLY::find_property(elem, "texture_v", &index) != nullptr )
    {
      TexCoordsPointsAvailable = true;
      vertProps[3].name = "texture_u";
      vertProps[4].name = "texture_v";
    }

    if ( TexCoordsPointsAvailable )
    {
      TexCoordsPoints = vtkSmartPointer<vtkFloatArray>::New();
      TexCoordsPoints->SetName("TCoords");
      TexCoordsPoints->SetNumberOfComponents(2);
      output->GetPointData()->SetTCoords(TexCoordsPoints);
    }
  }

  bool TexCoordsPointsAvailableFace = false;
  if ((elem = vtkPLY::find_element(ply, "face")) != nullptr && !TexCoordsPointsAvailable)
  {
    if (vtkPLY::find_property(elem, "texcoord", &index) != nullptr)
    {
      TexCoordsPointsAvailableFace = true;
      TexCoordsPoints = vtkSmartPointer<vtkFloatArray>::New();
      TexCoordsPoints->SetName("TCoords");
      TexCoordsPoints->SetNumberOfComponents(2);
      output->GetPointData()->SetTCoords(TexCoordsPoints);
    }
  }
  // Okay, now we can grab the data
  int numPts = 0, numPolys = 0;
  for (int i = 0; i < nelems; i++)
  {
    //get the description of the first element */
    elemName = elist[i];
    vtkPLY::ply_get_element_description (ply, elemName, &numElems, &nprops);

    // if we're on vertex elements, read them in
    if ( elemName && !strcmp ("vertex", elemName) )
    {
      // Create a list of points
      numPts = numElems;
      vtkPoints *pts = vtkPoints::New();
      pts->SetDataTypeToFloat();
      pts->SetNumberOfPoints(numPts);

      // Setup to read the PLY elements
      vtkPLY::ply_get_property (ply, elemName, &vertProps[0]);
      vtkPLY::ply_get_property (ply, elemName, &vertProps[1]);
      vtkPLY::ply_get_property (ply, elemName, &vertProps[2]);

      if ( TexCoordsPointsAvailable )
      {
        vtkPLY::ply_get_property (ply, elemName, &vertProps[3]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[4]);
        TexCoordsPoints->SetNumberOfTuples(numPts);
      }

      if ( NormalPointsAvailable )
      {
        vtkPLY::ply_get_property (ply, elemName, &vertProps[5]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[6]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[7]);
        Normals->SetNumberOfTuples(numPts);
      }

      if ( RGBPointsAvailable )
      {
        vtkPLY::ply_get_property (ply, elemName, &vertProps[8]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[9]);
        vtkPLY::ply_get_property (ply, elemName, &vertProps[10]);
        if (RGBPointsHaveAlpha)
        {
          vtkPLY::ply_get_property(ply, elemName, &vertProps[11]);
        }
        RGBPoints->SetNumberOfTuples(numPts);
      }

      plyVertex vertex;
      for (int j=0; j < numPts; j++)
      {
        vtkPLY::ply_get_element (ply, (void *) &vertex);
        pts->SetPoint (j, vertex.x);
        if ( TexCoordsPointsAvailable )
        {
          TexCoordsPoints->SetTuple2(j, vertex.tex[0], vertex.tex[1]);
        }
        if ( NormalPointsAvailable )
        {
          Normals->SetTuple3(j, vertex.normal[0], vertex.normal[1], vertex.normal[2]);
        }
        if ( RGBPointsAvailable )
        {
          if (RGBPointsHaveAlpha)
          {
            RGBPoints->SetTuple4(j, vertex.red, vertex.green, vertex.blue, vertex.alpha);
          }
          else
          {
            RGBPoints->SetTuple3(j, vertex.red, vertex.green, vertex.blue);
          }
        }
      }
      output->SetPoints(pts);
      pts->Delete();
    }//if vertex

    else if ( elemName && !strcmp ("face", elemName) )
    {
      // Create a polygonal array
      numPolys = numElems;
      vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
      polys->Allocate(polys->EstimateSize(numPolys,3),numPolys/2);
      plyFace face;
      vtkIdType vtkVerts[256];

      // Get the face properties
      vtkPLY::ply_get_property (ply, elemName, &faceProps[0]);
      if ( intensityAvailable )
      {
        vtkPLY::ply_get_property (ply, elemName, &faceProps[1]);
        intensity->SetNumberOfComponents(1);
        intensity->SetNumberOfTuples(numPolys);
      }
      if ( RGBCellsAvailable )
      {
        vtkPLY::ply_get_property (ply, elemName, &faceProps[2]);
        vtkPLY::ply_get_property (ply, elemName, &faceProps[3]);
        vtkPLY::ply_get_property (ply, elemName, &faceProps[4]);

        if (RGBCellsHaveAlpha)
        {
          vtkPLY::ply_get_property(ply, elemName, &faceProps[5]);
        }
        RGBCells->SetNumberOfTuples(numPolys);
      }
      if (TexCoordsPointsAvailableFace)
      {
        vtkPLY::ply_get_property(ply, elemName, &faceProps[6]);
        TexCoordsPoints->SetNumberOfTuples(numPts);
      }

      // grab all the face elements
      for (int j=0; j < numPolys; j++)
      {
        //grab and element from the file
        vtkPLY::ply_get_element (ply, (void *) &face);
        for (int k=0; k < face.nverts; k++)
        {
          vtkVerts[k] = face.verts[k];
        }
        free(face.verts); // allocated in vtkPLY::ascii/binary_get_element

        polys->InsertNextCell(face.nverts,vtkVerts);
        if ( intensityAvailable )
        {
          intensity->SetValue(j,face.intensity);
        }
        if ( RGBCellsAvailable )
        {
          if (RGBCellsHaveAlpha)
          {
            RGBCells->SetValue(4 * j, face.red);
            RGBCells->SetValue(4 * j + 1, face.green);
            RGBCells->SetValue(4 * j + 2, face.blue);
            RGBCells->SetValue(4 * j + 3, face.alpha);
          }
          else
          {
            RGBCells->SetValue(3 * j, face.red);
            RGBCells->SetValue(3 * j + 1, face.green);
            RGBCells->SetValue(3 * j + 2, face.blue);
          }
        }
        if (TexCoordsPointsAvailableFace)
        {
          //Test to know if there is a texcoord for every vertex
          if (face.nverts == (face.ntexcoord / 2))
          {
            for (int k = 0; k < face.nverts; k++)
            {
              TexCoordsPoints->SetTuple2(vtkVerts[k], face.texcoord[k * 2], face.texcoord[(k * 2) + 1]);
            }
            free(face.texcoord);
          }
        }
      }
      output->SetPolys(polys);
    }//if face

    free(elist[i]); //allocated by ply_open_for_reading
    elist[i] = nullptr;

  }//for all elements of the PLY file
  free(elist); //allocated by ply_open_for_reading

  vtkDebugMacro( <<"Read: " << numPts << " points, "
                 << numPolys << " polygons");

  // close the PLY file
  vtkPLY::ply_close (ply);

  return 1;
}

int vtkPLYReader::CanReadFile(const char *filename)
{
  FILE *fd = vtksys::SystemTools::Fopen(filename, "rb");
  if (!fd) return 0;

  char line[4] = {};
  const char *result = fgets(line, sizeof(line), fd);
  fclose(fd);
  return (result && strncmp(result, "ply", 3) == 0);
}

void vtkPLYReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);

  os << indent << "Comments:\n";
  indent = indent.GetNextIndent();
  for(int i = 0; i < this->Comments->GetNumberOfValues(); ++i)
  {
    os << indent << this->Comments->GetValue(i) << "\n";
  }
}
