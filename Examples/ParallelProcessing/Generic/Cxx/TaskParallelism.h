/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TaskParallelism.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#ifndef __TASKPARA_H
#define __TASKPARA_H

#include "vtkActor.h"
#include "vtkAssignAttribute.h"
#include "vtkAttributeDataToFieldDataFilter.h"
#include "vtkCamera.h"
#include "vtkContourFilter.h"
#include "vtkDataSetMapper.h"
#include "vtkFieldDataToAttributeDataFilter.h"
#include "vtkGlyph3D.h"
#include "vtkGlyphSource2D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageGradient.h"
#include "vtkImageGradientMagnitude.h"
#include "vtkImageShrink3D.h"
#include "vtkMPIController.h"
#include "vtkProbeFilter.h"
#include "vtkRTAnalyticSource.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"


typedef vtkPolyDataMapper* (*taskFunction)(vtkRenderWindow* renWin,
                                           double data, vtkCamera* cam);


vtkPolyDataMapper* task1(vtkRenderWindow* renWin, double data,vtkCamera* cam);
vtkPolyDataMapper* task2(vtkRenderWindow* renWin, double data,vtkCamera* cam);


static const double EXTENT = 20;

static const int WINDOW_WIDTH = 400;
static const int WINDOW_HEIGHT = 300;

#endif
