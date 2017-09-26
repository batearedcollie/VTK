/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGL2PSExporter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGL2PSExporter
 * @brief   export a scene as a PostScript file using GL2PS.
 *
 * vtkGL2PSExporter is a concrete subclass of vtkExporter that writes
 * high quality vector PostScript (PS/EPS), PDF or SVG files by using
 * GL2PS.  GL2PS can be obtained at: http://www.geuz.org/gl2ps/.  This
 * can be very useful when one requires publication quality pictures.
 * This class works best with simple 3D scenes and most 2D plots.
 * Please note that GL2PS has its limitations since PostScript is not
 * an ideal language to represent complex 3D scenes.  However, this
 * class does allow one to write mixed vector/raster files by using
 * the Write3DPropsAsRasterImage ivar.  Please do read the caveats
 * section of this documentation.
 *
 * By default vtkGL2PSExporter generates Encapsulated PostScript (EPS)
 * output along with the text in portrait orientation with the
 * background color of the window being drawn.  The generated output
 * is also compressed using zlib. The various other options are set to
 * sensible defaults.
 *
 * The output file format (FileFormat) can be either PostScript (PS),
 * Encapsulated PostScript (EPS), PDF, SVG or TeX.  The file extension
 * is generated automatically depending on the FileFormat.  The
 * default is EPS.  When TeX output is chosen, only the text strings
 * in the plot are generated and put into a picture environment.  One
 * can turn on and off the text when generating PS/EPS/PDF/SVG files
 * by using the Text boolean variable.  By default the text is drawn.
 * The background color of the renderwindow is drawn by default.  To
 * make the background white instead use the DrawBackgroundOff
 * function.  Landscape figures can be generated by using the
 * LandscapeOn function.  Portrait orientation is used by default.
 * Several of the GL2PS options can be set.  The names of the ivars
 * for these options are similar to the ones that GL2PS provides.
 * Compress, SimpleLineOffset, Silent, BestRoot, PS3Shading and
 * OcclusionCull are similar to the options provided by GL2PS.  Please
 * read the function documentation or the GL2PS documentation for more
 * details.  The ivar Write3DPropsAsRasterImage allows one to generate
 * mixed vector/raster images.  All the 3D props in the scene will be
 * written as a raster image and all 2D actors will be written as
 * vector graphic primitives.  This makes it possible to handle
 * transparency and complex 3D scenes.  This ivar is set to Off by
 * default.  Specific 3D props can be excluded from the rasterization
 * process by adding them to the RasterExclusions ivar.  Props in this
 * collection will be rendered as 2D vector primitives instead.
 *
 * @warning
 * By default (with Write3DPropsAsRasterImage set to Off) exporting
 * complex 3D scenes can take a long while and result in huge output
 * files.  Generating correct vector graphics output for scenes with
 * transparency is almost impossible.  However, one can set
 * Write3DPropsAsRasterImageOn and generate mixed vector/raster files.
 * This should work fine with complex scenes along with transparent
 * actors.
 *
 * @sa
 * vtkExporter
 *
 * @par Thanks:
 * Thanks to Goodwin Lawlor and Prabhu Ramachandran for this class.
*/

#ifndef vtkOpenGLGL2PSExporter_h
#define vtkOpenGLGL2PSExporter_h

#include "vtkIOExportOpenGLModule.h" // For export macro
#include "vtkGL2PSExporter.h"

#include "vtkNew.h" // For vtkNew

class vtkActor;
class vtkActor2D;
class vtkBillboardTextActor3D;
class vtkCollection;
class vtkCoordinate;
class vtkImageData;
class vtkIntArray;
class vtkLabeledDataMapper;
class vtkLabeledContourMapper;
class vtkMatrix4x4;
class vtkPath;
class vtkProp;
class vtkPropCollection;
class vtkProp3DCollection;
class vtkRenderer;
class vtkRendererCollection;
class vtkScalarBarActor;
class vtkTextActor;
class vtkTextActor3D;
class vtkTextMapper;
class vtkTextProperty;
class vtkTexturedActor2D;

class VTKIOEXPORTOPENGL_EXPORT vtkOpenGLGL2PSExporter : public vtkGL2PSExporter
{
public:
  static vtkOpenGLGL2PSExporter *New();
  vtkTypeMacro(vtkOpenGLGL2PSExporter, vtkGL2PSExporter)
  void PrintSelf(ostream& os, vtkIndent indent) override;

protected:
  vtkOpenGLGL2PSExporter();
  ~vtkOpenGLGL2PSExporter() override;

  void WriteData() override;

  void SavePropVisibility(vtkRendererCollection *renCol,
                          vtkIntArray *volVis, vtkIntArray *actVis,
                          vtkIntArray *act2dVis);
  void RestorePropVisibility(vtkRendererCollection *renCol,
                             vtkIntArray *volVis, vtkIntArray *actVis,
                             vtkIntArray *act2dVis);
  void Turn3DPropsOff(vtkRendererCollection *renCol);
  void Turn2DPropsOff(vtkRendererCollection *renCol);
  static void TurnSpecialPropsOff(vtkCollection *specialPropCol,
                           vtkRendererCollection *renCol);

  void GetVisibleContextActors(vtkPropCollection *contextActors,
                               vtkRendererCollection *renCol);
  void SetPropVisibilities(vtkPropCollection *col, int vis);

  void DrawSpecialProps(vtkCollection *propCol, vtkRendererCollection *renCol);
  //@{
  /**
   * Reimplement this to handle your own special props. Must call this function
   * at the end of the override for default handling.
   */
  virtual void HandleSpecialProp(vtkProp *prop, vtkRenderer *ren);
  void DrawBillboardTextActor3D(vtkBillboardTextActor3D *textAct,
                                vtkRenderer *ren);
  void DrawTextActor(vtkTextActor *textAct, vtkRenderer *ren);
  void DrawTextActor3D(vtkTextActor3D *textAct, vtkRenderer *ren);
  void DrawTextMapper(vtkTextMapper *textMap, vtkActor2D *textAct,
                      vtkRenderer *ren);
  void DrawLabeledDataMapper(vtkLabeledDataMapper *mapper, vtkRenderer *ren);
  void DrawLabeledContourMapper(vtkActor *act, vtkLabeledContourMapper *mapper,
                                vtkRenderer *ren);
  void DrawScalarBarActor(vtkScalarBarActor *bar, vtkRenderer *ren);
  void DrawTexturedActor2D(vtkTexturedActor2D *act, vtkRenderer *ren);
  void DrawViewportTextOverlay(const char *string, vtkTextProperty *tprop,
                               vtkCoordinate *coord, vtkRenderer *ren);
  //@}

  /**
   * Copy the region copyRect from the framebuffer into the gl2ps document.
   * copyRect is in viewport coordinates [xmin, ymin, width, height].
   */
  void CopyPixels(int copyRect[4], vtkRenderer *ren);

  void DrawContextActors(vtkPropCollection *contextActs,
                         vtkRendererCollection *renCol);

  vtkNew<vtkImageData> PixelData;

private:
  vtkOpenGLGL2PSExporter(const vtkOpenGLGL2PSExporter&) = delete;
  void operator=(const vtkOpenGLGL2PSExporter&) = delete;
};

#endif
