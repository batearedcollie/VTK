/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHull.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkHull
 * @brief   produce an n-sided convex hull
 *
 * vtkHull is a filter which will produce an n-sided convex hull given a
 * set of n planes. (The convex hull bounds the input polygonal data.)
 * The hull is generated by squeezing the planes towards the input
 * vtkPolyData, until the planes just touch the vtkPolyData. Then,
 * the resulting planes are used to generate a polyhedron (i.e., hull)
 * that is represented by triangles.
 *
 * The n planes can be defined in a number of ways including 1) manually
 * specifying each plane; 2) choosing the six face planes of the input's
 * bounding box; 3) choosing the eight vertex planes of the input's
 * bounding box; 4) choosing the twelve edge planes of the input's
 * bounding box; and/or 5) using a recursively subdivided octahedron.
 * Note that when specifying planes, the plane normals should point
 * outside of the convex region.
 *
 * The output of this filter can be used in combination with vtkLODActor
 * to represent a levels-of-detail in the LOD hierarchy. Another use of
 * this class is to manually specify the planes, and then generate the
 * polyhedron from the planes (without squeezing the planes towards the
 * input). The method GenerateHull() is used to do this.
*/

#ifndef vtkHull_h
#define vtkHull_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkCellArray;
class vtkPlanes;
class vtkPoints;
class vtkPolyData;

class VTKFILTERSCORE_EXPORT vtkHull : public vtkPolyDataAlgorithm
{
public:
  static vtkHull *New();
  vtkTypeMacro(vtkHull,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) VTK_OVERRIDE;

  /**
   * Remove all planes from the current set of planes.
   */
  void RemoveAllPlanes( void );

  //@{
  /**
   * Add a plane to the current set of planes. It will be added at the
   * end of the list, and an index that can later be used to set this
   * plane's normal will be returned. The values A, B, C are from the
   * plane equation Ax + By + Cz + D = 0. This vector does not have to
   * have unit length (but it must have a non-zero length!). If a value
   * 0 > i >= -NumberOfPlanes is returned, then the plane is parallel
   * with a previously inserted plane, and |-i-1| is the index of the
   * plane that was previously inserted. If a value i < -NumberOfPlanes
   * is returned, then the plane normal is zero length.
   */
  int  AddPlane( double A, double B, double C );
  int  AddPlane( double plane[3] );
  //@}

  //@{
  /**
   * Set the normal values for plane i. This is a plane that was already
   * added to the current set of planes with AddPlane(), and is now being
   * modified. The values A, B, C are from the plane equation
   * Ax + By + Cz + D = 0. This vector does not have to have unit length.
   * Note that D is set to zero, except in the case of the method taking
   * a vtkPlanes* argument, where it is set to the D value defined there.
   */
  void SetPlane( int i, double A, double B, double C );
  void SetPlane( int i, double plane[3] );
  //@}

  //@{
  /**
   * Variations of AddPlane()/SetPlane() that allow D to be set. These
   * methods are used when GenerateHull() is used.
   */
  int AddPlane( double A, double B, double C, double D );
  int AddPlane( double plane[3], double D );
  void SetPlane( int i, double A, double B, double C, double D );
  void SetPlane( int i, double plane[3], double D );
  //@}

  /**
   * Set all the planes at once using a vtkPlanes implicit function.
   * This also sets the D value, so it can be used with GenerateHull().
   */
  void SetPlanes( vtkPlanes *planes );

  //@{
  /**
   * Get the number of planes in the current set of planes.
   */
  vtkGetMacro( NumberOfPlanes, int );
  //@}

  /**
   * Add the 8 planes that represent the vertices of a cube - the combination
   * of the three face planes connecting to a vertex - (1,1,1), (1,1,-1),
   * (1,-1,1), (1,-1,1), (-1,1,1), (-1,1,-1), (-1,-1,1), (-1,-1-1).
   */
  void AddCubeVertexPlanes();

  /**
   * Add the 12 planes that represent the edges of a cube - halfway between
   * the two connecting face planes - (1,1,0), (-1,-1,0), (-1,1,0), (1,-1,0),
   * (0,1,1), (0,-1,-1), (0,1,-1), (0,-1,1), (1,0,1), (-1,0,-1),
   * (1,0,-1), (-1,0,1)
   */
  void AddCubeEdgePlanes();

  /**
   * Add the six planes that make up the faces of a cube - (1,0,0),
   * (-1, 0, 0), (0,1,0), (0,-1,0), (0,0,1), (0,0,-1)
   */
  void AddCubeFacePlanes();

  /**
   * Add the planes that represent the normals of the vertices of a
   * polygonal sphere formed by recursively subdividing the triangles
   * in an octahedron.  Each triangle is subdivided by connecting the
   * midpoints of the edges thus forming 4 smaller triangles. The
   * level indicates how many subdivisions to do with a level of 0
   * used to add the 6 planes from the original octahedron, level 1
   * will add 18 planes, and so on.
   */
  void AddRecursiveSpherePlanes( int level );

  //@{
  /**
   * A special method that is used to generate a polyhedron directly
   * from a set of n planes. The planes that are supplied by the user
   * are not squeezed towards the input data (in fact the user need
   * not specify an input). To use this method, you must provide an
   * instance of vtkPolyData into which the points and cells defining
   * the polyhedron are placed. You must also provide a bounding box
   * where you expect the resulting polyhedron to lie. This can be
   * a very generous fit, it's only used to create the initial polygons
   * that are eventually clipped.
   */
  void GenerateHull(vtkPolyData *pd, double *bounds);
  void GenerateHull(vtkPolyData *pd, double xmin, double xmax,
                    double ymin, double ymax, double zmin, double zmax);
  //@}

protected:
  vtkHull();
  ~vtkHull() VTK_OVERRIDE;

  // The planes - 4 doubles per plane for A, B, C, D
  double     *Planes;

  // This indicates the current size (in planes - 4*sizeof(double)) of
  // the this->Planes array. Planes are allocated in chunks so that the
  // array does not need to be reallocated every time a new plane is added
  int       PlanesStorageSize;

  // The number of planes that have been added
  int       NumberOfPlanes;

  // Internal method used to find the position of each plane
  void      ComputePlaneDistances(vtkPolyData *input);

  // Internal method used to create the actual polygons from the set
  // of planes
  void      ClipPolygonsFromPlanes( vtkPoints *points, vtkCellArray *polys,
                                    const double *bounds );

  // Internal method used to create the initial "big" polygon from the
  // plane equation. This polygon is clipped by all other planes to form
  // the final polygon (or it may be clipped entirely)
  void      CreateInitialPolygon( double *, int, const double * );

  // The method that does it all...
  int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *) VTK_OVERRIDE;
private:
  vtkHull(const vtkHull&) VTK_DELETE_FUNCTION;
  void operator=(const vtkHull&) VTK_DELETE_FUNCTION;
};

#endif
