/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkXdmf3SILBuilder.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 1993-2002 Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkXdmf3SILBuilder -
// .SECTION Description
//
//
// This file is a helper for the vtkXdmf3Reader and vtkXdmf3Writer and
// not intended to be part of VTK public API
// VTK-HeaderTest-Exclude: vtkXdmf3ArrayKeeper.h

#ifndef __vtkXdmf3SILBuilder_h
#define __vtkXdmf3SILBuilder_h

#include "vtkIOXdmf3Module.h" // For export macro
#include "vtkType.h"

class vtkMutableDirectedGraph;
class vtkStringArray;
class vtkUnsignedCharArray;

class VTKIOXDMF3_EXPORT vtkXdmf3SILBuilder
{
public:
  vtkStringArray* NamesArray;
  vtkUnsignedCharArray* CrossEdgesArray;
  vtkMutableDirectedGraph* SIL;
  vtkIdType RootVertex;
  vtkIdType BlocksRoot;
  vtkIdType HierarchyRoot;
  vtkIdType VertexCount;

  // Description:
  // Initializes the data-structures.
  void Initialize();

  // Description:
  // Add vertex, child-edge or cross-edge to the graph.
  vtkIdType AddVertex(const char* name);
  vtkIdType AddChildEdge(vtkIdType parent, vtkIdType child);
  vtkIdType AddCrossEdge(vtkIdType src, vtkIdType dst);

  // Description:
  // Returns the vertex id for the root vertex.
  vtkIdType GetRootVertex();
  vtkIdType GetBlocksRoot();
  vtkIdType GetHierarchyRoot();

  bool IsMaxedOut();

  vtkXdmf3SILBuilder();
  ~vtkXdmf3SILBuilder();
};

#endif //__vtkXdmf3SILBuilder_h
