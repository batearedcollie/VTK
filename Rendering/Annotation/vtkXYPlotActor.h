/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkXYPlotActor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkXYPlotActor
 * @brief   generate an x-y plot from input dataset(s) or field data
 *
 * vtkXYPlotActor creates an x-y plot of data from one or more input data
 * sets or field data. The class plots dataset scalar values (y-axis) against
 * the points (x-axis). The x-axis values are generated by taking the point
 * ids, computing a cumulative arc length, or a normalized arc length. More
 * than one input data set can be specified to generate multiple plots.
 * Alternatively, if field data is supplied as input, the class plots one
 * component against another. (The user must specify which component to use
 * as the x-axis and which for the y-axis.)
 *
 * To use this class to plot dataset(s), you must specify one or more
 * input datasets containing scalar and point data.  You'll probably also
 * want to invoke a method to control how the point coordinates are converted
 * into x values (by default point ids are used).
 *
 * To use this class to plot field data, you must specify one or more input
 * data objects with its associated field data. You'll also want to specify
 * which component to use as the x-axis and which to use as the y-axis.
 * Note that when plotting field data, the x and y values are used directly
 * (i.e., there are no options to normalize the components).
 *
 * Once you've set up the plot, you'll want to position it.  The
 * PositionCoordinate defines the lower-left location of the x-y plot
 * (specified in normalized viewport coordinates) and the Position2Coordinate
 * define the upper-right corner. (Note: the Position2Coordinate is relative
 * to PositionCoordinate, so you can move the vtkXYPlotActor around the
 * viewport by setting just the PositionCoordinate.) The combination of the
 * two position coordinates specifies a rectangle in which the plot will lie.
 *
 * Optional features include the ability to specify axes labels, label
 * format and plot title. You can also
 * manually specify the x and y plot ranges (by default they are computed
 * automatically). The Border instance variable is used to create space
 * between the boundary of the plot window (specified by PositionCoordinate
 * and Position2Coordinate) and the plot itself.
 *
 * The font property of the plot title can be modified through the
 * TitleTextProperty attribute.
 * The font property of the axes titles and labels can be modified through the
 * AxisTitleTextProperty and AxisLabelTextProperty attributes. You may also
 * use the GetXAxisActor2D or GetYAxisActor2D methods
 * to access each individual axis actor to modify their font properties.
 * In the same way, the GetLegendBoxActor method can be used to access
 * the legend box actor to modify its font properties.
 *
 * There are several advanced features as well. You can assign per curve
 * properties (such as color and a plot symbol). (Note that each input
 * dataset and/or data object creates a single curve.) Another option is to
 * add a plot legend that graphically indicates the correspondance between
 * the curve, curve symbols, and the data source. You can also exchange the
 * x and y axes if you prefer you plot orientation that way.
 *
 * @warning
 * If you are interested in plotting something other than scalar data, you
 * can use the vtk data shuffling filters (e.g.,
 * vtkAttributeDataToFieldDataFilter and vtkFieldDataToAttributeDataFilter)
 * to convert the data into scalar data and/or points.
 *
 * @par Thanks:
 * This class was written by:
 * Will Schroeder, Jim Miller, Charles Law, Sebastien Barre, Amy Squillacote,
 * Ken Martin, Mathieu Malaterre, Jeff Lee, Francois Finet, Julien Bertel,
 * Claire Guilbaud, and Philippe Pebay
 *
 * @sa
 * vtkActor2D vtkTextMapper vtkScalarBarActor vtkAxisActor2D vtkCubeAxesActor2D
 * vtkAttributeDataToFieldDataFilter vtkFieldDataToAttributeDataFilter
 * vtkTextProperty
*/

#ifndef vtkXYPlotActor_h
#define vtkXYPlotActor_h

#define VTK_XYPLOT_INDEX                 0
#define VTK_XYPLOT_ARC_LENGTH            1
#define VTK_XYPLOT_NORMALIZED_ARC_LENGTH 2
#define VTK_XYPLOT_VALUE                 3

#define VTK_XYPLOT_ROW 0
#define VTK_XYPLOT_COLUMN 1

#define VTK_XYPLOT_Y_AXIS_TOP     0
#define VTK_XYPLOT_Y_AXIS_HCENTER 1
#define VTK_XYPLOT_Y_AXIS_VCENTER 2 // rotate by 90 degrees (y-axis aligned)

