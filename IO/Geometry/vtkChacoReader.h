/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkChacoReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*----------------------------------------------------------------------------
 Copyright (c) Sandia Corporation
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.
----------------------------------------------------------------------------*/

// .NAME vtkChacoReader - Read a Chaco file and create a vtkUnstructuredGrid.
// .SECTION Description
// vtkChacoReader is an unstructured grid source object that reads Chaco
// files.  The reader DOES NOT respond to piece requests. Chaco
// is a graph partitioning package developed at Sandia National Laboratories
// in the early 1990s.  (http://www.cs.sandia.gov/~bahendr/chaco.html)
//
// Note that the Chaco "edges" become VTK "cells", and the Chaco
// "vertices" become VTK "points".

#ifndef __vtkChacoReader_h
#define __vtkChacoReader_h

#include "vtkIOGeometryModule.h" // For export macro
#include "vtkUnstructuredGridAlgorithm.h"

class VTKIOGEOMETRY_EXPORT vtkChacoReader : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkChacoReader *New();
  vtkTypeMacro(vtkChacoReader,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Specify the base name of the Chaco files.  The reader will try to
  // open BaseName.coords and BaseName.graph.

  vtkSetStringMacro(BaseName);
  vtkGetStringMacro(BaseName);

  // Description:
  // Indicate whether this reader should create a cell array containing
  // global IDs for the cells in the output vtkUnstructuredGrid.  These cells
  // represent the edges that were in the Chaco file.  Each edge is a vtkLine.
  // Default is ON.

  vtkSetMacro(GenerateGlobalElementIdArray, int);
  vtkGetMacro(GenerateGlobalElementIdArray, int);
  vtkBooleanMacro(GenerateGlobalElementIdArray, int);

  static const char *GetGlobalElementIdArrayName() { return "GlobalElementId"; }

  // Description:
  // Indicate whether this reader should create a point array of global IDs for
  // the points in the output vtkUnstructuredGrid.  These points are the vertices
  // that were in the Chaco file.  Global point IDs start at "1" for the first
  // vertex in BaseName.coords and go up from there.
  // Default is ON.

  vtkSetMacro(GenerateGlobalNodeIdArray, int);
  vtkGetMacro(GenerateGlobalNodeIdArray, int);
  vtkBooleanMacro(GenerateGlobalNodeIdArray, int);

  static const char *GetGlobalNodeIdArrayName() { return "GlobalNodeId"; }

  // Description:
  // Indicate whether this reader should create a point array for each
  // vertex weight in the Chaco file.
  // Default is OFF.

  vtkSetMacro(GenerateVertexWeightArrays, int);
  vtkGetMacro(GenerateVertexWeightArrays, int);
  vtkBooleanMacro(GenerateVertexWeightArrays, int);

  // Description:
  // Returns the number of weights per vertex in the Chaco file, whether or
  // not GenerateVertexWeightArrays is ON.

  vtkGetMacro(NumberOfVertexWeights, int);

  // Description:
  // This method returns the name of the selected Vertex weight point
  // array.  If you did not turn on GenerateVertexWeightArrays, or
  // if the weight you requested is invalid, it returns NULL.
  // Weights begin at one and go up to NumberOfVertexWeights.
  // This is a pointer to our copy of the name, so don't "delete" it.

  const char *GetVertexWeightArrayName(int weight);

  // Description:
  // Each edge in the Chaco file connects two vertices.  The file may
  // specify one or more weights for each edge.  (The weight for an
  // edge from vertex A to vertex B equals the weight from B to A.)
  // Indicate with the following parameter whether this reader should
  // create a cell array for each weight for every edge.
  // Default is OFF.

  vtkSetMacro(GenerateEdgeWeightArrays, int);
  vtkGetMacro(GenerateEdgeWeightArrays, int);
  vtkBooleanMacro(GenerateEdgeWeightArrays, int);

  // Description:
  // Returns the number of weights per edge in the Chaco file, whether or
  // not GenerateEdgeWeightArrays is ON.

  vtkGetMacro(NumberOfEdgeWeights, int);

  // Description:
  // This method returns the name of the selected Edge weight cell
  // array.  If you did not turn on GenerateEdgeWeightArrays, or
  // if the weight you requested is invalid, it returns NULL.
  // Weights begin at one and go up to NumberOfEdgeWeights.
  // This is a pointer to our copy of the name, so don't "delete" it.

  const char *GetEdgeWeightArrayName(int weight);

  // Description:
  // Access to meta data generated by RequestInformation.
  vtkGetMacro(Dimensionality, int);
  vtkGetMacro(NumberOfEdges, vtkIdType);
  vtkGetMacro(NumberOfVertices, vtkIdType);

  // Description:
  // After this filter executes, this method returns the number of
  // cell arrays that were created to hold the edge weights.  It
  // is equal to NumberOfEdgeWeights if GenerateEdgeWeightArrays was ON.

  vtkGetMacro(NumberOfCellWeightArrays, int);

  // Description:
  // After this filter executes, this method returns the number of
  // point arrays that were created to hold the vertex weights.  It
  // is equal to NumberOfVertexWeights if GenerateVertexWeightArrays was ON.

  vtkGetMacro(NumberOfPointWeightArrays, int);

protected:
  vtkChacoReader();
  ~vtkChacoReader();

  int BuildOutputGrid(vtkUnstructuredGrid *gr);

  // methods for parsing Chaco files

  void CloseCurrentFile();
  int OpenCurrentFile();

  int InputGeom(vtkIdType nvtxs, int igeom, double *x, double *y, double *z);
  int InputGraph1();
  int InputGraph2( vtkIdType **start, vtkIdType **adjacency, double **vweights,
      double **eweights);
  int GetCoordsMetadata();
  void GetGraphMetadata();

  // methods for creating vtkUnstructuredGrid from Chaco file data

  int ReadFile(vtkUnstructuredGrid* output);
  void AddElementIds(vtkUnstructuredGrid* output);
  void AddNodeIds(vtkUnstructuredGrid* output);

  void MakeWeightArrayNames(int nv, int ne);

  // Parameters for controlling what is read in.
  char *BaseName;
  int GenerateGlobalElementIdArray;
  int GenerateGlobalNodeIdArray;

  int GenerateVertexWeightArrays;
  int GenerateEdgeWeightArrays;

  FILE *CurrentGeometryFP;
  FILE *CurrentGraphFP;
  char *CurrentBaseName;
  vtkSetStringMacro(CurrentBaseName);

  char **VarrayName;
  char **EarrayName;

  //----------------------------------------------------------------------
  // The following metadata is read during RequestInformation.  If you
  // add new metadata, you must modify vtkPChacoReader::RequestInformation
  // to include it when process 0 broadcasts the metadata.

  int Dimensionality;
  vtkIdType NumberOfVertices;
  vtkIdType NumberOfEdges;
  int NumberOfVertexWeights;   // in file
  int NumberOfEdgeWeights;     // in file
  int GraphFileHasVertexNumbers;

  //----------------------------------------------------------------------

  int NumberOfPointWeightArrays;   // in output unstuctured grid
  int NumberOfCellWeightArrays;    // in output unstuctured grid

  // Keep the points and cells
  // around so they don't need to be re-read when the
  // options change.
  vtkUnstructuredGrid *DataCache;

  // Should I re-read in the geometry and topology of the dataset
  int RemakeDataCacheFlag;

  int RequestInformation(
    vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int RequestData(
    vtkInformation *, vtkInformationVector **, vtkInformationVector *);

private:
  vtkChacoReader(const vtkChacoReader&); // Not implemented
  void operator=(const vtkChacoReader&); // Not implemented

  double ReadVal(FILE *infile, int *end_flag);
  vtkIdType ReadInt(FILE *infile, int *end_flag);
  void FlushLine( FILE *infile);
  void ResetInputBuffers();

  char *Line;
  int Line_length;
  int Offset;
  int Break_pnt;
  int Save_pnt;

  void ClearWeightArrayNames();
};

#endif
