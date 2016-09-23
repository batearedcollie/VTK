/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestGPURayCastAdditive.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// This test covers switch from perspective to parallel projection.
// This test volume renders a synthetic dataset with unsigned char values,
// with the composite method.

#include "vtkNew.h"
#include "vtkSphere.h"
#include "vtkSampleFunction.h"

#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkTestUtilities.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkVolumeProperty.h"
#include "vtkCamera.h"
#include "vtkRegressionTestImage.h"
#include "vtkImageShiftScale.h"
#include "vtkImageData.h"
#include "vtkPointData.h"
#include "vtkDataArray.h"
#include "vtkOSPRayPass.h"


int TestGPURayCastPerspectiveParallel(int argc,
                                      char *argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;
  bool useOSP = true;
  for (int i = 0; i < argc; i++)
  {
    if (!strcmp(argv[i], "-GL"))
    {
      cerr << "GL" << endl;
      useOSP = false;
    }
  }

  // Create a spherical implicit function.
  vtkSphere *shape=vtkSphere::New();
  shape->SetRadius(0.1);
  shape->SetCenter(0.0,0.0,0.0);

  vtkSampleFunction *source=vtkSampleFunction::New();
  source->SetImplicitFunction(shape);
  shape->Delete();
  source->SetOutputScalarTypeToDouble();
  source->SetSampleDimensions(127,127,127); // intentional NPOT dimensions.
  source->SetModelBounds(-1.0,1.0,-1.0,1.0,-1.0,1.0);
  source->SetCapping(false);
  source->SetComputeNormals(false);
  source->SetScalarArrayName("values");

  source->Update();

  vtkDataArray *a=source->GetOutput()->GetPointData()->GetScalars("values");
  double range[2];
  a->GetRange(range);

  vtkImageShiftScale *t=vtkImageShiftScale::New();
  t->SetInputConnection(source->GetOutputPort());
  source->Delete();
  t->SetShift(-range[0]);
  double magnitude=range[1]-range[0];
  if(magnitude==0.0)
  {
    magnitude=1.0;
  }
  t->SetScale(255.0/magnitude);
  t->SetOutputScalarTypeToUnsignedChar();

  t->Update();

  vtkRenderWindow *renWin=vtkRenderWindow::New();
  vtkRenderer *ren1=vtkRenderer::New();
  ren1->SetBackground(0.1,0.4,0.2);

  renWin->AddRenderer(ren1);
  ren1->Delete();
  renWin->SetSize(301,300); // intentional odd and NPOT  width/height

  vtkRenderWindowInteractor *iren=vtkRenderWindowInteractor::New();
  iren->SetRenderWindow(renWin);
  renWin->Delete();

  renWin->Render(); // make sure we have an OpenGL context.

  vtkGPUVolumeRayCastMapper *volumeMapper;
  vtkVolumeProperty *volumeProperty;
  vtkVolume *volume;

  volumeMapper=vtkGPUVolumeRayCastMapper::New();
  volumeMapper->SetBlendModeToComposite();
  volumeMapper->SetInputConnection(
    t->GetOutputPort());

  volumeProperty=vtkVolumeProperty::New();
  volumeProperty->ShadeOff();
  volumeProperty->SetInterpolationType(VTK_LINEAR_INTERPOLATION);

  vtkPiecewiseFunction *compositeOpacity = vtkPiecewiseFunction::New();
  compositeOpacity->AddPoint(0.0,0.0);
  compositeOpacity->AddPoint(80.0,1.0);
  compositeOpacity->AddPoint(80.1,0.0);
  compositeOpacity->AddPoint(255.0,0.0);
  volumeProperty->SetScalarOpacity(compositeOpacity);

  vtkColorTransferFunction *color=vtkColorTransferFunction::New();
  color->AddRGBPoint(0.0  ,0.0,0.0,1.0);
  color->AddRGBPoint(40.0  ,1.0,0.0,0.0);
  color->AddRGBPoint(255.0,1.0,1.0,1.0);
  volumeProperty->SetColor(color);
  color->Delete();

  volume=vtkVolume::New();
  volume->SetMapper(volumeMapper);
  volume->SetProperty(volumeProperty);
  ren1->AddViewProp(volume);

// Attach OSPRay render pass
  vtkNew<vtkOSPRayPass> osprayPass;
  if (useOSP)
  {
    ren1->SetPass(osprayPass.GetPointer());
  }

  int valid=volumeMapper->IsRenderSupported(renWin,volumeProperty);

  int retVal;
  if(valid)
  {
    ren1->ResetCamera();

    // Render composite. Default camera is perpective.
    renWin->Render();

    // Switch to parallel
    vtkCamera *c=ren1->GetActiveCamera();
    c->SetParallelProjection(true);
    renWin->Render();

    retVal = vtkTesting::Test(argc, argv, renWin, 75);
    if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
      iren->Start();
    }
  }
  else
  {
    retVal=vtkTesting::PASSED;
    cout << "Required extensions not supported." << endl;
  }

  volumeMapper->Delete();
  volumeProperty->Delete();
  volume->Delete();
  iren->Delete();
  t->Delete();
  compositeOpacity->Delete();

  return !((retVal == vtkTesting::PASSED) || (retVal == vtkTesting::DO_INTERACTOR));
}
