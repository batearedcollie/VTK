/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkHyperTreeGridGeometryEntry.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkHyperTreeGridGeometryEntry
JB
 * @brief   GeometryEntry is a cache data for cursors requiring coordinates
 *
 * cf. vtkHyperTreeGridEntry
 *
 * @sa
 * vtkHyperTreeGridEntry
 * vtkHyperTreeGridLevelEntry
 * vtkHyperTreeGridGeometryEntry
 * vtkHyperTreeGridGeometryLevelEntry
 * vtkHyperTreeGridNonOrientedGeometryCursor
 * vtkHyperTreeGridNonOrientedSuperCursor
 * vtkHyperTreeGridNonOrientedSuperCursorLight
 *
 * @par Thanks:
 * This class was written by Jacques-Bernard Lekien, Jerome Dubois and
 * Guenole Harel, CEA 2018.
 * This work was supported by Commissariat a l'Energie Atomique
 * CEA, DAM, DIF, F-91297 Arpajon, France.
 */

#ifndef vtkHyperTreeGridGeometryEntry_h
#define vtkHyperTreeGridGeometryEntry_h

#ifndef __VTK_WRAP__

#include "vtkObject.h"

class vtkHyperTree;
class vtkHyperTreeGrid;

class vtkHyperTreeGridGeometryEntry
{
public:
  /**
   * Display info about the entry
   */
  void PrintSelf( ostream& os, vtkIndent indent );

  /**
   * Constructor
   */
  vtkHyperTreeGridGeometryEntry() {
    this->Index = 0;
    for (unsigned int d = 0; d < 3; ++ d )
    {
      this->Origin[ d ] = 0.;
    }
  }

  /**
   * Constructor
   */
  vtkHyperTreeGridGeometryEntry(
    vtkIdType index,
    const double* origin
  ) {
    this->Index = index;
    for (unsigned int d = 0; d < 3; ++ d )
    {
      this->Origin[ d ] = origin[ d ];
    }
  }

  /**
   * Destructor
   */
  ~vtkHyperTreeGridGeometryEntry() = default;

  /**
   * Dump information
   */
  void Dump( ostream& os );

  /**
   * Initialize cursor at root of given tree index in grid.
   */
  vtkHyperTree* Initialize(
    vtkHyperTreeGrid* grid,
    vtkIdType treeIndex,
    bool create = false
  );

  /**
   * Initialize cursor from explicit required data
   */
  void Initialize(
    vtkIdType index,
    const double* origin
  ) {
    this->Index = index;
    for (unsigned int d = 0; d < 3; ++ d )
    {
      this->Origin[ d ] = origin[ d ];
    }
  }

  /**
   * Copy function
   */
  void Copy( const vtkHyperTreeGridGeometryEntry* entry) {
    this->Index = entry->Index;
    for (unsigned int d = 0; d < 3; ++ d )
    {
      this->Origin[d] = entry->Origin[d];
    }
  }

  /**
   * Return the index of the current vertex in the tree.
   */
  vtkIdType GetVertexId() const {
    return this->Index;
  }

  /**
   * Return the global index (relative to the grid) of the
   * current vertex in the tree.
   */
  vtkIdType GetGlobalNodeIndex( const vtkHyperTree* tree ) const;

  /**
   * Set the global index for the root cell of the HyperTree.
   */
  void SetGlobalIndexStart( vtkHyperTree* tree, vtkIdType index );

  /**
   * Set the global index for the current cell of the HyperTree.
   */
  void SetGlobalIndexFromLocal( vtkHyperTree* tree, vtkIdType index );

  /**
   * Is the cursor pointing to a leaf?
   */
  bool IsLeaf( const vtkHyperTree* tree ) const;

  /**
   * Change the current cell's status: if leaf then becomes coarse and
   * all its children are created, cf. HyperTree.
   */
  void SubdivideLeaf( vtkHyperTree* tree, unsigned int level );

  /**
   * Is the cursor pointing to a coarse with all childrens leaves ?
   */
  bool IsTerminalNode( const vtkHyperTree* tree ) const;

  /**
   * Is the cursor at tree root?
   */
  bool IsRoot() const {
    return ( this->Index == 0 );
  }

  /**
   * Move the cursor to child `child' of the current vertex.
   * \pre not_leaf: !IsLeaf()
   * \pre valid_child: ichild>=0 && ichild<this->GetNumberOfChildren()
   */
  void ToChild(
    const vtkHyperTreeGrid* grid,
    const vtkHyperTree* tree,
    const double* sizeChild,
    unsigned char ichild
  );

  /**
   * Getter for origin coordinates of the current cell.
   */
  const double* GetOrigin() const {
    return this->Origin;
  }

  /**
   * Getter for bounding box of the current cell.
   */
  void GetBounds(
    const double* sizeChild,
    double bounds[6]
  ) const {
    // Compute bounds
    bounds[0] = this->Origin[0];
    bounds[1] = this->Origin[0] + sizeChild[0];
    bounds[2] = this->Origin[1];
    bounds[3] = this->Origin[1] + sizeChild[1];
    bounds[4] = this->Origin[2];
    bounds[5] = this->Origin[2] + sizeChild[2];
  }

  /**
   * Getter for center of the current cell.
   */
  void GetPoint(
    const double* sizeChild,
    double point[3]
  ) const {
    // Compute center point coordinates
    point[0] = this->Origin[0] + sizeChild[0] / 2.;
    point[1] = this->Origin[1] + sizeChild[1] / 2.;
    point[2] = this->Origin[2] + sizeChild[2] / 2.;
  }

private:

  /**
   * index of the current cell in the HyperTree.
   */
  vtkIdType Index;

  /**
   * origin coiordinates of the current cell
   */
  double Origin[3];
};

#endif// __VTK_WRAP__


#endif // vtkHyperTreeGridGeometryEntry_h
// VTK-HeaderTest-Exclude: vtkHyperTreeGridGeometryEntry.h
