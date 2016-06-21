/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkActorNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkActorNode - vtkViewNode specialized for vtkActors
// .SECTION Description
// State storage and graph traversal for vtkActor/Mapper and Property
// Made a choice to merge actor, mapper and property together. If there
// is a compelling reason to separate them we can.

#ifndef vtkActorNode_h
#define vtkActorNode_h

#include "vtkRenderingSceneGraphModule.h" // For export macro
#include "vtkViewNode.h"

class VTKRENDERINGSCENEGRAPH_EXPORT vtkActorNode :
  public vtkViewNode
{
public:
  static vtkActorNode* New();
  vtkTypeMacro(vtkActorNode, vtkViewNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  //Description:
  //Build containers for our child nodes.
  virtual void Build(bool prepass);

protected:
  vtkActorNode();
  ~vtkActorNode();

 private:
  vtkActorNode(const vtkActorNode&); // Not implemented.
  void operator=(const vtkActorNode&) VTK_DELETE_FUNCTION;
};

#endif