#include "vtkRenderingAnnotationModule.h" // For export macro
#include "vtkActor2D.h"
#include "vtkSmartPointer.h" // For SP

class vtkXYPlotActorConnections;
class vtkAlgorithmOutput;
class vtkAppendPolyData;
class vtkAxisActor2D;
class vtkDataObject;
class vtkDataObjectCollection;
class vtkDataSet;
class vtkDataSetCollection;
class vtkDoubleArray;
class vtkGlyph2D;
class vtkGlyphSource2D;
class vtkIntArray;
class vtkLegendBoxActor;
class vtkPlanes;
class vtkPolyData;
class vtkPolyDataMapper2D;
class vtkTextActor;
class vtkTextMapper;
class vtkTextProperty;

class VTKRENDERINGANNOTATION_EXPORT vtkXYPlotActor : public vtkActor2D
{
public:
  vtkTypeMacro(vtkXYPlotActor,vtkActor2D);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Instantiate object with autorange computation; bold, italic, and shadows
   * on; arial font family; the number of labels set to 5 for the x and y
   * axes; a label format of "%-#6.3g"; and x coordinates computed from point
   * ids.
   */
  static vtkXYPlotActor *New();

  //---Data Set Input----------------------------------------------------------
  // The following methods are used to plot input datasets. Datasets
  // will be plotted if set as input; otherwise the input data objects
  // will be plotted (if defined).

  //@{
  /**
   * Add a dataset to the list of data to append. The array name specifies
   * which point array to plot. The array must be a vtkDataArray subclass, i.e.
   * a numeric array. If the array name is NULL, then the default
   * scalars are used.  The array can have multiple components, but only the
   * first component is ploted. Note that AddInputDataSet() does not setup
   * a pipeline connection whereas AddInputConnection() does.
   */
  void AddDataSetInput(vtkDataSet *ds, const char* arrayName, int component);
  void AddDataSetInput(vtkDataSet *ds) {this->AddDataSetInput(ds, nullptr, 0);}
  void AddDataSetInputConnection(vtkAlgorithmOutput *in, const char* arrayName, int component);
  void AddDataSetInputConnection(vtkAlgorithmOutput *in) {this->AddDataSetInputConnection(in, nullptr, 0);}
  //@}

  //@{
  /**
   * Remove a dataset from the list of data to append.
   */
  void RemoveDataSetInput(vtkDataSet *ds, const char* arrayName, int component);
  void RemoveDataSetInput(vtkDataSet *ds) {this->RemoveDataSetInput(ds, nullptr, 0);}
  void RemoveDataSetInputConnection(vtkAlgorithmOutput *in, const char* arrayName, int component);
  void RemoveDataSetInputConnection(vtkAlgorithmOutput *in)
  {
    this->RemoveDataSetInputConnection(in, nullptr, 0);
  }
  //@}

  /**
   * This removes all of the data set inputs,
   * but does not change the data object inputs.
   */
  void RemoveAllDataSetInputConnections();

  //@{
  /**
   * If plotting points by value, which component to use to determine the
   * value. This sets a value per each input dataset (i.e., the ith dataset).
   */
  void SetPointComponent(int i, int comp);
  int GetPointComponent(int i);
  //---end Data Set Input-----------------------------------------------------
  //@}

  //@{
  /**
   * Specify how the independent (x) variable is computed from the points.
   * The independent variable can be the scalar/point index (i.e., point id),
   * the accumulated arc length along the points, the normalized arc length,
   * or by component value. If plotting datasets (e.g., points), the value
   * that is used is specified by the PointComponent ivar.  (Note: these
   * methods also control how field data is plotted. Field data is usually
   * plotted by value or index, if plotting length 1-dimensional length
   * measures are used.)
   */
  vtkSetClampMacro(XValues,int,VTK_XYPLOT_INDEX,VTK_XYPLOT_VALUE);
  vtkGetMacro(XValues,int);
  void SetXValuesToIndex(){this->SetXValues(VTK_XYPLOT_INDEX);};
  void SetXValuesToArcLength() {this->SetXValues(VTK_XYPLOT_ARC_LENGTH);};
  void SetXValuesToNormalizedArcLength()
    {this->SetXValues(VTK_XYPLOT_NORMALIZED_ARC_LENGTH);};
  void SetXValuesToValue() {this->SetXValues(VTK_XYPLOT_VALUE);};
  const char *GetXValuesAsString();
  //@}

