/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkLineSource.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkLineSource
 * @brief   create a line defined by two end points
 *
 * vtkLineSource is a source object that creates a polyline defined by
 * two endpoints or a collection of connected line segments. To define the line
 * by end points, use `SetPoint1` and `SetPoint2` methods. To define a broken
 * line comprising of multiple line segments, use `SetPoints` to provide the
 * corner points that for the line.
 *
 * Intermediate points within line segment (when specifying end points alone) or
 * each of the individual line segments (when specifying broken line) can be
 * specified in two ways. First, when `UseRegularRefinement` is true (default),
 * the `Resolution` is used to determine how many intermediate points to add
 * using regular refinement. Thus, if `Resolution` is set to 1, a mid point will
 * be added for each of the line segments resulting in a line with 3 points: the
 * two end points and the mid point. Second, when `UseRegularRefinement` is
 * false, refinement ratios for points per segment are specified using
 * `SetRefinementRatio` and `SetNumberOfRefinementRatios`. To generate same
 * points as `Resolution` set to 1, the refinement ratios will be `[0, 0.5,
 * 1.0]`. To add the end points of the line segment `0.0` and `1.0` must be
 * included in the collection of refinement ratios.
 *
 * @section ChangesVTK9 Changes in VTK 9.0
 *
 * Prior to VTK 9.0, when broken line was being generated, the texture
 * coordinates for each of the individual breaks in the line ranged from [0.0,
 * 1.0]. This has been changed to generate texture coordinates in the range
 * [0.0, 1.0] over the entire output line irrespective of whether the line was
 * generated by simply specifying the end points or multiple line segments.
 *
 * @par Thanks:
 * This class was extended by Philippe Pebay, Kitware SAS 2011, to support
 * broken lines as well as simple lines.
 */

#ifndef vtkLineSource_h
#define vtkLineSource_h

#include "vtkFiltersSourcesModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

#include <vector> //  for std::vector
class vtkPoints;

class VTKFILTERSSOURCES_EXPORT vtkLineSource : public vtkPolyDataAlgorithm
{
public:
  static vtkLineSource* New();
  vtkTypeMacro(vtkLineSource, vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set position of first end point.
   */
  vtkSetVector3Macro(Point1, double);
  vtkGetVectorMacro(Point1, double, 3);
  void SetPoint1(float[3]);
  //@}

  //@{
  /**
   * Set position of other end point.
   */
  vtkSetVector3Macro(Point2, double);
  vtkGetVectorMacro(Point2, double, 3);
  void SetPoint2(float[3]);
  //@}

  //@{
  /**
   * Set/Get how the line segment is to be refined. One can choose to add points
   * at regular intervals per segment (defined using `Resolution`) or explicit
   * locations (defined using `SetRefinementRatio`). Default is true i.e
   * `Resolution` will be used to determine placement of points within each line
   * segment.
   */
  vtkSetMacro(UseRegularRefinement, bool);
  vtkGetMacro(UseRegularRefinement, bool);
  vtkBooleanMacro(UseRegularRefinement, bool);
  //@}

  //@{
  /**
   * Divide line into Resolution number of pieces. This is used when
   * `UseRegularRefinement` is true.
   */
  vtkSetClampMacro(Resolution, int, 1, VTK_INT_MAX);
  vtkGetMacro(Resolution, int);
  //@}

  //@{
  /**
   * API for setting/getting refinement ratios for points added to the line
   * segment. The ratio is in the range `[0.0, 1.0]` where 0.0 is the start of
   * the line segment and 1.0 is the end. When generating broken lines i.e.
   * using `SetPoints`, this specifies refinement points for each of the
   * individual line segment. Note that `0.0` and `1.0` must be explicitly
   * included to generate a point and the start and/or end of the line segment.
   * This is used only when `UseRegularRefinement` is false.
   */
  void SetNumberOfRefinementRatios(int);
  void SetRefinementRatio(int index, double value);
  int GetNumberOfRefinementRatios();
  double GetRefinementRatio(int index);
  //@}

  //@{
  /**
   * Set/Get the list of points defining a broken line
   */
  virtual void SetPoints(vtkPoints*);
  vtkGetObjectMacro(Points, vtkPoints);
  //@}

  //@{
  /**
   * Set/get the desired precision for the output points.
   * vtkAlgorithm::SINGLE_PRECISION - Output single-precision floating point.
   * vtkAlgorithm::DOUBLE_PRECISION - Output double-precision floating point.
   */
  vtkSetMacro(OutputPointsPrecision, int);
  vtkGetMacro(OutputPointsPrecision, int);
  //@}

protected:
  vtkLineSource(int res = 1);
  ~vtkLineSource() override;

  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  double Point1[3];
  double Point2[3];
  int Resolution;
  int OutputPointsPrecision;
  bool UseRegularRefinement;
  std::vector<double> RefinementRatios;

  /**
   * The list of points defining a broken line
   * NB: The Point1/Point2 definition of a single line segment is used by default
   */
  vtkPoints* Points;

private:
  vtkLineSource(const vtkLineSource&) = delete;
  void operator=(const vtkLineSource&) = delete;
};

#endif
