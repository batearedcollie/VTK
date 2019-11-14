/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestNIFTI2.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*
Test NIFTI-2 support in VTK.
*/

#include "vtkNew.h"
#include "vtkRegressionTestImage.h"
#include "vtkTestUtilities.h"

#include "vtkCamera.h"
#include "vtkImageData.h"
#include "vtkImageMathematics.h"
#include "vtkImageProperty.h"
#include "vtkImageSlice.h"
#include "vtkImageSliceMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "vtkNIFTIImageHeader.h"
#include "vtkNIFTIImageReader.h"
#include "vtkNIFTIImageWriter.h"

#include <string>

static const char *dispfile = "Data/avg152T1_RL_nifti2.nii.gz";

static void TestDisplay(
  vtkRenderWindow *renwin, const char *infile, const char *tempDir)
{
  std::string outpath = tempDir;
  outpath += "/";
  outpath += "avg152T1_RL_nifti2.nii.gz";

  vtkNew<vtkNIFTIImageReader> reader1;
  if (!reader1->CanReadFile(infile))
  {
    cerr << "CanReadFile failed for " << infile << "\n";
    exit(1);
  }

  reader1->SetFileName(infile);
  reader1->Update();

  vtkNew<vtkNIFTIImageWriter> writer;
  writer->SetInputConnection(reader1->GetOutputPort());
  writer->SetFileName(outpath.c_str());
  writer->SetNIFTIHeader(reader1->GetNIFTIHeader());
  writer->SetSFormMatrix(reader1->GetSFormMatrix());
  writer->SetNIFTIVersion(2);
  writer->Update();

  vtkNew<vtkNIFTIImageReader> reader;
  reader->SetFileName(outpath.c_str());
  reader->Update();

  vtkNIFTIImageHeader *header =
    reader->GetNIFTIHeader();
  std::string magic = header->GetMagic();
  if (magic != "n+2")
  {
    cerr << "File is not a NIFTIv2 file\n";
    exit(1);
  }

  int size[3];
  double center[3], spacing[3];
  reader->GetOutput()->GetDimensions(size);
  reader->GetOutput()->GetCenter(center);
  reader->GetOutput()->GetSpacing(spacing);
  double center1[3] = { center[0], center[1], center[2] };
  double center2[3] = { center[0], center[1], center[2] };
  if (size[2] % 2 == 1)
  {
    center1[2] += 0.5*spacing[2];
  }
  if (size[0] % 2 == 1)
  {
    center2[0] += 0.5*spacing[0];
  }
  double vrange[2];
  reader->GetOutput()->GetScalarRange(vrange);

  vtkNew<vtkImageSliceMapper> map1;
  map1->BorderOn();
  map1->SliceAtFocalPointOn();
  map1->SliceFacesCameraOn();
  map1->SetInputConnection(reader->GetOutputPort());
  vtkNew<vtkImageSliceMapper> map2;
  map2->BorderOn();
  map2->SliceAtFocalPointOn();
  map2->SliceFacesCameraOn();
  map2->SetInputConnection(reader->GetOutputPort());

  vtkNew<vtkImageSlice> slice1;
  slice1->SetMapper(map1);
  slice1->GetProperty()->SetColorWindow(vrange[1]-vrange[0]);
  slice1->GetProperty()->SetColorLevel(0.5*(vrange[0]+vrange[1]));

  vtkNew<vtkImageSlice> slice2;
  slice2->SetMapper(map2);
  slice2->GetProperty()->SetColorWindow(vrange[1]-vrange[0]);
  slice2->GetProperty()->SetColorLevel(0.5*(vrange[0]+vrange[1]));

  double ratio = size[0]*1.0/(size[0]+size[2]);

  vtkNew<vtkRenderer> ren1;
  ren1->SetViewport(0,0,ratio,1.0);

  vtkNew<vtkRenderer> ren2;
  ren2->SetViewport(ratio,0.0,1.0,1.0);
  ren1->AddViewProp(slice1);
  ren2->AddViewProp(slice2);

  vtkCamera *cam1 = ren1->GetActiveCamera();
  cam1->ParallelProjectionOn();
  cam1->SetParallelScale(0.5*spacing[1]*size[1]);
  cam1->SetFocalPoint(center1[0], center1[1], center1[2]);
  cam1->SetPosition(center1[0], center1[1], center1[2] - 100.0);

  vtkCamera *cam2 = ren2->GetActiveCamera();
  cam2->ParallelProjectionOn();
  cam2->SetParallelScale(0.5*spacing[1]*size[1]);
  cam2->SetFocalPoint(center2[0], center2[1], center2[2]);
  cam2->SetPosition(center2[0] + 100.0, center2[1], center2[2]);

  renwin->SetSize(size[0] + size[2], size[1]);
  renwin->AddRenderer(ren1);
  renwin->AddRenderer(ren2);
};

int TestNIFTI2(int argc, char *argv[])
{
  // perform the display test
  char *infile =
    vtkTestUtilities::ExpandDataFileName(argc, argv, dispfile);
  if (!infile)
  {
    cerr << "Could not locate input file " << dispfile << "\n";
    return 1;
  }
  std::string inpath = infile;
  delete [] infile;

  char *tempDir = vtkTestUtilities::GetArgOrEnvOrDefault(
    "-T", argc, argv, "VTK_TEMP_DIR", "Testing/Temporary");
  if (!tempDir)
  {
    cerr << "Could not determine temporary directory.\n";
    return 1;
  }
  std::string tmppath = tempDir;
  delete [] tempDir;

  vtkNew<vtkRenderWindow> renwin;
  vtkNew<vtkRenderWindowInteractor> iren;
  iren->SetRenderWindow(renwin);

  TestDisplay(renwin, inpath.c_str(), tmppath.c_str());

  int retVal = vtkRegressionTestImage(renwin);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    renwin->Render();
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
  }

  return (retVal != vtkRegressionTester::PASSED);
}