  //---Data Object Input------------------------------------------------------
  // The following methods are used to plot input data objects. Datasets will
  // be plotted in preference to data objects if set as input; otherwise the
  // input data objects will be plotted (if defined).

  //@{
  /**
   * Add a data object to the list of data to display.
   */
  void AddDataObjectInput(vtkDataObject *in);
  void AddDataObjectInputConnection(vtkAlgorithmOutput *alg);
  //@}

  //@{
  /**
   * Remove a dataset from the list of data to display.
   */
  void RemoveDataObjectInputConnection(vtkAlgorithmOutput *aout);
  void RemoveDataObjectInput(vtkDataObject *in);
  //@}

  //@{
  /**
   * Indicate whether to plot rows or columns. If plotting rows, then
   * the dependent variables is taken from a specified row,
   * versus rows (y).
   */
  vtkSetClampMacro(DataObjectPlotMode,int,VTK_XYPLOT_ROW,VTK_XYPLOT_COLUMN);
  vtkGetMacro(DataObjectPlotMode,int);
  void SetDataObjectPlotModeToRows()
    {this->SetDataObjectPlotMode(VTK_XYPLOT_ROW);}
  void SetDataObjectPlotModeToColumns()
    {this->SetDataObjectPlotMode(VTK_XYPLOT_COLUMN);}
  const char *GetDataObjectPlotModeAsString();
  //@}

  //@{
  /**
   * Specify which component of the input data object to use as the
   * independent variable for the ith input data object. (This ivar is
   * ignored if plotting the index.) Note that the value is interpreted
   * differently depending on DataObjectPlotMode. If the mode is Rows, then
   * the value of DataObjectXComponent is the row number; otherwise it's the
   * column number.
   */
  void SetDataObjectXComponent(int i, int comp);
  int GetDataObjectXComponent(int i);
  //@}

  //@{
  /**
   * Specify which component of the input data object to use as the
   * dependent variable for the ith input data object. (This ivar is
   * ignored if plotting the index.) Note that the value is interpreted
   * differently depending on DataObjectPlotMode. If the mode is Rows, then
   * the value of DataObjectYComponent is the row number; otherwise it's the
   * column number.
   */
  void SetDataObjectYComponent(int i, int comp);
  int GetDataObjectYComponent(int i);
  //---end Data Object Input--------------------------------------------------
  //@}

  //---Per Curve Properties---------------------------------------------------
  // The following methods are used to set properties on each curve that is
  // plotted. Each input dataset (or data object) results in one curve. The
  // methods that follow have an index i that corresponds to the input dataset
  // or data object.
  void SetPlotColor(int i, double r, double g, double b);
  void SetPlotColor(int i, const double color[3]) {
    this->SetPlotColor(i, color[0], color[1], color[2]); };
  double *GetPlotColor(int i) VTK_SIZEHINT(3);
  void SetPlotSymbol(int i,vtkPolyData *input);
  vtkPolyData *GetPlotSymbol(int i);
  void SetPlotLabel(int i, const char *label);
  const char *GetPlotLabel(int i);

  // Allow per-curve specification of line and point rendering.  These override
  // global settings PlotPoints and PlotLines.  If not on, the default behavior
  // is governed by PlotPoints and PlotLines ivars.
  vtkGetMacro(PlotCurvePoints, int);
  vtkSetMacro(PlotCurvePoints, int);
  vtkBooleanMacro(PlotCurvePoints, int);

  vtkGetMacro(PlotCurveLines, int);
  vtkSetMacro(PlotCurveLines, int);
  vtkBooleanMacro(PlotCurveLines, int);

  void SetPlotLines(int i, int);
  int GetPlotLines(int i);

  void SetPlotPoints(int i, int);
  int GetPlotPoints(int i);
  //---end Per Curve Properties-----------------------------------------------

  //@{
  /**
   * Enable/Disable exchange of the x-y axes (i.e., what was x becomes y, and
   * vice-versa). Exchanging axes affects the labeling as well.
   */
  vtkSetMacro(ExchangeAxes, int);
  vtkGetMacro(ExchangeAxes, int);
  vtkBooleanMacro(ExchangeAxes, int);
  //@}

