/*==================================================================

  Program:   Visualization Toolkit
  Module:    TestHyperTreeGridTernary3DAxisReflectionYZCenterMaterial.cxx

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

#include "vtkHyperTreeGridGeometry.h"
#include "vtkHyperTreeGridAxisReflection.h"
#include "vtkHyperTreeGridSource.h"

#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkNew.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

int TestHyperTreeGridTernary3DAxisReflectionYZCenterMaterial( int argc, char* argv[] )
{
  // Hyper tree grid
  vtkNew<vtkHyperTreeGridSource> htGrid;
  htGrid->SetMaximumLevel( 6 );
  htGrid->SetGridSize( 3, 3, 2 );
  htGrid->SetGridScale( 1.5, 1., .7 );
  htGrid->SetDimension( 3 );
  htGrid->SetBranchFactor( 3 );
  htGrid->UseMaterialMaskOn();
  htGrid->SetDescriptor( "RRR .R. .RR ..R ..R .R.|R.......................... ........................... ........................... .............R............. ....RR.RR........R......... .....RRRR.....R.RR......... ........................... ........................... ...........................|........................... ........................... ........................... ...RR.RR.......RR.......... ........................... RR......................... ........................... ........................... ........................... ........................... ........................... ........................... ........................... ............RRR............|........................... ........................... .......RR.................. ........................... ........................... ........................... ........................... ........................... ........................... ........................... ...........................|........................... ..........................." );
  htGrid->SetMaterialMask( "111 011 011 111 011 110|111111111111111111111111111 111111111111111111111111111 000000000100110111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 000110011100000100100010100|000001011011111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111001111111101111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111|000000000111100100111100100 000000000111001001111001001 000000111100100111111111111 000000111001001111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 111111111111111111111111111 110110110100111110111000000|111111111111111111111111111 111111111111111111111111111" );

  // Axis reflections
  vtkNew<vtkHyperTreeGridAxisReflection> reflection1;
  reflection1->SetInputConnection( htGrid->GetOutputPort() );
  reflection1->SetPlaneToY();
  reflection1->SetCenter( 1.5 );
  vtkNew<vtkHyperTreeGridAxisReflection> reflection2;
  reflection2->SetInputConnection( reflection1->GetOutputPort() );
  reflection2->SetPlaneToZ();
  reflection2->SetCenter( .7 );

  // Geometries
  vtkNew<vtkHyperTreeGridGeometry> geometry1;
  geometry1->SetInputConnection( reflection2->GetOutputPort() );
  geometry1->Update();
  vtkPolyData* pd = geometry1->GetPolyDataOutput();
  vtkNew<vtkHyperTreeGridGeometry> geometry2;
  geometry2->SetInputConnection( reflection2->GetOutputPort() );

  // Mappers
  vtkMapper::SetResolveCoincidentTopologyToPolygonOffset();
  vtkNew<vtkPolyDataMapper> mapper1;
  mapper1->SetInputConnection( geometry1->GetOutputPort() );
  mapper1->ScalarVisibilityOff();
  vtkNew<vtkPolyDataMapper> mapper2;
  mapper2->SetInputConnection( geometry2->GetOutputPort() );
  mapper2->SetScalarRange( pd->GetCellData()->GetScalars()->GetRange() );

  // Actors
  vtkNew<vtkActor> actor1;
  actor1->SetMapper( mapper1 );
  actor1->GetProperty()->SetRepresentationToWireframe();
  actor1->GetProperty()->SetColor( .7, .7, .7 );
  vtkNew<vtkActor> actor2;
  actor2->SetMapper( mapper2 );

  // Camera
  double bd[6];
  pd->GetBounds( bd );
  vtkNew<vtkCamera> camera;
  camera->SetClippingRange( 1., 100. );
  camera->SetFocalPoint( pd->GetCenter() );
  camera->SetPosition( -.8 * bd[1], 2.1 * bd[3], -4.8 * bd[5] );

  // Renderer
  vtkNew<vtkRenderer> renderer;
  renderer->SetActiveCamera( camera );
  renderer->SetBackground( 1., 1., 1. );
  renderer->AddActor( actor1 );
  renderer->AddActor( actor2 );

  // Render window
  vtkNew<vtkRenderWindow> renWin;
  renWin->AddRenderer( renderer );
  renWin->SetSize( 400, 400 );
  renWin->SetMultiSamples( 0 );

  // Interactor
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow( renWin );

  // Render and test
  renWin->Render();

  int retVal = vtkRegressionTestImageThreshold( renWin, 55 );
  if ( retVal == vtkRegressionTester::DO_INTERACTOR )
  {
    iren->Start();
  }

  return !retVal;
}
