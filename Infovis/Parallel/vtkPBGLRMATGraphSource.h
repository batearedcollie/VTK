/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPBGLRMATGraphSource.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/
/*
 * Copyright (C) 2008 The Trustees of Indiana University.
 * Use, modification and distribution is subject to the Boost Software
 * License, Version 1.0. (See http://www.boost.org/LICENSE_1_0.txt)
 */
// .NAME vtkPBGLRMATGraphSource - a distributed graph with random edges built
// accorting to the recursive matrix (R-MAT) model.
//
// .SECTION Description
// Generates a directed distributed graph with a specified number of
// vertices (N=2^X) and a set of probabilities A, B, C, and D, using
// the recursive matrix (R-MAT) method of Chakrabarti, Zhan, and
// Faloutsos. Edges are generated by randomly selecting an element
// within the adjacency matrix, then adding the corresponding
// edge. The element in the adjacency matrix is selected by placing
// set of grids over the adjacency list. The topmost grid has four
// quadrants, and the probability of creating an edge within that
// quadrant is specified by A, B, C, or D, corresponding to the
// top-left, top-right, bottom-left, and bottom-right quadrants in the
// grid, respectively:
//
//   +-+-+
//   |A|B|
//   +-+-+
//   |C|D|
//   +-+-+
//
// Naturally, A+B+C+D must equal 1. Once a quadrant has been selected,
// the quadrant itself is subdivided into another A-B-C-D grid and the
// same process is applied repeatedly until the grid itself matches
// the adjacency matrix and a specific edge is selected.
//
// Typically, A >= B, A >= C, and A >= D. A and D are viewed as two
// separate communities, while B and C provide interconnections
// between those two communities. The more skewed the probabilities
// between the communities (A >= D), the more unbalanced the resulting
// degree distributions will be. With no skew (A=B=C=D=0.25), this
// generator produces graphs similar to the random graphs provided by
// vtkPRandomGraphSource. Greater skew values tend to produce graphs
// with a power-law degree distribution, which mimics the behavior of
// many real-world graphs based on social networks.
#ifndef __vtkPBGLRMATGraphSource_h
#define __vtkPBGLRMATGraphSource_h

#include "vtkInfovisParallelModule.h" // For export macro
#include "vtkGraphAlgorithm.h"

class vtkGraph;
class vtkPVXMLElement;

class VTKINFOVISPARALLEL_EXPORT vtkPBGLRMATGraphSource : public vtkGraphAlgorithm
{
public:
  static vtkPBGLRMATGraphSource* New();
  vtkTypeMacro(vtkPBGLRMATGraphSource,vtkGraphAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The number of vertices in the graph. This value will always be a
  // power of two.
  vtkGetMacro(NumberOfVertices, vtkIdType);

  // Description:
  // Sets the number of vertices in the graph, which will be rounded
  // to the nearest power of two.
  virtual void SetNumberOfVertices(vtkIdType value);

  // Description:
  // Creates a graph with the specified number of edges. Duplicate
  // (parallel) edges are allowed.
  vtkGetMacro(NumberOfEdges, vtkIdType);
  vtkSetClampMacro(NumberOfEdges, vtkIdType, 0, VTK_LARGE_ID);

  // Description:
  // Set the quadrant probabilities A, B, C, D. Requires that
  // A+B+C+D=1.
  void SetProbabilities(double A, double B, double C, double D);

  // Description:
  // Retrieves the quadrant probabilities.
  void GetProbabilities(double *A, double *B, double *C, double *D);

  // Description:
  // When set, includes edge weights in an array named "edge_weights".
  // Defaults to off.  Weights are random between 0 and 1.
  vtkSetMacro(IncludeEdgeWeights, bool);
  vtkGetMacro(IncludeEdgeWeights, bool);
  vtkBooleanMacro(IncludeEdgeWeights, bool);

  // Description:
  // The name of the edge weight array. Default "edge weight".
  vtkSetStringMacro(EdgeWeightArrayName);
  vtkGetStringMacro(EdgeWeightArrayName);

  // Description:
  // If this flag is set to true, edges where the source and target
  // vertex are the same can be generated.  The default is to forbid
  // such loops.
  vtkSetMacro(AllowSelfLoops, bool);
  vtkGetMacro(AllowSelfLoops, bool);
  vtkBooleanMacro(AllowSelfLoops, bool);

  // Description:
  // Add pedigree ids to vertex and edge data.
  vtkSetMacro(GeneratePedigreeIds, bool);
  vtkGetMacro(GeneratePedigreeIds, bool);
  vtkBooleanMacro(GeneratePedigreeIds, bool);

  // Description:
  // The name of the vertex pedigree id array. Default "vertex id".
  vtkSetStringMacro(VertexPedigreeIdArrayName);
  vtkGetStringMacro(VertexPedigreeIdArrayName);

  // Description:
  // The name of the edge pedigree id array. Default "edge id".
  vtkSetStringMacro(EdgePedigreeIdArrayName);
  vtkGetStringMacro(EdgePedigreeIdArrayName);

  // Description:
  // Control the seed used for pseudo-random-number generation.
  // This ensures that vtkPBGLRMATGraphSource can produce repeatable
  // results. The seed values provided for each process should be different,
  vtkSetMacro(Seed, int);
  vtkGetMacro(Seed, int);

protected:
  vtkPBGLRMATGraphSource();
  ~vtkPBGLRMATGraphSource();
  vtkIdType NumberOfVertices;
  vtkIdType NumberOfEdges;
  double A;
  double B;
  double C;
  double D;
  bool IncludeEdgeWeights;
  bool AllowSelfLoops;
  bool GeneratePedigreeIds;
  int Seed;
  char* EdgeWeightArrayName;
  char* VertexPedigreeIdArrayName;
  char* EdgePedigreeIdArrayName;

  virtual int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*);

  // Description:
  // Creates directed or undirected output based on Directed flag.
  virtual int RequestDataObject(vtkInformation*,
                                vtkInformationVector** inputVector,
                                vtkInformationVector* outputVector);

private:
  vtkPBGLRMATGraphSource(const vtkPBGLRMATGraphSource&); // Not implemented
  void operator=(const vtkPBGLRMATGraphSource&);   // Not implemented
};

#endif