  //@{
  /**
   * Normally the x-axis is plotted from minimum to maximum. Setting this instance
   * variable causes the x-axis to be plotted from maximum to minimum. Note that
   * boolean always applies to the x-axis even if ExchangeAxes is set.
   */
  vtkSetMacro(ReverseXAxis, int);
  vtkGetMacro(ReverseXAxis, int);
  vtkBooleanMacro(ReverseXAxis, int);
  //@}

  //@{
  /**
   * Normally the y-axis is plotted from minimum to maximum. Setting this instance
   * variable causes the y-axis to be plotted from maximum to minimum. Note that
   * boolean always applies to the y-axis even if ExchangeAxes is set.
   */
  vtkSetMacro(ReverseYAxis, int);
  vtkGetMacro(ReverseYAxis, int);
  vtkBooleanMacro(ReverseYAxis, int);
  //@}

  //@{
  /**
   * Retrieve handles to the legend box and glyph source. This is useful
   * if you would like to change the default behavior of the legend box
   * or glyph source. For example, the default glyph can be changed from
   * a line to a vertex plus line, etc.)
   */
  vtkGetObjectMacro(LegendActor,vtkLegendBoxActor);
  vtkGetObjectMacro(GlyphSource,vtkGlyphSource2D);
  //@}

  //@{
  /**
   * Set/Get the title of the x-y plot.
   */
  vtkSetStringMacro(Title);
  vtkGetStringMacro(Title);
  //@}

  //@{
  /**
   * Set/Get the title of the x axis
   */
  vtkSetStringMacro(XTitle);
  vtkGetStringMacro(XTitle);
  //@}

  //@{
  /**
   * Set/Get the title of the y axis
   */
  virtual void SetYTitle( const char* );
  char* GetYTitle();
  //@}

  //@{
  /**
   * Retrieve handles to the X and Y axis (so that you can set their text
   * properties for example)
   */
  vtkAxisActor2D *GetXAxisActor2D()
  {
    return this->XAxis;
  }
  vtkAxisActor2D *GetYAxisActor2D()
  {
    return this->YAxis;
  }
  //@}

  //@{
  /**
   * Set the plot range (range of independent and dependent variables)
   * to plot. Data outside of the range will be clipped. If the plot
   * range of either the x or y variables is set to (v1,v2), where
   * v1 == v2, then the range will be computed automatically. Note that
   * the x-range values should be consistent with the way the independent
   * variable is created (via INDEX, DISTANCE, or ARC_LENGTH).
   */
  vtkSetVector2Macro(XRange,double);
  vtkGetVectorMacro(XRange,double,2);
  vtkSetVector2Macro(YRange,double);
  vtkGetVectorMacro(YRange,double,2);
  void SetPlotRange(double xmin, double ymin, double xmax, double ymax)
    {this->SetXRange(xmin,xmax); this->SetYRange(ymin,ymax);}
  //@}

  //@{
  /**
   * Set/Get the number of annotation labels to show along the x and y axes.
   * This values is a suggestion: the number of labels may vary depending
   * on the particulars of the data. The convenience method
   * SetNumberOfLabels() sets the number of x and y labels to the same value.
   */
  vtkSetClampMacro(NumberOfXLabels, int, 0, 50);
  vtkGetMacro(NumberOfXLabels, int);
  vtkSetClampMacro(NumberOfYLabels, int, 0, 50);
  vtkGetMacro(NumberOfYLabels, int);
  void SetNumberOfLabels(int num)
    {this->SetNumberOfXLabels(num); this->SetNumberOfYLabels(num);}
  //@}

  //@{
  /**
   * Set/Get the flag that controls whether the labels and ticks are
   * adjusted for "nice" numerical values to make it easier to read
   * the labels. The adjustment is based in the Range instance variable.
   * Call GetAdjustedRange and GetAdjustedNumberOfLabels to get the adjusted
   * range and number of labels.
   */
  void SetAdjustXLabels(int adjust);
  vtkGetMacro( AdjustXLabels, int );
  void SetAdjustYLabels(int adjust);
  vtkGetMacro( AdjustYLabels, int );
  //@}

  //@{
  /**
   * Set/Get the number of minor ticks in X or Y.
   */
  void SetNumberOfXMinorTicks(int num);
  int GetNumberOfXMinorTicks();
  void SetNumberOfYMinorTicks(int num);
  int GetNumberOfYMinorTicks();
  //@}

