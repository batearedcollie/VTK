#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# This example demonstrates the use of multiline 2D text using
# vtkTextMappers.  It shows several justifications as well as single-line
# and multiple-line text inputs.
#
# First we include the VTK Tcl packages which will make available
# all of the vtk commands to Tcl
#
font_size = 14
# Create the text mappers and the associated Actor2Ds.
# The font and text properties (except justification) are the same for each
# single line mapper. Let's create a common text property object
singleLineTextProp = vtk.vtkTextProperty()
singleLineTextProp.SetFontSize(font_size)
singleLineTextProp.SetFontFamilyToArial()
singleLineTextProp.BoldOff()
singleLineTextProp.ItalicOff()
singleLineTextProp.ShadowOff()
# The font and text properties (except justification) are the same for each
# multi line mapper. Let's create a common text property object
multiLineTextProp = vtk.vtkTextProperty()
multiLineTextProp.ShallowCopy(singleLineTextProp)
multiLineTextProp.BoldOn()
multiLineTextProp.ItalicOn()
multiLineTextProp.ShadowOn()
# The text is on a single line and bottom-justified.
singleLineTextB = vtk.vtkTextMapper()
singleLineTextB.SetInput("Single line (bottom)")
tprop = singleLineTextB.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetVerticalJustificationToBottom()
tprop.SetColor(1,0,0)
singleLineTextActorB = vtk.vtkActor2D()
singleLineTextActorB.SetMapper(singleLineTextB)
singleLineTextActorB.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorB.GetPositionCoordinate().SetValue(0.05,0.85)
# The text is on a single line and center-justified (vertical justification).
singleLineTextC = vtk.vtkTextMapper()
singleLineTextC.SetInput("Single line (centered)")
tprop = singleLineTextC.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetVerticalJustificationToCentered()
tprop.SetColor(0,1,0)
singleLineTextActorC = vtk.vtkActor2D()
singleLineTextActorC.SetMapper(singleLineTextC)
singleLineTextActorC.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorC.GetPositionCoordinate().SetValue(0.05,0.75)
# The text is on a single line and top-justified - above 0.5
singleLineTextT = vtk.vtkTextMapper()
singleLineTextT.SetInput("Single line (top)")
tprop = singleLineTextT.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetVerticalJustificationToTop()
tprop.SetColor(0,0,1)
singleLineTextActorT = vtk.vtkActor2D()
singleLineTextActorT.SetMapper(singleLineTextT)
singleLineTextActorT.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorT.GetPositionCoordinate().SetValue(0.05,0.65)
# The text is on a single line and top-justified -below 0.5
singleLineTextTB = vtk.vtkTextMapper()
singleLineTextTB.SetInput("Single line below (top)")
tprop = singleLineTextTB.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetVerticalJustificationToTop()
tprop.SetColor(0,0,1)
singleLineTextActorTB = vtk.vtkActor2D()
singleLineTextActorTB.SetMapper(singleLineTextTB)
singleLineTextActorTB.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorTB.GetPositionCoordinate().SetValue(0.5,0.25)
# The text is on a single line and centered -below 0.5
singleLineTextCC = vtk.vtkTextMapper()
singleLineTextCC.SetInput("HHHHH")
tprop = singleLineTextCC.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetJustificationToCentered()
tprop.SetVerticalJustificationToCentered()
tprop.UseTightBoundingBoxOn()
tprop.SetColor(0,0,0)
singleLineTextActorCC = vtk.vtkActor2D()
singleLineTextActorCC.SetMapper(singleLineTextCC)
singleLineTextActorCC.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorCC.GetPositionCoordinate().SetValue(0.5,0.15)
# The text is on a single line and bottom-right justified with a tight bounding-box
singleLineTextBR = vtk.vtkTextMapper()
singleLineTextBR.SetInput("Line bottom")
tprop = singleLineTextBR.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetJustificationToRight()
tprop.SetVerticalJustificationToBottom()
tprop.UseTightBoundingBoxOn()
tprop.SetColor(0,0,1)
singleLineTextActorBR = vtk.vtkActor2D()
singleLineTextActorBR.SetMapper(singleLineTextBR)
singleLineTextActorBR.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorBR.GetPositionCoordinate().SetValue(0.95,0.15)
# The text is on a single line and bottom-right justified with a tight bounding-box
singleLineTextBL = vtk.vtkTextMapper()
singleLineTextBL.SetInput("Tight line (bottom)")
tprop = singleLineTextBL.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetJustificationToLeft()
tprop.SetVerticalJustificationToBottom()
tprop.UseTightBoundingBoxOn()
tprop.SetColor(0,0,1)
singleLineTextActorBL = vtk.vtkActor2D()
singleLineTextActorBL.SetMapper(singleLineTextBL)
singleLineTextActorBL.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorBL.GetPositionCoordinate().SetValue(0.05,0.15)
# The text is on a single line and top-justified - above 0.5
singleLineTextLTT = vtk.vtkTextMapper()
singleLineTextLTT.SetInput("Single line (top)")
tprop = singleLineTextLTT.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetVerticalJustificationToTop()
tprop.UseTightBoundingBoxOn()
tprop.SetColor(0,0,1)
singleLineTextActorLTT = vtk.vtkActor2D()
singleLineTextActorLTT.SetMapper(singleLineTextLTT)
singleLineTextActorLTT.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorLTT.GetPositionCoordinate().SetValue(0.05,0.15)
# The text is on a single line and top-justified - above 0.5
singleLineTextRTT = vtk.vtkTextMapper()
singleLineTextRTT.SetInput("nge ne op")
tprop = singleLineTextRTT.GetTextProperty()
tprop.ShallowCopy(singleLineTextProp)
tprop.SetJustificationToRight()
tprop.SetVerticalJustificationToTop()
tprop.UseTightBoundingBoxOn()
tprop.SetColor(0,0,1)
singleLineTextActorRTT = vtk.vtkActor2D()
singleLineTextActorRTT.SetMapper(singleLineTextRTT)
singleLineTextActorRTT.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
singleLineTextActorRTT.GetPositionCoordinate().SetValue(0.95,0.15)

