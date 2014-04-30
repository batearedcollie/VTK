/*=========================================================================

  Program:   Visualization Toolkit

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkPolyDataNormals.h"
#include "vtkVBOPolyDataMapper.h"
#include "vtkPLYReader.h"
#include "vtkNew.h"
#include "vtkProperty.h"
#include "vtkLightKit.h"

#include "vtkTimerLog.h"

#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

//----------------------------------------------------------------------------
int TestVBOPLYMapper(int argc, char *argv[])
{
  vtkNew<vtkActor> actor;
  vtkNew<vtkVBOPolyDataMapper> mapper;
  vtkNew<vtkRenderer> renderer;
  renderer->SetBackground(0.0, 0.0, 0.0);
  vtkNew<vtkRenderWindow> renderWindow;
  renderWindow->SetSize(300, 300);
  renderWindow->AddRenderer(renderer.Get());
  renderer->AddActor(actor.Get());

  vtkNew<vtkLightKit> lightKit;
  lightKit->AddLightsToRenderer(renderer.Get());


  const char* fileName = vtkTestUtilities::ExpandDataFileName(argc, argv,
                                                              "Data/bunny.ply");

  vtkNew<vtkPLYReader> reader;
  reader->SetFileName(fileName);
  reader->Update();
  vtkNew<vtkPolyDataNormals> computeNormals;
  computeNormals->SetInputConnection(reader->GetOutputPort());
  computeNormals->Update();

  mapper->SetInputConnection(computeNormals->GetOutputPort());
  actor->SetMapper(mapper.Get());
  actor->GetProperty()->SetDiffuseColor(1.0, 0.65, 0.7);
  actor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  actor->GetProperty()->SetSpecular(0.5);
  actor->GetProperty()->SetOpacity(1.0);

  vtkNew<vtkRenderWindowInteractor> interactor;
  interactor->SetRenderWindow(renderWindow.Get());
  renderWindow->SetMultiSamples(0);
  interactor->Initialize();

  vtkNew<vtkTimerLog> timer;
  double time(0.0);
  for (int i = 0; i < 10; ++i)
    {
    timer->StartTimer();
    renderWindow->Render();
    timer->StopTimer();
    cout << "Rendering frame " << i << ": " << timer->GetElapsedTime() << endl;
    time += timer->GetElapsedTime();
    }
  cout << "Average time: " << time / 10.0 << endl;

  interactor->Start();

  delete [] fileName;

  return EXIT_SUCCESS;
}