  //@{
  /**
   * Enable/Disable the creation of a legend. If on, the legend labels will
   * be created automatically unless the per plot legend symbol has been
   * set.
   */
  vtkSetMacro(Legend, int);
  vtkGetMacro(Legend, int);
  vtkBooleanMacro(Legend, int);
  //@}

  //@{
  /**
   * Set/Get the position of the title. This has no effect if
   * AdjustTitlePosition is true.
   */
  vtkSetVector2Macro(TitlePosition,double);
  vtkGetVector2Macro(TitlePosition,double);
  //@}

  //@{
  /**
   * If true, the xyplot actor will adjust the position of the title
   * automatically to be upper-middle. Default is true.
   */
  vtkSetMacro(AdjustTitlePosition, int);
  vtkGetMacro(AdjustTitlePosition, int);
  vtkBooleanMacro(AdjustTitlePosition, int);
  //@}

enum Alignment {
  AlignLeft = 0x1,
  AlignRight = 0x2,
  AlignHCenter = 0x4,
  AlignTop = 0x10,
  AlignBottom = 0x20,
  AlignVCenter = 0x40,
  AlignAxisLeft = 0x100,
  AlignAxisRight = 0x200,
  AlignAxisHCenter = 0x400,
  AlignAxisTop = 0x1000,
  AlignAxisBottom = 0x2000,
  AlignAxisVCenter = 0x4000
};

  //@{
  /**
   * If AdjustTitlePosition is true, the xyplot actor will
   * adjust the position of the title automatically depending on the
   * given mode, the mode is a combination of the Alignment flags.
   * by default: vtkXYPlotActor::AlignHCenter | vtkXYPlotActor::Top
   * | vtkXYPlotActor::AlignAxisVCenter
   */
  vtkSetMacro(AdjustTitlePositionMode, int);
  vtkGetMacro(AdjustTitlePositionMode, int);
  //@}

  //@{
  /**
   * Use these methods to control the position of the legend. The variables
   * LegendPosition and LegendPosition2 define the lower-left and upper-right
   * position of the legend. The coordinates are expressed as normalized
   * values with respect to the rectangle defined by PositionCoordinate and
   * Position2Coordinate. Note that LegendPosition2 is relative to
   * LegendPosition.
   */
  vtkSetVector2Macro(LegendPosition,double);
  vtkGetVector2Macro(LegendPosition,double);
  vtkSetVector2Macro(LegendPosition2,double);
  vtkGetVector2Macro(LegendPosition2,double);
  //@}

  //@{
  /**
   * Set/Get the title text property.
   */
  virtual void SetTitleTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(TitleTextProperty,vtkTextProperty);
  //@}

  //@{
  /**
   * Set/Get the title text property of all axes. Note that each axis can
   * be controlled individually through the GetX/YAxisActor2D() methods.
   */
  virtual void SetAxisTitleTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(AxisTitleTextProperty,vtkTextProperty);
  //@}

  //@{
  /**
   * Set/Get the labels text property of all axes. Note that each axis can
   * be controlled individually through the GetX/YAxisActor2D() methods.
   */
  virtual void SetAxisLabelTextProperty(vtkTextProperty *p);
  vtkGetObjectMacro(AxisLabelTextProperty,vtkTextProperty);
  //@}

  //@{
  /**
   * Enable/Disable plotting of Log of x-values.
   */
  vtkSetMacro(Logx, int);
  vtkGetMacro(Logx, int);
  vtkBooleanMacro(Logx, int);
  //@}

  //@{
  /**
   * Set/Get the format with which to print the labels . This sets both X
   * and Y label formats. GetLabelFormat() returns X label format.
   */
  virtual void SetLabelFormat ( const char* );
  const char* GetLabelFormat()
  {
    return this->GetXLabelFormat();
  }
  //@}

  //@{
  /**
   * Set/Get the format with which to print the X label.
   */
  virtual void SetXLabelFormat ( const char* );
  vtkGetStringMacro(XLabelFormat);
  //@}

  //@{
  /**
   * Set/Get the format with which to print the Y label.
   */
  virtual void SetYLabelFormat ( const char* );
  vtkGetStringMacro(YLabelFormat);
  //@}

  //@{
  /**
   * Set/Get the spacing between the plot window and the plot. The value
   * is specified in pixels.
   */
  vtkSetClampMacro(Border, int, 0, 50);
  vtkGetMacro(Border, int);
  //@}

