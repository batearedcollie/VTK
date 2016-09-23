/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTreeMapLayoutStrategy.cxx

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

#include "vtkActor.h"
#include "vtkCirclePackFrontChainLayoutStrategy.h"
#include "vtkIntArray.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTestUtilities.h"
#include "vtkTree.h"
#include "vtkTreeFieldAggregator.h"
#include "vtkCirclePackLayout.h"
#include "vtkCirclePackToPolyData.h"

#define VTK_CREATE(type, name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

void TestStrategy(vtkCirclePackLayoutStrategy* strategy, vtkTreeAlgorithm* input, double posX, double posY, vtkRenderer* ren)
{
  VTK_CREATE(vtkCirclePackLayout, layout);
  layout->SetLayoutStrategy(strategy);
  layout->SetInputConnection(input->GetOutputPort());
  layout->Update();
  vtkDataArray* vda = layout->GetOutput()->GetVertexData()->GetArray("circles");
  // Test GetBoundingCircle() and FindVertex()
  double cinfo[3];
  layout->GetBoundingCircle(vda->GetNumberOfTuples() - 1, cinfo);
  double pnt[2];
  pnt[0] = cinfo[0];
  pnt[1] = cinfo[1];
  if(((int) layout->FindVertex(pnt)) != (vda->GetNumberOfTuples() - 1))
  {
    cout << "GetBoundingCircle() and FindVertex() returned incorrect id" << endl;
    exit(1);
  }

  VTK_CREATE(vtkCirclePackToPolyData, poly);
  poly->SetInputConnection(layout->GetOutputPort());
  VTK_CREATE(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(poly->GetOutputPort());
  mapper->SetScalarRange(0,600);
  mapper->SetScalarModeToUseCellFieldData();
  mapper->SelectColorArray("size");
  VTK_CREATE(vtkActor, actor);
  actor->SetMapper(mapper);
  actor->SetPosition(posX, posY, 0);
  ren->AddActor(actor);
}

int TestCirclePackLayoutStrategy(int argc, char* argv[])
{
  VTK_CREATE(vtkRenderer, ren);
  // Create input
  VTK_CREATE(vtkMutableDirectedGraph, builder);
  VTK_CREATE(vtkIntArray, sizeArr);
  sizeArr->SetName("size");
  builder->GetVertexData()->AddArray(sizeArr);
  builder->AddVertex();
  sizeArr->InsertNextValue(0);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(77);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(15);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(99);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(107);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(432);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(259);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(242);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(306);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(91);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(46);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(47);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(150);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(90);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(10);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(456);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(40);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(98);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(100);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(400);
  builder->AddChild(0);
  sizeArr->InsertNextValue(500);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(105);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(15);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(410);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(320);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(410);
  builder->AddChild(0);
  sizeArr->InsertNextValue(450);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(136);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(458);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);
  builder->AddChild(0);
  sizeArr->InsertNextValue(1);

  VTK_CREATE(vtkTree, tree);
  if (!tree->CheckedShallowCopy(builder))
  {
    cerr << "Invalid tree structure." << endl;
  }

  VTK_CREATE(vtkTreeFieldAggregator, agg);
  agg->SetInputData(tree);
  agg->SetField("size");
  agg->SetLeafVertexUnitSize(false);

  // Test Front Chain layout
  VTK_CREATE(vtkCirclePackFrontChainLayoutStrategy, fc);
  TestStrategy(fc, agg, 0, 0, ren);

  VTK_CREATE(vtkRenderWindowInteractor, iren);
  VTK_CREATE(vtkRenderWindow, win);
  win->AddRenderer(ren);
  win->SetInteractor(iren);

  int retVal = vtkRegressionTestImage(win);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    win->Render();
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
  }
  return !retVal;
}
