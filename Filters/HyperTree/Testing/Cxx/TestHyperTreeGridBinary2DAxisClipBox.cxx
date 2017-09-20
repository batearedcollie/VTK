/*==================================================================

  Program:   Visualization Toolkit
  Module:    TestHyperTreeGridBinary2DAxisClipBox.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

===================================================================*/
// .SECTION Thanks
// This test was written by Philippe Pebay, 2016
// This work was supported by Commissariat a l'Energie Atomique (CEA/DIF)

#include "vtkHyperTreeGrid.h"
#include "vtkHyperTreeGridAxisClip.h"
#include "vtkHyperTreeGridGeometry.h"
#include "vtkHyperTreeGridSource.h"

#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkDataSetMapper.h"
#include "vtkLineSource.h"
#include "vtkNew.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyLine.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

int TestHyperTreeGridBinary2DAxisClipBox( int argc, char* argv[] )
{
  // Hyper tree grid
  vtkNew<vtkHyperTreeGridSource> htGrid;
  int maxLevel = 6;
  htGrid->SetMaximumLevel( maxLevel );
  htGrid->SetGridSize( 2, 3, 1 );
  htGrid->SetGridScale( 1.5, 1., 10. );  // this is to test that orientation fixes scale
  htGrid->SetDimension( 2 );
  htGrid->SetOrientation( 2 ); // in xy plane
  htGrid->SetBranchFactor( 2 );
  htGrid->SetDescriptor( "RRRRR.|.... .R.. RRRR R... R...|.R.. ...R ..RR .R.. R... .... ....|.... ...R ..R. .... .R.. R...|.... .... .R.. ....|...." );

  // Axis clip
  vtkNew<vtkHyperTreeGridAxisClip> clip;
  clip->SetInputConnection( htGrid->GetOutputPort() );
  clip->SetClipTypeToBox();
  double x0 = 0.725;
  double x1 = 1.6;
  double y0 = 1.46;
  double y1 = 2.3;
  double z0 = -.5;
  double z1 = 1.9;
  clip->SetBounds( x0, x1, y0, y1, z0, z1 );

  // Geometries
  vtkNew<vtkHyperTreeGridGeometry> geometry1;
  geometry1->SetInputConnection( htGrid->GetOutputPort() );
  geometry1->Update();
  vtkPolyData* pd = geometry1->GetPolyDataOutput();
  vtkNew<vtkHyperTreeGridGeometry> geometry2;
  geometry2->SetInputConnection( clip->GetOutputPort() );

  // Rectangle
  double p0[3] = { x0, y0, 0. };
  double p1[3] = { x1, y0, 0. };
  double p2[3] = { x1, y1, 0. };
  double p3[3] = { x0, y1, 0. };
  vtkNew<vtkPoints> points;
  points->InsertNextPoint( p0 );
  points->InsertNextPoint( p1 );
  points->InsertNextPoint( p2 );
  points->InsertNextPoint( p3 );
  points->InsertNextPoint( p0 );
  vtkNew<vtkPolyLine> polyLine;
  polyLine->GetPointIds()->SetNumberOfIds( 5 );
  polyLine->GetPointIds()->SetId( 0, 0 );
  polyLine->GetPointIds()->SetId( 1, 1 );
  polyLine->GetPointIds()->SetId( 2, 2 );
  polyLine->GetPointIds()->SetId( 3, 3 );
  polyLine->GetPointIds()->SetId( 4, 0 );
  vtkNew<vtkCellArray> edges;
  edges->InsertNextCell( polyLine.GetPointer() );
  vtkNew<vtkPolyData> rectangle;
  rectangle->SetPoints( points.GetPointer() );
  rectangle->SetLines( edges.GetPointer() );

  // Mappers
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
  vtkNew<vtkDataSetMapper> mapper1;
  mapper1->SetInputConnection( geometry2->GetOutputPort() );
  mapper1->SetScalarRange( pd->GetCellData()->GetScalars()->GetRange() );
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection( geometry1->GetOutputPort() );
  mapper2->ScalarVisibilityOff();
  vtkNew<vtkPolyDataMapper> mapper3;
  mapper3->SetInputData( rectangle.GetPointer() );
  mapper3->ScalarVisibilityOff();

  // Actors
  vtkNew<vtkActor> actor1;
  actor1->SetMapper( mapper1.GetPointer() );
  vtkNew<vtkActor> actor2;
  actor2->SetMapper( mapper2.GetPointer() );
  actor2->GetProperty()->SetRepresentationToWireframe();
  actor2->GetProperty()->SetColor( .7, .7, .7 );
  vtkNew<vtkActor> actor3;
  actor3->SetMapper( mapper3.GetPointer() );
  actor3->GetProperty()->SetColor( .3, .3, .3 );
  actor3->GetProperty()->SetLineWidth( 3 );

  // Camera
  vtkHyperTreeGrid* ht = htGrid->GetHyperTreeGridOutput();
  double bd[6];
  ht->GetBounds( bd );
  vtkNew<vtkCamera> camera;
  camera->SetClippingRange( 1., 100. );
  camera->SetFocalPoint( pd->GetCenter() );
  camera->SetPosition( .5 * bd[1], .5 * bd[3], 6. );

  // Renderer
  vtkNew<vtkRenderer> renderer;
  renderer->SetActiveCamera( camera.GetPointer() );
  renderer->SetBackground( 1., 1., 1. );
  renderer->AddActor( actor1.GetPointer() );
  renderer->AddActor( actor2.GetPointer() );
  renderer->AddActor( actor3.GetPointer() );

  // Render window
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer( renderer.GetPointer() );
  renWin->SetSize( 400, 400 );
  renWin->SetMultiSamples( 0 );

  // Interactor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow( renWin.GetPointer() );

  // Render and test
  renWin->Render();

  int retVal = vtkRegressionTestImageThreshold( renWin.GetPointer(), 70 );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR )
  {
    iren->Start();
  }

  return !retVal;
}
