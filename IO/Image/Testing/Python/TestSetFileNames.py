#!/usr/bin/env python

globFileNames = vtk.vtkGlobFileNames()
globFileNames.AddFileNames("" + str(VTK_DATA_ROOT) + "/Data/headsq/quarter.*")
sortFileNames = vtk.vtkSortFileNames()
sortFileNames.SetInputFileNames(globFileNames.GetFileNames())
sortFileNames.NumericSortOn()
reader = vtk.vtkImageReader2()
reader.SetFileNames(sortFileNames.GetFileNames())
reader.SetDataExtent(0,63,0,63,1,1)
reader.SetDataByteOrderToLittleEndian()
# set Z slice to 2: if output is not numerically sorted, the wrong
# slice will be shown
viewer = vtk.vtkImageViewer()
viewer.SetInputConnection(reader.GetOutputPort())
viewer.SetZSlice(2)
viewer.SetColorWindow(2000)
viewer.SetColorLevel(1000)
viewer.GetRenderer().SetBackground(0,0,0)
viewer.Render()
del reader
# --- end of script --
