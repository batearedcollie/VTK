/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestAMRIterator.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkAMRBox.h"
#include "vtkOverlappingAMR.h"
#include "vtkAMRGaussianPulseSource.h"
#include "vtkCompositeDataIterator.h"

#include <iostream>
#include <string>

//-----------------------------------------------------------------------------
int TestAMRIterator( int, char *[] )
{
  int expected[3][2] =
    {
     {0,0},
     {1,0},
     {1,1}
    };

  int rc = 0;

  vtkAMRGaussianPulseSource *amrSource = vtkAMRGaussianPulseSource::New();
  amrSource->Update();
  vtkOverlappingAMR *amrData =
      vtkOverlappingAMR::SafeDownCast( amrSource->GetOutput() );


  vtkCompositeDataIterator *iter = amrData->NewIterator();
  iter->InitTraversal();
  for(int idx=0 ;!iter->IsDoneWithTraversal(); iter->GoToNextItem(),++idx )
    {
    vtkAMRBox amrBox = amrData->GetAMRBox( iter );
    cout << "Level: " << amrBox.GetLevel() << " Block: " << amrBox.GetBlockId();
    cout << endl;
    if( amrBox.GetLevel() != expected[idx][0] )
      {
      ++rc;
      }
    if( amrBox.GetBlockId() != expected[idx][1] )
      {
      ++rc;
      }
    } // END for
  iter->Delete();

  amrSource->Delete();
  return( rc );
}
