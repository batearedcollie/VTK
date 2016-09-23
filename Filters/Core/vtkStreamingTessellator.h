/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStreamingTessellator.h
  Language:  C++

  Copyright 2003 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
  license for use of this work by or on behalf of the
  U.S. Government. Redistribution and use in source and binary forms, with
  or without modification, are permitted provided that this Notice and any
  statement of authorship are reproduced on all copies.

=========================================================================*/
/**
 * @class   vtkStreamingTessellator
 * @brief   An algorithm that refines an initial simplicial tessellation using edge subdivision
 *
 * This class is a simple algorithm that takes a single starting simplex -- a
 * tetrahedron, triangle, or line segment -- and calls a function you
 * pass it with (possibly many times) tetrahedra, triangles, or lines
 * adaptively sampled from the one you specified. It
 * uses an algorithm you specify to control the level of adaptivity.
 *
 * This class does not create vtkUnstructuredGrid output because it is
 * intended for use in mappers as well as filters. Instead, it
 * calls the registered function with simplices as they are
 * created.
 *
 * The subdivision algorithm should change the vertex
 * coordinates (it must change both geometric and, if desired, parametric
 * coordinates) of the midpoint. These coordinates need not be
 * changed unless the EvaluateEdge() member returns true.
 * The vtkStreamingTessellator itself has no way of creating
 * a more accurate midpoint vertex.
 *
 * Here's how to use this class:
 * - Call AdaptivelySample1Facet, AdaptivelySample2Facet, or
 *   AdaptivelySample3Facet, with an edge, triangle, or
 *   tetrahedron you want tessellated.
 * - The adaptive tessellator classifies each edge by passing
 *   the midpoint values to the vtkEdgeSubdivisionCriterion.
 * - After each edge is classified, the tessellator subdivides
 *   edges as required until the subdivision criterion is
 *   satisfied or the maximum subdivision depth has been
 *   reached.
 * - Edges, triangles, or tetrahedra connecting the vertices
 *   generated by the subdivision algorithm are processed by
 *   calling the user-defined callback functions (set with
 *   SetTetrahedronCallback(), SetTriangleCallback(),
 *   or SetEdgeCallback() ).
 *
 * @warning
 * Note that the vertices passed to AdaptivelySample3Facet, AdaptivelySample2Facet,
 * or AdaptivelySample1Facet must be at least 6, 5, or 4 entries long, respectively!
 * This is because the &lt;r,s,t&gt;, &lt;r,s&gt;, or &lt;r&gt;
 * parametric coordinates of the vertices are maintained as the
 * facet is subdivided. This information is often
 * required by the subdivision algorithm in order to compute
 * an error metric. You may change the number of parametric coordinates
 * associated with each vertex using vtkStreamingTessellator::SetEmbeddingDimension().
 *
 * @par Interpolating Field Values:
 * If you wish, you may also use \p vtkStreamingTessellator to interpolate field
 * values at newly created vertices. Interpolated field values are stored just beyond
 * the parametric coordinates associated with a vertex. They will always be \p double
 * values; it does not make sense to interpolate a boolean or string value and your
 * output and subdivision subroutines may always cast to a \p float or use \p floor() to
 * truncate an interpolated value to an integer.
 *
 * @sa
 * vtkEdgeSubdivisionCriterion
*/

#ifndef vtkStreamingTessellator_h
#define vtkStreamingTessellator_h

#include "vtkFiltersCoreModule.h" // For export macro
#include "vtkObject.h"

#undef PARAVIEW_DEBUG_TESSELLATOR

class vtkEdgeSubdivisionCriterion;

class VTKFILTERSCORE_EXPORT vtkStreamingTessellator : public vtkObject
{
  public:
    vtkTypeMacro(vtkStreamingTessellator,vtkObject);
    static vtkStreamingTessellator* New();
    virtual void PrintSelf( ostream& os, vtkIndent indent );