# The text is on multiple lines and left- and top-justified - below 0.5
textMapperL = vtk.vtkTextMapper()
textMapperL.SetInput("This is\nmulti-line\ntext output\n(left-top)")
tprop = textMapperL.GetTextProperty()
tprop.ShallowCopy(multiLineTextProp)
tprop.SetJustificationToLeft()
tprop.SetVerticalJustificationToTop()
tprop.SetColor(1,0,0)
textActorL = vtk.vtkActor2D()
textActorL.SetMapper(textMapperL)
textActorL.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
textActorL.GetPositionCoordinate().SetValue(0.05,0.5)
# The text is on multiple lines and left- and top-justified - above 0.5
textMapperLA = vtk.vtkTextMapper()
textMapperLA.SetInput("This is\nmulti-line\ntext output\nabove (left-top)")
tprop = textMapperLA.GetTextProperty()
tprop.ShallowCopy(multiLineTextProp)
tprop.SetJustificationToLeft()
tprop.SetVerticalJustificationToTop()
tprop.SetColor(1,0,0)
textActorLA = vtk.vtkActor2D()
textActorLA.SetMapper(textMapperLA)
textActorLA.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
textActorLA.GetPositionCoordinate().SetValue(0.5,0.85)
# The text is on multiple lines and center-justified (both horizontal and
# vertical).
textMapperC = vtk.vtkTextMapper()
textMapperC.SetInput("This is\nmulti-line\ntext output\n(centered)")
tprop = textMapperC.GetTextProperty()
tprop.ShallowCopy(multiLineTextProp)
tprop.SetJustificationToCentered()
tprop.SetVerticalJustificationToCentered()
tprop.SetColor(0,1,0)
textActorC = vtk.vtkActor2D()
textActorC.SetMapper(textMapperC)
textActorC.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
textActorC.GetPositionCoordinate().SetValue(0.5,0.5)
# The text is on multiple lines and right- and bottom-justified.
textMapperR = vtk.vtkTextMapper()
textMapperR.SetInput("This is\nmulti-line\ntext output\n(right-bottom)")
tprop = textMapperR.GetTextProperty()
tprop.ShallowCopy(multiLineTextProp)
tprop.SetJustificationToRight()
tprop.SetVerticalJustificationToBottom()
tprop.SetColor(0,0,1)
textActorR = vtk.vtkActor2D()
textActorR.SetMapper(textMapperR)
textActorR.GetPositionCoordinate().SetCoordinateSystemToNormalizedDisplay()
textActorR.GetPositionCoordinate().SetValue(0.95,0.5)
# Draw the grid to demonstrate the placement of the text.
# Set up the necessary points.
Pts = vtk.vtkPoints()
Pts.InsertNextPoint(0.05,0.0,0.0)
Pts.InsertNextPoint(0.05,1.0,0.0)
Pts.InsertNextPoint(0.5,0.0,0.0)
Pts.InsertNextPoint(0.5,1.0,0.0)
Pts.InsertNextPoint(0.95,0.0,0.0)
Pts.InsertNextPoint(0.95,1.0,0.0)
Pts.InsertNextPoint(0.0,0.5,0.0)
Pts.InsertNextPoint(1.0,0.5,0.0)
Pts.InsertNextPoint(0.00,0.85,0.0)
Pts.InsertNextPoint(1.00,0.85,0.0)
Pts.InsertNextPoint(0.00,0.75,0.0)
Pts.InsertNextPoint(0.50,0.75,0.0)
Pts.InsertNextPoint(0.00,0.65,0.0)
Pts.InsertNextPoint(0.50,0.65,0.0)
Pts.InsertNextPoint(0.00,0.25,0.0)
Pts.InsertNextPoint(1.00,0.25,0.0)
Pts.InsertNextPoint(0.00,0.15,0.0)
Pts.InsertNextPoint(1.00,0.15,0.0)

