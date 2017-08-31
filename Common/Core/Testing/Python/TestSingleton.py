"""Test support of VTK singleton objects

Created on Aug 30, 2017 by David Gobbi

"""

import sys
import vtk
from vtk.test import Testing

class TestSingleton(Testing.vtkTest):
    def testOutputWindow(self):
        a = vtk.vtkOutputWindow()
        b = vtk.vtkOutputWindow()
        self.assertIs(a, b)

    def testObject(self):
        a = vtk.vtkObject()
        b = vtk.vtkObject()
        self.assertNotEqual(a, b)

if __name__ == "__main__":
    Testing.main([(TestSingleton, 'test')])
