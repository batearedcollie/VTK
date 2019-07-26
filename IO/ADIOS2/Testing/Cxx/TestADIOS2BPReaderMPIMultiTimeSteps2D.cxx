/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestTemporalXdmfReaderWriter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// Description:
// This tests reading of a simple ADIOS2 bp file.

#include "vtkADIOS2CoreImageReader.h"

#include "vtkActor.h"
#include "vtkAlgorithm.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkCompositeRenderManager.h"
#include "vtkDataArray.h"
#include "vtkDataSetMapper.h"
#include "vtkImageData.h"
#include "vtkImageDataToPointSet.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkMultiPieceDataSet.h"
#include "vtkMPIController.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkUnsignedIntArray.h"
#include "vtkXMLPMultiBlockDataWriter.h"

#include "vtkTestUtilities.h"
#include "vtkNew.h"

#include <sstream> // istringstream

struct TestArgs
{
  int *retval;
  int argc;
  char **argv;
};

void TestADIOS2BPReaderMPIMultiTimeSteps2D(vtkMultiProcessController* controller, void* _args)
{
  TestArgs *args = reinterpret_cast<TestArgs *>(_args);
  int argc = args->argc;
  char **argv = args->argv;
  *(args->retval) = 1;

  int currentRank = controller->GetLocalProcessId();
  vtkNew<vtkADIOS2CoreImageReader> reader;

  // Read the input data file
  char* filePath = vtkTestUtilities::ExpandDataFileName(argc, argv,
                              "Data/ADIOS2/2D_HeatTransfer_10-step/sim.bp");

  if (!reader->CanReadFile(filePath))
  {
    std::cerr << "Cannot read file " << reader->GetFileName() << std::endl;
    return;
  }
  reader->SetFileName(filePath);
  delete [] filePath;

  reader->SetController(controller);

  reader->UpdateInformation();
  auto& availVars = reader->GetAvilableVariables();
  assert(availVars.size() == 1);
  // Get the dimension
  std::string varName = availVars.begin()->first;

  reader->SetDimensionArray("T");
  reader->SetTimeStepArray("T");

  reader->Update();

  vtkSmartPointer<vtkMultiBlockDataSet> output = vtkMultiBlockDataSet::SafeDownCast(reader->GetOutput());
  assert(output->GetNumberOfBlocks() == 1);
  vtkSmartPointer<vtkMultiPieceDataSet> mpds = vtkMultiPieceDataSet::SafeDownCast(output->GetBlock(0));
  assert(mpds->GetNumberOfPieces() == 2);
  vtkSmartPointer<vtkImageData> image0 =  vtkImageData::SafeDownCast(mpds->GetPiece(0));
  vtkSmartPointer<vtkImageData> image1 =  vtkImageData::SafeDownCast(mpds->GetPiece(1));

  vtkNew<vtkImageDataToPointSet> imageToPointset;
  if (currentRank == 0)
  { // Rank0 should read one block as vtkImageData into index 0
    assert(image0);
    //assert(!image1);
    assert(image0->GetCellData()->GetNumberOfArrays() == 1);
    image0->GetCellData()->SetActiveScalars("T");
    imageToPointset->SetInputData(image0);
  }
  else if (currentRank == 1)
  { // Rank1 should read one block as vtkImageData into index 1
    assert(!image0);
    assert(image1);
    assert(image1->GetCellData()->GetNumberOfArrays() == 1);
    image1->GetCellData()->SetActiveScalars("T");
    imageToPointset->SetInputData(image1);
  }

  imageToPointset->Update();

  // Since I fail to find a proper mapper to render two vtkImageDatas inside
  // a vtkMultiPieceDataSet in a vtkMultiBlockDataSet, I render the image directly here
  vtkNew<vtkDataSetMapper> mapper;
  mapper->SetInputDataObject(imageToPointset->GetOutput());
  mapper->ScalarVisibilityOn();
  mapper->SetScalarRange(0, 2000);
  mapper->SetScalarModeToUseCellData();
  mapper->ColorByArrayComponent("T", 0);

  vtkNew<vtkActor> actor;
  actor->SetMapper(mapper);
  actor->GetProperty()->EdgeVisibilityOn();

  vtkNew<vtkCompositeRenderManager> prm;

  vtkSmartPointer<vtkRenderer> renderer;
  renderer.TakeReference(prm->MakeRenderer());
  renderer->AddActor(actor);
  renderer->SetBackground(0.5, 0.5, 0.5);
  renderer->GetActiveCamera()->Elevation(300);
  renderer->GetActiveCamera()->Yaw(60);
  renderer->ResetCamera();

  vtkSmartPointer<vtkRenderWindow> rendWin;
  rendWin.TakeReference(prm->MakeRenderWindow());
  rendWin->SetSize(600, 300);
  rendWin->SetPosition(0, 200 * controller->GetLocalProcessId());
  rendWin->AddRenderer(renderer);

  prm->SetRenderWindow(rendWin);
  prm->SetController(controller);
  prm->InitializePieces();
  prm->InitializeOffScreen(); // Mesa GL only

  if (controller->GetLocalProcessId() == 0)
  {
    rendWin->Render();

    // Do the test comparsion
    int retval = vtkRegressionTestImage(rendWin);
    if (retval == vtkRegressionTester::DO_INTERACTOR)
    {
      vtkNew<vtkRenderWindowInteractor> iren;
      iren->SetRenderWindow(rendWin);
      iren->Initialize();
      iren->Start();
      retval = vtkRegressionTester::PASSED;
    }
    std::cout <<"retval after render=" <<retval <<std::endl;
    *(args->retval) = (retval == vtkRegressionTester::PASSED) ? 0 : 1;

    prm->StopServices();
  }
  else // not root node
  {
    prm->StartServices();
  }

  controller->Broadcast(args->retval, 1, 0);
}


int TestADIOS2BPReaderMPIMultiTimeSteps2D(int argc, char* argv[])
{
  int retval{1};

  // Note that this will create a vtkMPIController if MPI
  // is configured, vtkThreadedController otherwise.
  vtkNew<vtkMPIController> controller;
  controller->Initialize(&argc, &argv);

  vtkMultiProcessController::SetGlobalController(controller);

  TestArgs args;
  args.retval = &retval;
  args.argc = argc;
  args.argv = argv;

  controller->SetSingleMethod(TestADIOS2BPReaderMPIMultiTimeSteps2D, &args);
  controller->SingleMethodExecute();

  controller->Finalize();

  return retval;
}
