/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRotationalExtrusionFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkRotationalExtrusionFilter - sweep polygonal data creating "skirt" from free edges and lines, and lines from vertices
// .SECTION Description
// vtkRotationalExtrusionFilter is a modeling filter. It takes polygonal
// data as input and generates polygonal data on output. The input dataset
// is swept around the z-axis to create new polygonal primitives. These
// primitives form a "skirt" or swept surface. For example, sweeping a
// line results in a cylindrical shell, and sweeping a circle creates a
// torus.
//
// There are a number of control parameters for this filter. You can
// control whether the sweep of a 2D object (i.e., polygon or triangle
// strip) is capped with the generating geometry via the "Capping" instance
// variable. Also, you can control the angle of rotation, and whether
// translation along the z-axis is performed along with the rotation.
// (Translation is useful for creating "springs".) You also can adjust
// the radius of the generating geometry using the "DeltaRotation" instance
// variable.
//
// The skirt is generated by locating certain topological features. Free
// edges (edges of polygons or triangle strips only used by one polygon or
// triangle strips) generate surfaces. This is true also of lines or
// polylines. Vertices generate lines.
//
// This filter can be used to model axisymmetric objects like cylinders,
// bottles, and wine glasses; or translational/rotational symmetric objects
// like springs or corkscrews.

// .SECTION Caveats
// If the object sweeps 360 degrees, radius does not vary, and the object
// does not translate, capping is not performed. This is because the cap
// is unnecessary.
//
// Some polygonal objects have no free edges (e.g., sphere). When swept,
// this will result in two separate surfaces if capping is on, or no surface
// if capping is off.

// .SECTION See Also
// vtkLinearExtrusionFilter

#ifndef vtkRotationalExtrusionFilter_h
#define vtkRotationalExtrusionFilter_h

#include "vtkFiltersModelingModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class VTKFILTERSMODELING_EXPORT vtkRotationalExtrusionFilter : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkRotationalExtrusionFilter,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Create object with capping on, angle of 360 degrees, resolution = 12, and
  // no translation along z-axis.
  // vector (0,0,1), and point (0,0,0).
  static vtkRotationalExtrusionFilter *New();

  // Description:
  // Set/Get resolution of sweep operation. Resolution controls the number
  // of intermediate node points.
  vtkSetClampMacro(Resolution,int,1,VTK_INT_MAX);
  vtkGetMacro(Resolution,int);

  // Description:
  // Turn on/off the capping of the skirt.
  vtkSetMacro(Capping,int);
  vtkGetMacro(Capping,int);
  vtkBooleanMacro(Capping,int);

  // Description:
  // Set/Get angle of rotation.
  vtkSetMacro(Angle,double);
  vtkGetMacro(Angle,double);

  // Description:
  // Set/Get total amount of translation along the z-axis.
  vtkSetMacro(Translation,double);
  vtkGetMacro(Translation,double);

  // Description:
  // Set/Get change in radius during sweep process.
  vtkSetMacro(DeltaRadius,double);
  vtkGetMacro(DeltaRadius,double);

protected:
  vtkRotationalExtrusionFilter();
  ~vtkRotationalExtrusionFilter() {}

  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
  int Resolution;
  int Capping;
  double Angle;
  double Translation;
  double DeltaRadius;
private:
  vtkRotationalExtrusionFilter(const vtkRotationalExtrusionFilter&);  // Not implemented.
  void operator=(const vtkRotationalExtrusionFilter&);  // Not implemented.
};

#endif