    typedef void (*VertexProcessorFunction)( const double*, vtkEdgeSubdivisionCriterion*, void*, const void* );
    typedef void (*EdgeProcessorFunction)( const double*, const double*, vtkEdgeSubdivisionCriterion*, void*, const void* );
    typedef void (*TriangleProcessorFunction)( const double*, const double*, const double*, vtkEdgeSubdivisionCriterion*, void*, const void* );
    typedef void (*TetrahedronProcessorFunction)( const double*, const double*, const double*, const double*, vtkEdgeSubdivisionCriterion*, void*, const void* );

    enum {MaxFieldSize = 18};

    //@{
    /**
     * Get/Set the function called for each output tetrahedron (3-facet).
     */
    virtual void SetTetrahedronCallback( TetrahedronProcessorFunction );
    virtual TetrahedronProcessorFunction GetTetrahedronCallback() const;
    //@}

    //@{
    /**
     * Get/Set the function called for each output triangle (2-facet).
     */
    virtual void SetTriangleCallback( TriangleProcessorFunction );
    virtual TriangleProcessorFunction GetTriangleCallback() const;
    //@}

    //@{
    /**
     * Get/Set the function called for each output line segment (1-facet).
     */
    virtual void SetEdgeCallback( EdgeProcessorFunction );
    virtual EdgeProcessorFunction GetEdgeCallback() const;
    //@}

    //@{
    /**
     * Get/Set the function called for each output line segment (1-facet).
     */
    virtual void SetVertexCallback( VertexProcessorFunction );
    virtual VertexProcessorFunction GetVertexCallback() const;
    //@}

    //@{
    /**
     * Get/Set a void pointer passed to the triangle and edge output functions.
     */
    virtual void SetPrivateData( void* Private );
    virtual void* GetPrivateData() const;
    //@}

    // can't wrap const private data because python wrapper will try to cast it to void*, not const void*

    //@{
    /**
     * Get/Set a constant void pointer passed to the simplex output functions.
     */
    virtual void SetConstPrivateData( const void* ConstPrivate );
    virtual const void* GetConstPrivateData() const;
    //@}

    //@{
    /**
     * Get/Set the algorithm used to determine whether an edge should be
     * subdivided or left as-is. This is used once for each call to
     * AdaptivelySample1Facet (which is recursive and will call itself
     * resulting in additional edges to be checked) or three times for
     * each call to AdaptivelySample2Facet (also recursive).
     */
    virtual void SetSubdivisionAlgorithm( vtkEdgeSubdivisionCriterion* );
    virtual vtkEdgeSubdivisionCriterion* GetSubdivisionAlgorithm() ;
    //@}

    virtual const vtkEdgeSubdivisionCriterion* GetSubdivisionAlgorithm() const;

    //@{
    /**
     * Get/Set the number of parameter-space coordinates associated with each input and output point.
     * The default is \a k for \a k -facets. You may
     * specify a different dimension, \a d, for each type of \a k -facet to be processed.
     * For example, \p SetEmbeddingDimension( \p 2, \p 3 ) would associate \a r, \a s, and
     * \a t coordinates with each input and output point generated by \p AdaptivelySample2Facet
     * but does not say anything about input or output points generated by
     * \p AdaptivelySample1Facet.
     * Call \p SetEmbeddingDimension( \p -1, \a d ) to specify the same dimension for
     * all possible \a k values.
     * \a d may not exceed 8, as that would be plain silly.
     */
    virtual void SetEmbeddingDimension( int k, int d );
    int GetEmbeddingDimension( int k ) const;
    //@}

