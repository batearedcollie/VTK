# This example demonstrates the use and manipulation of lookup tables.

#
# First we include the VTK Tcl packages which will make available
# all of the vtk commands from Tcl. The vtkinteraction package defines
# a simple Tcl/Tk interactor widget. The vtktesting package includes a
# predefined set of colors.
#
package require vtk
package require vtkinteraction
package require vtktesting

# First create pipeline a simple pipeline that reads a structure grid
# and then extracts a plane from the grid. The plane will be colored
# differently by using different lookup tables.
#
# Note: the Update method is manually invoked because it causes the
# reader to read; later on we use the output of the reader to set
# a range for the scalar values.
vtkMultiBlockPLOT3DReader pl3d
    pl3d SetXYZFileName "$VTK_DATA_ROOT/Data/combxyz.bin"
    pl3d SetQFileName "$VTK_DATA_ROOT/Data/combq.bin"
    pl3d SetScalarFunctionNumber 100
    pl3d SetVectorFunctionNumber 202
    pl3d Update

set pl3dOutput [[pl3d GetOutput] GetBlock 0]

vtkStructuredGridGeometryFilter plane
    plane SetInputData $pl3dOutput
    plane SetExtent 1 100 1 100 7 7
vtkLookupTable lut
vtkPolyDataMapper planeMapper
    planeMapper SetLookupTable lut
    planeMapper SetInputConnection [plane GetOutputPort]
    eval planeMapper SetScalarRange [$pl3dOutput GetScalarRange]
vtkActor planeActor
    planeActor SetMapper planeMapper

# This creates an outline around the data.
vtkStructuredGridOutlineFilter outline
    outline SetInputData $pl3dOutput
vtkPolyDataMapper outlineMapper
    outlineMapper SetInputConnection [outline GetOutputPort]
vtkActor outlineActor
    outlineActor SetMapper outlineMapper

# Much of the following is commented out. To try different lookup tables,
# uncommented the appropriate portions.
#

# This creates a black to white lut.
#    lut SetHueRange 0 0
#    lut SetSaturationRange 0 0
#    lut SetValueRange 0.2 1.0

# This creates a red to blue lut.
#    lut SetHueRange 0.0 0.667

# This creates a blue to red lut.
#    lut SetHueRange 0.667 0.0

# This creates a wierd effect. The Build() method causes the lookup table
# to allocate memory and create a table based on the currect hue, saturation,
# value, and alpha (transparency) range. Here we then manually overwrite the
# values generated by the Build() method.
lut SetNumberOfColors 256
lut Build
for {set i 0} {$i<16} {incr i 1} {
    eval lut SetTableValue [expr $i*16] $red 1
    eval lut SetTableValue [expr $i*16+1] $green 1
    eval lut SetTableValue [expr $i*16+2] $blue 1
    eval lut SetTableValue [expr $i*16+3] $black 1
}

# Create the RenderWindow, Renderer and both Actors
#
vtkRenderer ren1
vtkRenderWindow renWin
    renWin AddRenderer ren1
vtkRenderWindowInteractor iren
    iren SetRenderWindow renWin

# Add the actors to the renderer, set the background and size
#
ren1 AddActor outlineActor
ren1 AddActor planeActor

ren1 SetBackground 0.1 0.2 0.4
ren1 TwoSidedLightingOff

renWin SetSize 250 250

iren Initialize

set cam1 [ren1 GetActiveCamera]
$cam1 SetClippingRange 3.95297 50
$cam1 SetFocalPoint 8.88908 0.595038 29.3342
$cam1 SetPosition -12.3332 31.7479 41.2387
$cam1 SetViewUp 0.060772 -0.319905 0.945498

iren AddObserver UserEvent {wm deiconify .vtkInteract}

# prevent the tk window from showing up then start the event loop
wm withdraw .