  //@{
  /**
   * Set/Get whether the points are rendered.  The point size can be set in
   * the property object. This is a global flag which affects the plot only
   * if per curve symbols are not defined.
   */
  vtkGetMacro(PlotPoints, int);
  vtkSetMacro(PlotPoints, int);
  vtkBooleanMacro(PlotPoints, int);
  //@}

  //@{
  /**
   * Set/Get whether the lines are rendered.  The line width can be set in
   * the property object.
   */
  vtkGetMacro(PlotLines, int);
  vtkSetMacro(PlotLines, int);
  vtkBooleanMacro(PlotLines, int);
  //@}

  //@{
  /**
   * Set/Get the factor that controls how big glyphs are in the plot.
   * The number is expressed as a fraction of the length of the diagonal
   * of the plot bounding box.
   */
  vtkSetClampMacro(GlyphSize, double, 0.0, 0.2);
  vtkGetMacro(GlyphSize, double);
  //@}

  /**
   * Given a position within the viewport used by the plot, return the
   * the plot coordinates (XAxis value, YAxis value)
   */
  void ViewportToPlotCoordinate(vtkViewport *viewport, double &u, double &v);

  //@{
  /**
   * An alternate form of ViewportToPlotCoordinate() above. This method
   * inputs the viewport coordinate pair (defined by the ivar
   * ViewportCoordinate)and then stores them in the ivar PlotCoordinate.
   */
  void ViewportToPlotCoordinate(vtkViewport *viewport);
  vtkSetVector2Macro(PlotCoordinate,double);
  vtkGetVector2Macro(PlotCoordinate,double);
  //@}

  /**
   * Given a plot coordinate, return the viewpoint position
   */
  void PlotToViewportCoordinate(vtkViewport *viewport, double &u, double &v);

  //@{
  /**
   * An alternate form of PlotToViewportCoordinate() above. This method
   * inputs the plot coordinate pair (defined in the ivar PlotCoordinate)
   * and then stores them in the ivar ViewportCoordinate. (This method
   * can be wrapped.)
   */
  void PlotToViewportCoordinate(vtkViewport *viewport);
  vtkSetVector2Macro(ViewportCoordinate,double);
  vtkGetVector2Macro(ViewportCoordinate,double);
  //@}

  /**
   * Is the specified viewport position within the plot area (as opposed to the
   * region used by the plot plus the labels)?
   */
  int IsInPlot(vtkViewport *viewport, double u, double v);

  //@{
  /**
   * Set/Get the flag that controls whether a box will be drawn/filled
   * corresponding to the chart box.
   */
  vtkSetMacro(ChartBox, int);
  vtkGetMacro(ChartBox, int);
  vtkBooleanMacro(ChartBox, int);
  //@}

  //@{
  /**
   * Set/Get the flag that controls whether a box will be drawn/filled
   * corresponding to the legend box.
   */
  vtkSetMacro(ChartBorder, int);
  vtkGetMacro(ChartBorder, int);
  vtkBooleanMacro(ChartBorder, int);
  //@}

  /**
   * Get the box vtkProperty2D.
   */
  vtkProperty2D* GetChartBoxProperty() { return this->ChartBoxActor->GetProperty(); };

  //@{
  /**
   * Set/Get if the X reference line is visible. hidden by default
   */
  vtkSetMacro(ShowReferenceXLine, int);
  vtkGetMacro(ShowReferenceXLine, int);
  vtkBooleanMacro(ShowReferenceXLine, int);
  //@}

  //@{
  /**
   * Set/Get the value for the X reference line
   */
  vtkSetMacro(ReferenceXValue, double);
  vtkGetMacro(ReferenceXValue, double);
  //@}

  //@{
  /**
   * Set/Get if the Y reference line is visible. hidden by default
   */
  vtkSetMacro(ShowReferenceYLine, int);
  vtkGetMacro(ShowReferenceYLine, int);
  vtkBooleanMacro(ShowReferenceYLine, int);
  //@}

  //@{
  /**
   * Set/Get the value for the Y reference line
   */
  vtkSetMacro(ReferenceYValue, double);
  vtkGetMacro(ReferenceYValue, double);
  //@}

  /**
   * Take into account the modified time of internal helper classes.
   */
  vtkMTimeType GetMTime() override;

  /**
   * Write the XY Ploat Actor as a CSV (comma separated value) representation.
   */
  void PrintAsCSV(ostream &os);