    //@{
    /**
     * Get/Set the number of field value coordinates associated with each input and output point.
     * The default is 0; no field values are interpolated.
     * You may specify a different size, \a s, for each type of \a k -facet to be processed.
     * For example, \p SetFieldSize( \p 2, \p 3 ) would associate 3 field value coordinates
     * with each input and output point of an \p AdaptivelySample2Facet call,
     * but does not say anything about input or output points of \p AdaptivelySample1Facet.
     * Call \p SetFieldSize( \p -1, \a s ) to specify the same dimension for all possible \a k values.
     * \a s may not exceed vtkStreamingTessellator::MaxFieldSize.
     * This is a compile-time constant that defaults to 18, which is large enough for
     * a scalar, vector, tensor, normal, and texture coordinate to be included at each point.

     * Normally, you will not call \a SetFieldSize() directly; instead, subclasses of
     * vtkEdgeSubdivisionCriterion, such as vtkShoeMeshSubdivisionAlgorithm, will call it
     * for you.

     * In any event, setting \a FieldSize to a non-zero value means you must pass field
     * values to the \p AdaptivelySamplekFacet routines; For example,
     * @verbatim
     * vtkStreamingTessellator* t = vtkStreamingTessellator::New();
     * t->SetFieldSize( 1, 3 );
     * t->SetEmbeddingDimension( 1, 1 ); // not really required, this is the default
     * double p0[3+1+3] = { x0, y0, z0, r0, fx0, fy0, fz0 };
     * double p1[3+1+3] = { x1, y1, z1, r1, fx1, fy1, fz1 };
     * t->AdaptivelySample1Facet( p0, p1 );
     * @endverbatim
     * This would adaptively sample an curve (1-facet) with geometry and
     * a vector field at every output point on the curve.
     */
    virtual void SetFieldSize( int k, int s );
    int GetFieldSize( int k ) const;
    //@}

    //@{
    /**
     * Get/Set the maximum number of subdivisions that may occur.
     */
    virtual void SetMaximumNumberOfSubdivisions( int num_subdiv_in );
    int GetMaximumNumberOfSubdivisions();
    //@}

    //@{
    /**
     * This will adaptively subdivide the tetrahedron (3-facet),
     * triangle (2-facet), or edge (1-facet) until the subdivision
     * algorithm returns false for every edge or the maximum recursion
     * depth is reached.

     * Use \p SetMaximumNumberOfSubdivisions to change the maximum
     * recursion depth.

     * The AdaptivelySample0Facet method is provided as a convenience.
     * Obviously, there is no way to adaptively subdivide a vertex.
     * Instead the input vertex is passed unchanged to the output
     * via a call to the registered VertexProcessorFunction callback.

     * .SECTION Warning
     * This assumes that you have called SetSubdivisionAlgorithm(),
     * SetEdgeCallback(), SetTriangleCallback(), and SetTetrahedronCallback()
     * with valid values!
     */
    void AdaptivelySample3Facet( double* v1, double* v2, double* v3, double* v4 ) const ;
    void AdaptivelySample2Facet( double* v1, double* v2, double* v3 ) const ;
    void AdaptivelySample1Facet( double* v1, double* v2 ) const ;
    void AdaptivelySample0Facet( double* v1 ) const ;
    //@}

    //@{
    /**
     * Reset/access the histogram of subdivision cases encountered.
     * The histogram may be used to examine coverage during testing as well as characterizing the
     * tessellation algorithm's performance.
     * You should call ResetCounts() once, at the beginning of a stream of tetrahedra.
     * It must be called before AdaptivelySample3Facet() to prevent uninitialized memory reads.

     * These functions have no effect (and return 0) when PARAVIEW_DEBUG_TESSELLATOR has not been defined.
     * By default, PARAVIEW_DEBUG_TESSELLATOR is not defined, and your code will be fast and efficient. Really!
     */
    void ResetCounts()
    {
#ifdef PARAVIEW_DEBUG_TESSELLATOR
      for ( int i=0; i<11; ++i )
        {
        this->CaseCounts[i] = 0;
        for ( int j=0; j<51; ++j )
          {
          this->SubcaseCounts[i][j] = 0;
          }
        }
#endif // PARAVIEW_DEBUG_TESSELLATOR
    }
    vtkIdType GetCaseCount( int c )
      {
#ifdef PARAVIEW_DEBUG_TESSELLATOR
      return this->CaseCounts[c];
#else
      (void)c;
      return 0;
#endif // PARAVIEW_DEBUG_TESSELLATOR
      }
    vtkIdType GetSubcaseCount( int casenum, int sub )
      {
#ifdef PARAVIEW_DEBUG_TESSELLATOR
      return this->SubcaseCounts[casenum][sub];
#else
      (void)casenum;
      (void)sub;
      return 0;
#endif // PARAVIEW_DEBUG_TESSELLATOR
      }
    //@}