# Set up the lines that use these points.
Lines = vtk.vtkCellArray()
Lines.InsertNextCell(2)
Lines.InsertCellPoint(0)
Lines.InsertCellPoint(1)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(2)
Lines.InsertCellPoint(3)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(4)
Lines.InsertCellPoint(5)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(6)
Lines.InsertCellPoint(7)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(8)
Lines.InsertCellPoint(9)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(10)
Lines.InsertCellPoint(11)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(12)
Lines.InsertCellPoint(13)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(14)
Lines.InsertCellPoint(15)
Lines.InsertNextCell(2)
Lines.InsertCellPoint(16)
Lines.InsertCellPoint(17)


# Create a grid that uses these points and lines.
Grid = vtk.vtkPolyData()
Grid.SetPoints(Pts)
Grid.SetLines(Lines)
# Set up the coordinate system.
normCoords = vtk.vtkCoordinate()
normCoords.SetCoordinateSystemToNormalizedDisplay()
# Set up the mapper and actor (2D) for the grid.
mapper = vtk.vtkPolyDataMapper2D()
mapper.SetInputData(Grid)
mapper.SetTransformCoordinate(normCoords)
gridActor = vtk.vtkActor2D()
gridActor.SetMapper(mapper)
gridActor.GetProperty().SetColor(0.1,0.1,0.1)
# Create the Renderer, RenderWindow, and RenderWindowInteractor
#
ren1 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.SetMultiSamples(0)
renWin.AddRenderer(ren1)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
# Add the actors to the renderer; set the background and size; zoom in
# closer to the image; render
#
ren1.AddActor2D(gridActor)
ren1.AddActor2D(textActorL)
ren1.AddActor2D(singleLineTextActorT)
ren1.AddActor2D(textActorLA)
ren1.AddActor2D(textActorC)
ren1.AddActor2D(textActorR)
ren1.AddActor2D(singleLineTextActorB)
ren1.AddActor2D(singleLineTextActorC)
ren1.AddActor2D(singleLineTextActorLTT)
ren1.AddActor2D(singleLineTextActorRTT)
ren1.AddActor2D(singleLineTextActorTB)
ren1.AddActor2D(singleLineTextActorBR)
ren1.AddActor2D(singleLineTextActorBL)
ren1.AddActor2D(singleLineTextActorCC)
ren1.SetBackground(1,1,1)
renWin.SetSize(500,300)
ren1.GetActiveCamera().Zoom(1.5)
renWin.Render()
# Set the user method (bound to key 'u')
#
# Withdraw the default tk window.
# --- end of script --