  //@{
  /**
   * WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
   * DO NOT USE THIS METHOD OUTSIDE OF THE RENDERING PROCESS.
   * Draw the x-y plot.
   */
  int RenderOpaqueGeometry(vtkViewport*) override;
  int RenderOverlay(vtkViewport*) override;
  int RenderTranslucentPolygonalGeometry(vtkViewport *) override {return 0;}
  //@}

  /**
   * Does this prop have some translucent polygonal geometry?
   */
  int HasTranslucentPolygonalGeometry() override;

  /**
   * Release any graphics resources that are being consumed by this actor.
   * The parameter window could be used to determine which graphic
   * resources to release.
   */
  void ReleaseGraphicsResources(vtkWindow *) override;

  //@{
  /**
   * Set/Get the position of the title of X axis.
   */
  void SetXTitlePosition(double position);
  double GetXTitlePosition();
  //@}

  //@{
  /**
   * Set/Get the position of the title of Y axis.
   */
  vtkSetMacro(YTitlePosition,int);
  vtkGetMacro(YTitlePosition,int);
  void SetYTitlePositionToTop()
  {
    this->SetYTitlePosition( VTK_XYPLOT_Y_AXIS_TOP );
  }
  void SetYTitlePositionToHCenter()
  {
    this->SetYTitlePosition( VTK_XYPLOT_Y_AXIS_HCENTER );
  }
  void SetYTitlePositionToVCenter()
  {
    this->SetYTitlePosition( VTK_XYPLOT_Y_AXIS_VCENTER );
  }
  //@}

  //@{
  /**
   * Set plot properties
   */
  virtual void SetPlotGlyphType( int, int );
  virtual void SetLineWidth( double );
  virtual void AddUserCurvesPoint( double, double, double );
  virtual void RemoveAllActiveCurves();
  //@}

  //@{
  /**
   * Set legend properties
   */
  virtual void SetLegendBorder( int );
  virtual void SetLegendBox( int );
  virtual void SetLegendUseBackground( int );
  virtual void SetLegendBackgroundColor( double, double, double );
  //@}

  //@{
  /**
   * Set title properties
   */
  virtual void SetTitleColor( double, double, double );
  virtual void SetTitleFontFamily( int );
  virtual void SetTitleBold( int );
  virtual void SetTitleItalic( int );
  virtual void SetTitleShadow( int );
  virtual void SetTitleFontSize( int );
  virtual void SetTitleJustification( int );
  virtual void SetTitleVerticalJustification( int );
  //@}

  //@{
  /**
   * Set axes properties
   */
  virtual void SetXAxisColor( double, double, double );
  virtual void SetYAxisColor( double, double, double );
  //@}

  //@{
  /**
   * Set axis title properties
   */
  virtual void SetAxisTitleColor( double, double, double );
  virtual void SetAxisTitleFontFamily( int );
  virtual void SetAxisTitleBold( int );
  virtual void SetAxisTitleItalic( int );
  virtual void SetAxisTitleShadow( int );
  virtual void SetAxisTitleFontSize( int );
  virtual void SetAxisTitleJustification( int );
  virtual void SetAxisTitleVerticalJustification( int );
  //@}

  //@{
  /**
   * Set axis label properties
   */
  virtual void SetAxisLabelColor( double, double, double );
  virtual void SetAxisLabelFontFamily( int );
  virtual void SetAxisLabelBold( int );
  virtual void SetAxisLabelItalic( int );
  virtual void SetAxisLabelShadow( int );
  virtual void SetAxisLabelFontSize( int );
  virtual void SetAxisLabelJustification( int );
  virtual void SetAxisLabelVerticalJustification( int );
  //@}

protected:
  vtkXYPlotActor();
  ~vtkXYPlotActor() override;

  vtkXYPlotActorConnections* InputConnectionHolder;
  char** SelectedInputScalars; // list of data set arrays to plot
  vtkIntArray* SelectedInputScalarsComponent; // list of components
  vtkXYPlotActorConnections *DataObjectInputConnectionHolder; //list of data objects to plot
  char*  Title;
  char*  XTitle;
  vtkTextActor* YTitleActor;
  int   XValues;
  int   NumberOfXLabels;
  int   NumberOfYLabels;
  int   Logx;
  char* XLabelFormat;
  char* YLabelFormat;
  double XRange[2];
  double YRange[2];
  double XComputedRange[2];  //range actually used by plot
  double YComputedRange[2];  //range actually used by plot
  int Border;
  int PlotLines;
  int PlotPoints;
  int PlotCurveLines;
  int PlotCurvePoints;
  int ExchangeAxes;
  int ReverseXAxis;
  int ReverseYAxis;
  int AdjustXLabels;
  int AdjustYLabels;
  int AdjustTitlePosition;
  double TitlePosition[2];
  int AdjustTitlePositionMode;

