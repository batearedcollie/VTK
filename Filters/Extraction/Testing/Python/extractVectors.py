#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# Create the RenderWindow, Renderer and both Actors
#
ren1 = vtk.vtkRenderer()
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(ren1)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
# create pipeline
#
pl3d = vtk.vtkMultiBlockPLOT3DReader()
pl3d.SetXYZFileName("" + str(VTK_DATA_ROOT) + "/Data/combxyz.bin")
pl3d.SetQFileName("" + str(VTK_DATA_ROOT) + "/Data/combq.bin")
pl3d.SetScalarFunctionNumber(100)
pl3d.SetVectorFunctionNumber(202)
pl3d.Update()
output = pl3d.GetOutput().GetBlock(0)
vx = vtk.vtkExtractVectorComponents()
vx.SetInputData(output)
vx.Update()
isoVx = vtk.vtkContourFilter()
isoVx.SetInputData(vx.GetVxComponent())
isoVx.SetValue(0,.38)
normalsVx = vtk.vtkPolyDataNormals()
normalsVx.SetInputConnection(isoVx.GetOutputPort())
normalsVx.SetFeatureAngle(45)
isoVxMapper = vtk.vtkPolyDataMapper()
isoVxMapper.SetInputConnection(normalsVx.GetOutputPort())
isoVxMapper.ScalarVisibilityOff()
isoVxMapper.ImmediateModeRenderingOn()
isoVxActor = vtk.vtkActor()
isoVxActor.SetMapper(isoVxMapper)
isoVxActor.GetProperty().SetColor(1,0.7,0.6)
vy = vtk.vtkExtractVectorComponents()
vy.SetInputData(output)
vy.Update()
isoVy = vtk.vtkContourFilter()
isoVy.SetInputData(vy.GetVyComponent())
isoVy.SetValue(0,.38)
normalsVy = vtk.vtkPolyDataNormals()
normalsVy.SetInputConnection(isoVy.GetOutputPort())
normalsVy.SetFeatureAngle(45)
isoVyMapper = vtk.vtkPolyDataMapper()
isoVyMapper.SetInputConnection(normalsVy.GetOutputPort())
isoVyMapper.ScalarVisibilityOff()
isoVyMapper.ImmediateModeRenderingOn()
isoVyActor = vtk.vtkActor()
isoVyActor.SetMapper(isoVyMapper)
isoVyActor.GetProperty().SetColor(0.7,1,0.6)
vz = vtk.vtkExtractVectorComponents()
vz.SetInputData(output)
vz.Update()
isoVz = vtk.vtkContourFilter()
isoVz.SetInputData(vz.GetVzComponent())
isoVz.SetValue(0,.38)
normalsVz = vtk.vtkPolyDataNormals()
normalsVz.SetInputConnection(isoVz.GetOutputPort())
normalsVz.SetFeatureAngle(45)
isoVzMapper = vtk.vtkPolyDataMapper()
isoVzMapper.SetInputConnection(normalsVz.GetOutputPort())
isoVzMapper.ScalarVisibilityOff()
isoVzMapper.ImmediateModeRenderingOn()
isoVzActor = vtk.vtkActor()
isoVzActor.SetMapper(isoVzMapper)
isoVzActor.GetProperty().SetColor(0.4,0.5,1)
outline = vtk.vtkStructuredGridOutlineFilter()
outline.SetInputData(output)
outlineMapper = vtk.vtkPolyDataMapper()
outlineMapper.SetInputConnection(outline.GetOutputPort())
outlineActor = vtk.vtkActor()
outlineActor.SetMapper(outlineMapper)
# Add the actors to the renderer, set the background and size
#
ren1.AddActor(outlineActor)
ren1.AddActor(isoVxActor)
isoVxActor.AddPosition(0,12,0)
ren1.AddActor(isoVyActor)
ren1.AddActor(isoVzActor)
isoVzActor.AddPosition(0,-12,0)
ren1.SetBackground(.8,.8,.8)
renWin.SetSize(321,321)
ren1.GetActiveCamera().SetPosition(-63.3093,-1.55444,64.3922)
ren1.GetActiveCamera().SetFocalPoint(8.255,0.0499763,29.7631)
ren1.GetActiveCamera().SetViewAngle(30)
ren1.GetActiveCamera().SetViewUp(0,0,1)
ren1.ResetCameraClippingRange()
# render the image
#
renWin.Render()
# prevent the tk window from showing up then start the event loop
# --- end of script --
