#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

# A script to test the vtkROIStencilSource
reader = vtk.vtkPNGReader()
reader.SetDataSpacing(0.8,0.8,1.5)
reader.SetDataOrigin(0.0,0.0,0.0)
reader.SetFileName("" + str(VTK_DATA_ROOT) + "/Data/fullhead15.png")
shiftScale = vtk.vtkImageShiftScale()
shiftScale.SetInputConnection(reader.GetOutputPort())
shiftScale.SetScale(0.2)
shiftScale.Update()
roiStencil1 = vtk.vtkROIStencilSource()
roiStencil1.SetShapeToEllipsoid()
roiStencil1.SetBounds(20,300,80,150,0,0)
roiStencil1.SetInformationInput(reader.GetOutput())
roiStencil2 = vtk.vtkROIStencilSource()
roiStencil2.SetShapeToCylinderX()
roiStencil2.SetBounds(20,300,80,150,0,0)
roiStencil2.SetInformationInput(reader.GetOutput())
roiStencil3 = vtk.vtkROIStencilSource()
roiStencil3.SetShapeToCylinderZ()
roiStencil3.SetBounds(20,300,80,150,0,0)
roiStencil3.SetInformationInput(reader.GetOutput())
roiStencil4 = vtk.vtkROIStencilSource()
roiStencil4.SetShapeToBox()
roiStencil4.SetBounds(20,300,80,150,0,0)
roiStencil4.SetInformationInput(reader.GetOutput())
stencil1 = vtk.vtkImageStencil()
stencil1.SetInputConnection(reader.GetOutputPort())
stencil1.SetBackgroundInputData(shiftScale.GetOutput())
stencil1.SetStencilConnection(roiStencil1.GetOutputPort())
stencil2 = vtk.vtkImageStencil()
stencil2.SetInputConnection(reader.GetOutputPort())
stencil2.SetBackgroundInputData(shiftScale.GetOutput())
stencil2.SetStencilConnection(roiStencil2.GetOutputPort())
stencil3 = vtk.vtkImageStencil()
stencil3.SetInputConnection(reader.GetOutputPort())
stencil3.SetBackgroundInputData(shiftScale.GetOutput())
stencil3.SetStencilConnection(roiStencil3.GetOutputPort())
stencil4 = vtk.vtkImageStencil()
stencil4.SetInputConnection(reader.GetOutputPort())
stencil4.SetBackgroundInputData(shiftScale.GetOutput())
stencil4.SetStencilConnection(roiStencil4.GetOutputPort())
mapper1 = vtk.vtkImageMapper()
mapper1.SetInputConnection(stencil1.GetOutputPort())
mapper1.SetColorWindow(2000)
mapper1.SetColorLevel(1000)
mapper1.SetZSlice(0)
mapper2 = vtk.vtkImageMapper()
mapper2.SetInputConnection(stencil2.GetOutputPort())
mapper2.SetColorWindow(2000)
mapper2.SetColorLevel(1000)
mapper2.SetZSlice(0)
mapper3 = vtk.vtkImageMapper()
mapper3.SetInputConnection(stencil3.GetOutputPort())
mapper3.SetColorWindow(2000)
mapper3.SetColorLevel(1000)
mapper3.SetZSlice(0)
mapper4 = vtk.vtkImageMapper()
mapper4.SetInputConnection(stencil4.GetOutputPort())
mapper4.SetColorWindow(2000)
mapper4.SetColorLevel(1000)
mapper4.SetZSlice(0)
actor1 = vtk.vtkActor2D()
actor1.SetMapper(mapper1)
actor2 = vtk.vtkActor2D()
actor2.SetMapper(mapper2)
actor3 = vtk.vtkActor2D()
actor3.SetMapper(mapper3)
actor4 = vtk.vtkActor2D()
actor4.SetMapper(mapper4)
imager1 = vtk.vtkRenderer()
imager1.AddActor2D(actor1)
imager1.SetViewport(0.5,0.0,1.0,0.5)
imager2 = vtk.vtkRenderer()
imager2.AddActor2D(actor2)
imager2.SetViewport(0.0,0.0,0.5,0.5)
imager3 = vtk.vtkRenderer()
imager3.AddActor2D(actor3)
imager3.SetViewport(0.5,0.5,1.0,1.0)
imager4 = vtk.vtkRenderer()
imager4.AddActor2D(actor4)
imager4.SetViewport(0.0,0.5,0.5,1.0)
imgWin = vtk.vtkRenderWindow()
imgWin.AddRenderer(imager1)
imgWin.AddRenderer(imager2)
imgWin.AddRenderer(imager3)
imgWin.AddRenderer(imager4)
imgWin.SetSize(512,512)
imgWin.Render()
# --- end of script --