  vtkTextMapper*   TitleMapper;
  vtkActor2D*      TitleActor;
  vtkTextProperty* TitleTextProperty;

  vtkAxisActor2D*  XAxis;
  vtkAxisActor2D*  YAxis;

  vtkTextProperty* AxisTitleTextProperty;
  vtkTextProperty* AxisLabelTextProperty;

  double ViewportCoordinate[2];
  double PlotCoordinate[2];

  //Handle data objects and datasets
  int DataObjectPlotMode;
  vtkIntArray* XComponent;
  vtkIntArray* YComponent;
  vtkIntArray* LinesOn;
  vtkIntArray* PointsOn;

  //The data drawn within the axes. Each curve is one polydata.
  //color is controlled by scalar data. The curves are appended
  //together, possibly glyphed with point symbols.
  int NumberOfInputs;
  vtkPolyData             **PlotData;
  vtkGlyph2D              **PlotGlyph;
  vtkAppendPolyData       **PlotAppend;
  vtkPolyDataMapper2D     **PlotMapper;
  vtkActor2D              **PlotActor;
  void                    InitializeEntries();

  // Legends and plot symbols. The legend also keeps track of
  // the symbols and such.
  int Legend;
  double LegendPosition[2];
  double LegendPosition2[2];
  vtkLegendBoxActor *LegendActor;
  vtkGlyphSource2D *GlyphSource;
  vtkPlanes *ClipPlanes;
  double GlyphSize;

  // Background box
  int ChartBox;
  vtkPolyData                *ChartBoxPolyData;
  vtkPolyDataMapper2D        *ChartBoxMapper;
  vtkActor2D                 *ChartBoxActor;
  int ChartBorder;
  vtkPolyData                *ChartBorderPolyData;
  vtkPolyDataMapper2D        *ChartBorderMapper;
  vtkActor2D                 *ChartBorderActor;

  // Reference lines
  int ShowReferenceXLine;
  int ShowReferenceYLine;
  double ReferenceXValue;
  double ReferenceYValue;

  vtkPolyData                *ReferenceLinesPolyData;
  vtkPolyDataMapper2D        *ReferenceLinesMapper;
  vtkActor2D                 *ReferenceLinesActor;

  // Keep track of changes.
  int CachedSize[2];
  vtkTimeStamp  BuildTime;

  void ComputeXRange(double range[2], double *lengths);
  void ComputeYRange(double range[2]);
  void ComputeDORange(double xrange[2], double yrange[2], double *lengths);

  virtual void CreatePlotData(int *pos, int *pos2, double xRange[2],
                              double yRange[2], double *norms,
                              int numDS, int numDO);
  void PlaceAxes(vtkViewport *viewport, int *size, int pos[2], int pos2[2]);
  void GenerateClipPlanes(int *pos, int *pos2);
  double ComputeGlyphScale(int i, int *pos, int *pos2);
  void ClipPlotData(int *pos, int *pos2, vtkPolyData *pd);
  double *TransformPoint(int pos[2], int pos2[2], double x[3], double xNew[3]);

  vtkSmartPointer<vtkDoubleArray> ActiveCurve;

  int YAxisTitleSize;
  int ActiveCurveIndex;
  int PlotColorIndex;

private:
  vtkXYPlotActor(const vtkXYPlotActor&) = delete;
  void operator=(const vtkXYPlotActor&) = delete;

  bool DoesConnectionMatch(int i, vtkAlgorithmOutput* in);

  int IsInputPresent(vtkAlgorithmOutput* in,
                     const char* arrayName,
                     int component);

  /**
   * Estimated sizes of Y axis title
   */
  int YTitleSize[2];

  /**
   * Position and orientation of Y axis title
   */
  int YTitlePosition;

  //@{
  /**
   * Estimated size of Y axis spacing
   */
  int YTitleDelta;
};
  //@}


#endif
