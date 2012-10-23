#!/usr/bin/env python
import vtk
from vtk.test import Testing
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# demonstrate use of point labeling and the selection window
# get the interactor ui
# Create the RenderWindow, Renderer and both Actors
#
ren1 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
# Create a selection window
xmin = 200
xLength = 100
xmax = expr.expr(globals(), locals(),["xmin","+","xLength"])
ymin = 200
yLength = 100
ymax = expr.expr(globals(), locals(),["ymin","+","yLength"])
pts = vtk.vtkPoints()
pts.InsertPoint(0,xmin,ymin,0)
pts.InsertPoint(1,xmax,ymin,0)
pts.InsertPoint(2,xmax,ymax,0)
pts.InsertPoint(3,xmin,ymax,0)
rect = vtk.vtkCellArray()
rect.InsertNextCell(5)
rect.InsertCellPoint(0)
rect.InsertCellPoint(1)
rect.InsertCellPoint(2)
rect.InsertCellPoint(3)
rect.InsertCellPoint(0)
selectRect = vtk.vtkPolyData()
selectRect.SetPoints(pts)
selectRect.SetLines(rect)
rectMapper = vtk.vtkPolyDataMapper2D()
rectMapper.SetInputData(selectRect)
rectActor = vtk.vtkActor2D()
rectActor.SetMapper(rectMapper)
# Create asphere
sphere = vtk.vtkSphereSource()
sphereMapper = vtk.vtkPolyDataMapper()
sphereMapper.SetInputConnection(sphere.GetOutputPort())
sphereMapper.GlobalImmediateModeRenderingOn()
sphereActor = vtk.vtkActor()
sphereActor.SetMapper(sphereMapper)
# Generate ids for labeling
ids = vtk.vtkIdFilter()
ids.SetInputConnection(sphere.GetOutputPort())
ids.PointIdsOn()
ids.CellIdsOn()
ids.FieldDataOn()
# Create labels for points
visPts = vtk.vtkSelectVisiblePoints()
visPts.SetInputConnection(ids.GetOutputPort())
visPts.SetRenderer(ren1)
visPts.SelectionWindowOn()
visPts.SetSelection(xmin,expr.expr(globals(), locals(),["xmin","+","xLength"]),ymin,expr.expr(globals(), locals(),["ymin","+","yLength"]))
ldm = vtk.vtkLabeledDataMapper()
ldm.SetInputConnection(visPts.GetOutputPort())
#    ldm SetLabelFormat "%g"
#    ldm SetLabelModeToLabelScalars
#    ldm SetLabelModeToLabelNormals
ldm.SetLabelModeToLabelFieldData()
#    ldm SetLabeledComponent 0
pointLabels = vtk.vtkActor2D()
pointLabels.SetMapper(ldm)
# Create labels for cells
cc = vtk.vtkCellCenters()
cc.SetInputConnection(ids.GetOutputPort())
visCells = vtk.vtkSelectVisiblePoints()
visCells.SetInputConnection(cc.GetOutputPort())
visCells.SetRenderer(ren1)
visCells.SelectionWindowOn()
visCells.SetSelection(xmin,expr.expr(globals(), locals(),["xmin","+","xLength"]),ymin,expr.expr(globals(), locals(),["ymin","+","yLength"]))
cellMapper = vtk.vtkLabeledDataMapper()
cellMapper.SetInputConnection(visCells.GetOutputPort())
#    cellMapper SetLabelFormat "%g"
#    cellMapper SetLabelModeToLabelScalars
#    cellMapper SetLabelModeToLabelNormals
cellMapper.SetLabelModeToLabelFieldData()
cellMapper.GetLabelTextProperty().SetColor(0,1,0)
cellLabels = vtk.vtkActor2D()
cellLabels.SetMapper(cellMapper)
# Add the actors to the renderer, set the background and size
#
ren1.AddActor(sphereActor)
ren1.AddActor2D(rectActor)
ren1.AddActor2D(pointLabels)
ren1.AddActor2D(cellLabels)
ren1.SetBackground(1,1,1)
renWin.SetSize(500,500)
renWin.Render()
# render the image
#
# prevent the tk window from showing up then start the event loop
def PlaceWindow (xmin,ymin,__vtk__temp0=0,__vtk__temp1=0):
    global xLength, yLength
    xmax = expr.expr(globals(), locals(),["xmin","+","xLength"])
    ymax = expr.expr(globals(), locals(),["ymin","+","yLength"])
    visPts.SetSelection(xmin,xmax,ymin,ymax)
    visCells.SetSelection(xmin,xmax,ymin,ymax)
    pts.InsertPoint(0,xmin,ymin,0)
    pts.InsertPoint(1,xmax,ymin,0)
    pts.InsertPoint(2,xmax,ymax,0)
    pts.InsertPoint(3,xmin,ymax,0)
    pts.Modified()
    #because insertions don't modify object - performance reasons
    renWin.Render()

def MoveWindow (__vtk__temp0=0,__vtk__temp1=0):
    y = 100
    while y < 300:
        x = 100
        while x < 300:
            PlaceWindow(x,y)
            x = x + 25

        y = y + 25


MoveWindow()
PlaceWindow(xmin,ymin)
# --- end of script --
