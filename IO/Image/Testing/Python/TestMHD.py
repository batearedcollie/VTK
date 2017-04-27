#!/usr/bin/env python
import vtk
from vtk.util.misc import vtkGetDataRoot
VTK_DATA_ROOT = vtkGetDataRoot()

Ren1 = vtk.vtkRenderer()
Ren1.SetBackground(0.33,0.35,0.43)
renWin = vtk.vtkRenderWindow()
renWin.AddRenderer(Ren1)
renWin.SetSize(300,300)
iren = vtk.vtkRenderWindowInteractor()
iren.SetRenderWindow(renWin)
reader = vtk.vtkImageReader()
reader.SetDataByteOrderToLittleEndian()
reader.SetDataExtent(0,63,0,63,1,93)
reader.SetDataSpacing(3.2,3.2,1.5)
reader.SetFilePrefix("" + str(VTK_DATA_ROOT) + "/Data/headsq/quarter")
reader.SetDataMask(0x7fff)
reader.Update()
pvTemp200 = vtk.vtkMetaImageWriter()
pvTemp200.SetFileName("mhdWriter.mhd")
pvTemp200.SetInputData(reader.GetOutput())
pvTemp200.Write()
pvTemp90 = vtk.vtkMetaImageReader()
pvTemp90.SetFileName("mhdWriter.mhd")
pvTemp90.Update()
pvTemp109 = vtk.vtkLookupTable()
pvTemp109.SetNumberOfTableValues(256)
pvTemp109.SetHueRange(0.6667,0)
pvTemp109.SetSaturationRange(1,1)
pvTemp109.SetValueRange(1,1)
pvTemp109.SetTableRange(37.3531,260)
pvTemp109.SetVectorComponent(0)
pvTemp109.Build()
pvTemp110 = vtk.vtkContourFilter()
pvTemp110.SetInputData(pvTemp90.GetOutput(0))
pvTemp110.SetValue(0,1150)
pvTemp110.SetComputeNormals(1)
pvTemp110.SetComputeGradients(0)
pvTemp110.SetComputeScalars(0)
pvTemp114 = vtk.vtkPolyDataMapper()
pvTemp114.SetInputConnection(pvTemp110.GetOutputPort())
pvTemp114.SetImmediateModeRendering(1)
pvTemp114.SetScalarRange(0,1)
pvTemp114.UseLookupTableScalarRangeOn()
pvTemp114.SetScalarVisibility(1)
pvTemp114.SetScalarModeToUsePointFieldData()
pvTemp114.SelectColorArray("ImageFile")
pvTemp114.SetLookupTable(pvTemp109)
pvTemp115 = vtk.vtkActor()
pvTemp115.SetMapper(pvTemp114)
pvTemp115.GetProperty().SetRepresentationToSurface()
pvTemp115.GetProperty().SetInterpolationToGouraud()
pvTemp115.GetProperty().SetAmbient(0)
pvTemp115.GetProperty().SetDiffuse(1)
pvTemp115.GetProperty().SetSpecular(0)
pvTemp115.GetProperty().SetSpecularPower(1)
pvTemp115.GetProperty().SetSpecularColor(1,1,1)
Ren1.AddActor(pvTemp115)
iren.Initialize()
# prevent the tk window from showing up then start the event loop
# --- end of script --
