/*=========================================================================

Copyright (c) Kitware Inc.
All rights reserved.

=========================================================================*/
// .SECTION Description
// This program illustrates the use of the vtkHyperTreeGrid
// data set and various filters acting upon hyper it.
// It generates output files in VTK format.
//
// .SECTION Usage
//
// .SECTION Thanks
// This program was written by Daniel Aguilera and Philippe Pebay
// This work was supported by Commissariat a l'Energie Atomique (CEA/DIF)

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkShrinkFilter.h>
#include <vtkProperty.h>
#include <vtkUnstructuredGridWriter.h>
#include <vtkInteractorStyleSwitch.h>

#include "Mesh.h"
#include "Cell.h"
#include "Node.h"

using namespace std;

#define SHIFT_ARGS() for (int j=i;j<(argc-1);j++) argv[j] = argv[j+1]; argc--; i--
#define SHIFT_NARGS(n) for (int j=i;j<(argc-(n));j++) argv[j] = argv[j+(n)]; argc-=(n); i--

void usage ()
{
  cout << "Usage : amr [-level <int>] [-refine <int>] [-nx <int>] [-ny <int>] [-nz <int>] [-write <file>] [-shrink] [-help]" << endl;
  cout << "   -level  : nombre de raffinement. Defaut = 3" << endl;
  cout << "   -refine : nombre de decoupage dans chaque direction. Defaut = 3" << endl;
  cout << "   -n[xyz] : dimension en nombre de noeuds pour chaque direction. Defaut = 5" << endl;
  cout << "   -write  : ecriture dans le fichier specifie avant affichage. Defaut = pas d'ecriture" << endl;
  cout << "   -shrink : application d'un filtre vtkShrink avant affichage. Defaut = pas de shrink" << endl;
  cout << "   -help   : affichage de cette aide" << endl;
  exit (0);
}

