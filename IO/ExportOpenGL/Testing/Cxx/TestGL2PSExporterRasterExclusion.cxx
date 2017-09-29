/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestGL2PSExporterRaster.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkTestUtilities.h"
#include "vtkRegressionTestImage.h"
#include "vtkGL2PSExporter.h"

#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkCubeAxesActor.h"
#include "vtkConeSource.h"
#include "vtkNew.h"
#include "vtkPolyDataMapper.h"
#include "vtkProp3DCollection.h"
#include "vtkProperty.h"
#include "vtkProperty2D.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkSmartPointer.h"
#include "vtkTestingInteractor.h"
#include "vtkTextActor.h"
#include "vtkTextProperty.h"

#include <string>

int TestGL2PSExporterRasterExclusion( int, char *[] )
{
  vtkNew<vtkConeSource> coneSource;
  vtkNew<vtkPolyDataMapper> coneMapper;
  vtkNew<vtkActor> coneActor;
  coneSource->SetResolution(25);
  coneMapper->SetInputConnection(coneSource->GetOutputPort());
  coneActor->SetMapper(coneMapper);
  coneActor->GetProperty()->SetColor(0.5, 0.5, 1.0);

  vtkNew<vtkTextActor> text1;
  text1->SetDisplayPosition(250, 435);
  text1->SetInput("Test\nmultiline\ntext"); // Won't render properly
  text1->GetTextProperty()->SetFontSize(18);
  text1->GetTextProperty()->SetFontFamilyToArial();
  text1->GetTextProperty()->SetJustificationToCentered();
  text1->GetTextProperty()->BoldOn();
  text1->GetTextProperty()->ItalicOn();
  text1->GetTextProperty()->SetColor(0.0, 0.0, 1.0);

  vtkNew<vtkTextActor> text2;
  text2->SetDisplayPosition(400, 250);
  text2->SetInput("Test rotated text");
  text2->GetTextProperty()->SetFontSize(22);
  text2->GetTextProperty()->SetFontFamilyToTimes();
  text2->GetTextProperty()->SetJustificationToCentered();
  text2->GetTextProperty()->SetVerticalJustificationToCentered();
  text2->GetTextProperty()->BoldOn();
  text2->GetTextProperty()->SetOrientation(45);
  text2->GetTextProperty()->SetColor(1.0, 0.0, 0.0);

  vtkNew<vtkTextActor> text3;
  text3->SetDisplayPosition(20, 20);
  text3->SetInput("Big text!");
  text3->GetTextProperty()->SetFontSize(45);
  text3->GetTextProperty()->SetFontFamilyToCourier();
  text3->GetTextProperty()->SetJustificationToLeft();
  text3->GetTextProperty()->BoldOn();
  text3->GetTextProperty()->SetOrientation(0);
  text3->GetTextProperty()->SetColor(0.2, 1.0, 0.2);

  vtkNew<vtkCubeAxesActor> axes;
  coneMapper->Update();
  axes->SetBounds(coneMapper->GetBounds());

  vtkNew<vtkRenderer> ren;
  axes->SetCamera(ren->GetActiveCamera());
  ren->AddActor(coneActor);
  ren->AddActor(axes);
  ren->AddActor(text1);
  ren->AddActor(text2);
  ren->AddActor(text3);
  ren->SetBackground(0.8, 0.8, 0.8);

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetMultiSamples(0);
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renWin);

  vtkSmartPointer<vtkCamera> camera = ren->GetActiveCamera();
  ren->ResetCamera();
  camera->Azimuth(30);

  renWin->SetSize(500, 500);
  renWin->Render();

  vtkNew<vtkProp3DCollection> exclusions;
  exclusions->AddItem(axes);

  vtkNew<vtkGL2PSExporter> exp;
  exp->SetRenderWindow(renWin);
  exp->SetFileFormatToPS();
  exp->CompressOff();
  exp->SetSortToBSP();
  exp->DrawBackgroundOn();
  exp->Write3DPropsAsRasterImageOn();
  exp->SetRasterExclusions(exclusions);

  std::string fileprefix = vtkTestingInteractor::TempDirectory +
      std::string("/TestGL2PSExporterRasterExclusion");

  exp->SetFilePrefix(fileprefix.c_str());
  exp->Write();

  renWin->GetInteractor()->Initialize();
  renWin->GetInteractor()->Start();

  return EXIT_SUCCESS;
}