  protected:

    static int EdgeCodesToCaseCodesPlusPermutation[64][2];
    static vtkIdType PermutationsFromIndex[24][14];
    static vtkIdType TetrahedralDecompositions[];

    void* PrivateData;
    const void* ConstPrivateData;
    vtkEdgeSubdivisionCriterion* Algorithm;

    VertexProcessorFunction Callback0;
    EdgeProcessorFunction Callback1;
    TriangleProcessorFunction Callback2;
    TetrahedronProcessorFunction Callback3;
#ifdef PARAVIEW_DEBUG_TESSELLATOR
    mutable vtkIdType CaseCounts[11];
    mutable vtkIdType SubcaseCounts[11][51];
#endif // PARAVIEW_DEBUG_TESSELLATOR

    /**
     * PointDimension is the length of each \p double* array associated with
     * each point passed to a subdivision algorithm:
     * PointDimension[i] = 3 + EmbeddingDimension[i] + FieldSize[i]
     * We store this instead of FieldSize for speed.
     * Only entries 1 through 3 are used; you can't subdivide 0-facets (points).
     * Well, maybe <i>you</i> can, but <i>I</i> can't!
     */
    int PointDimension[4];

    /**
     * The parametric dimension of each point passed to the subdivision algorithm.
     * Only entries 1 through 3 are used; you can't subdivide 0-facets (points).
     * Well, maybe <i>you</i> can, but <i>I</i> can't!
     */
    int EmbeddingDimension[4];

    /**
     * The number of subdivisions allowed.
     */
    int MaximumNumberOfSubdivisions;

    vtkStreamingTessellator();
    ~vtkStreamingTessellator();

    void AdaptivelySample3Facet( double* v1, double* v2, double* v3, double* v4, int maxDepth ) const ;
    void AdaptivelySample2Facet( double* v1, double* v2, double* v3, int maxDepth, int move=7 ) const ;
    void AdaptivelySample1Facet( double* v1, double* v2, int maxDepth ) const ;

    int BestTets( int*, double**, int, int ) const;

  private:
    vtkStreamingTessellator( const vtkStreamingTessellator& ) VTK_DELETE_FUNCTION;
    void operator = ( const vtkStreamingTessellator& ) VTK_DELETE_FUNCTION;
};

inline void vtkStreamingTessellator::AdaptivelySample3Facet( double* v1, double* v2, double* v3, double* v4 ) const
{ this->AdaptivelySample3Facet( v1, v2, v3, v4, this->MaximumNumberOfSubdivisions ); }
inline void vtkStreamingTessellator::AdaptivelySample2Facet( double* v1, double* v2, double* v3 ) const
{ this->AdaptivelySample2Facet( v1, v2, v3, this->MaximumNumberOfSubdivisions ); }
inline void vtkStreamingTessellator::AdaptivelySample1Facet( double* v1, double* v2 ) const
{ this->AdaptivelySample1Facet( v1, v2, this->MaximumNumberOfSubdivisions ); }

inline int vtkStreamingTessellator::GetEmbeddingDimension( int k ) const
{ if ( k <= 0 || k >= 4 ) return -1; return this->EmbeddingDimension[k]; }

inline int vtkStreamingTessellator::GetFieldSize( int k ) const
{ if ( k <= 0 || k >= 4 ) return -1; return this->PointDimension[k] - this->EmbeddingDimension[k] - 3; }

inline int vtkStreamingTessellator::GetMaximumNumberOfSubdivisions() {return this->MaximumNumberOfSubdivisions;}

#endif // vtkStreamingTessellator_h