int main( int argc, char *argv[] )
{
  int nx = 5;
  int ny = 5;
  int nz = 5;
  int level = 3; // profondeur de raffinement
  int refine = 3; // nombre de decoupage de la maille dans chaque direction
  bool shrink = false;
  string datafile = "";
  double R = 0.0;

  for (int i = 1; i < argc; i++)
    {
    // Refinement depth
    if (strcmp (argv[i], "-level") == 0)
      {
      if (i+1 < argc) {level = atoi (argv[i+1]); SHIFT_NARGS(2);}
      else usage();
      }
    // Franch factor
    else if (strcmp (argv[i], "-refine") == 0)
      {
      if (i+1 < argc) {refine = atoi (argv[i+1]); SHIFT_NARGS(2);}
      else usage();
      }
    // Dimensions
    else if (strcmp (argv[i], "-nx") == 0)
      {
      if (i+1 < argc) {nx = atoi (argv[i+1]); SHIFT_NARGS(2);}
      else usage();
      }
    else if (strcmp (argv[i], "-ny") == 0)
      {
      if (i+1 < argc) {ny = atoi (argv[i+1]); SHIFT_NARGS(2);}
      else usage();
      }
    else if (strcmp (argv[i], "-nz") == 0)
      {
      if (i+1 < argc) {nz = atoi (argv[i+1]); SHIFT_NARGS(2);}
      else usage();
      }
    else if (strcmp (argv[i], "-write") == 0)
      {
      if (i+1 < argc) {datafile = argv[i+1]; SHIFT_NARGS(2);}
      else usage();
      }
    else if (strcmp (argv[i], "-shrink") == 0)
      {
      shrink = true; SHIFT_ARGS();
      }
    else usage();
    }

  // si le rayon n'est pas defini on prend une sphere visible suivant X
  if (R == 0.0) R = nx;
  Cell::setR(R);
   
  Node * n1 = new Node (0.0,           0.0,           0.0);
  Node * n2 = new Node ((double) nx+1, 0.0,           0.0);
  Node * n3 = new Node ((double) nx+1, 0.0,           (double) nz+1);
  Node * n4 = new Node (0.0,           0.0,           (double) nz+1);
  Node * n5 = new Node (0.0,           (double) ny+1, 0.0);
  Node * n6 = new Node ((double) nx+1, (double) ny+1, 0.0);
  Node * n7 = new Node ((double) nx+1, (double) ny+1, (double) nz+1);
  Node * n8 = new Node (0.0,           (double) ny+1, (double) nz+1);

  // Create mesh
  Mesh * mesh = new Mesh (nx, ny, nz, n1, n2, n3, n4, n5, n6, n7, n8);
  mesh->setRefine (refine);
  for (int i = 0; i < level; i++) mesh->refine();

  // reduction des points
  mesh->mergePoints();

  // generation du dataset
  vtkDataSet * ds = mesh->getDataSet();

  // reduction des mailles
  vtkShrinkFilter * shrinkFilter = vtkShrinkFilter::New();
  if (shrink)
    {
    shrinkFilter->SetShrinkFactor (0.9);
    shrinkFilter->SetInputData (ds);
    shrinkFilter->Update();
    ds = shrinkFilter->GetOutput();
    }

  // ecriture du dataset
  if (datafile != "")
    {
    vtkUnstructuredGridWriter * writer = vtkUnstructuredGridWriter::New();
    writer->SetInputData(ds);
    writer->SetFileName (datafile.c_str());
    writer->Write();
    writer->Delete();
    }

  // Geometry filter
  vtkDataSetSurfaceFilter * dataSetSurfaceFilter = vtkDataSetSurfaceFilter::New();
  dataSetSurfaceFilter->SetInputData(ds);

  // Mappers
  vtkPolyDataMapper * polyDataMapper1 = vtkPolyDataMapper::New();
  polyDataMapper1->SetInputConnection(dataSetSurfaceFilter->GetOutputPort());
  polyDataMapper1->SetResolveCoincidentTopologyToPolygonOffset();
  polyDataMapper1->SetResolveCoincidentTopologyPolygonOffsetParameters( 0, 1 );
  vtkPolyDataMapper * polyDataMapper2 = vtkPolyDataMapper::New();
  polyDataMapper2->SetInputConnection(dataSetSurfaceFilter->GetOutputPort());
  polyDataMapper2->SetResolveCoincidentTopologyToPolygonOffset();
  polyDataMapper2->SetResolveCoincidentTopologyPolygonOffsetParameters( 1, 1 );

  // Actors
  vtkActor *actor1 = vtkActor::New();
  actor1->GetProperty()->SetColor(.8,.2,.2);
  actor1->SetMapper (polyDataMapper1);
  vtkActor *actor2 = vtkActor::New();
  actor2->GetProperty()->SetRepresentationToWireframe();
  actor2->GetProperty()->SetColor( .7, .7, .7 );
  actor2->SetMapper (polyDataMapper2);

  // Window and interactor
  vtkRenderer * ren = vtkRenderer::New();
  ren->SetBackground (0.8,0.8,0.8);
  ren->AddActor(actor1);
  ren->AddActor(actor2);

  vtkRenderWindow * renWindow = vtkRenderWindow::New();
  renWindow->SetSize (800,800);
  renWindow->AddRenderer(ren);

  vtkRenderWindowInteractor * interacteur = vtkRenderWindowInteractor::New();
  vtkInteractorStyleSwitch * style = vtkInteractorStyleSwitch::SafeDownCast (interacteur->GetInteractorStyle());
  interacteur->SetRenderWindow(renWindow);
  if (style) style->SetCurrentStyleToTrackballCamera ();

  // premier rendu
  renWindow->Render();

  interacteur->Start();
   
  // menage
  delete mesh;
  delete n1;
  delete n2;
  delete n3;
  delete n4;
  delete n5;
  delete n6;
  delete n7;
  delete n8;

  shrinkFilter->Delete();
  dataSetSurfaceFilter->Delete();
  polyDataMapper1->Delete();
  polyDataMapper2->Delete();
  actor1->Delete();
  actor2->Delete();
  ren->Delete();
  renWindow->Delete();
  interacteur->Delete();

  return 0;
}
