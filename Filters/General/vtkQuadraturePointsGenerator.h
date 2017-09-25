/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkQuadraturePointsGenerator.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkQuadraturePointsGenerator
 *
 *
 * Create a vtkPolyData on its output containing the vertices
 * for the quadrature points for one of the vtkDataArrays present
 * on its input vtkUnstructuredGrid. If the input data set has
 * has FieldData generated by vtkQuadraturePointInterpolator then
 * this will be set as point data. Note: Point sets are generated
 * per field array. This is because each field array may contain
 * its own dictionary.
 *
 * @sa
 * vtkQuadraturePointInterpolator, vtkQuadratureSchemeDefinition, vtkInformationQuadratureSchemeDefinitionVectorKey
*/

#ifndef vtkQuadraturePointsGenerator_h
#define vtkQuadraturePointsGenerator_h

#include "vtkFiltersGeneralModule.h" // For export macro
#include "vtkPolyDataAlgorithm.h"

class vtkPolyData;
class vtkUnstructuredGrid;
class vtkInformation;
class vtkInformationVector;

class VTKFILTERSGENERAL_EXPORT vtkQuadraturePointsGenerator : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkQuadraturePointsGenerator,vtkPolyDataAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkQuadraturePointsGenerator *New();

protected:
  int FillInputPortInformation(int port, vtkInformation *info) override;

  int RequestData(vtkInformation *req, vtkInformationVector **input, vtkInformationVector *output) override;

  /**
   * Generate the point set .
   */
  int Generate(vtkUnstructuredGrid *usgIn,
                vtkDataArray* offsets,
                vtkPolyData *pdOut);

  int GenerateField(vtkUnstructuredGrid *usgIn,
                vtkDataArray* data,
                vtkDataArray* offsets,
                vtkPolyData* pdOut);

  vtkQuadraturePointsGenerator();
  ~vtkQuadraturePointsGenerator() override;
private:
  vtkQuadraturePointsGenerator(const vtkQuadraturePointsGenerator &) VTK_DELETE_FUNCTION;
  void operator=(const vtkQuadraturePointsGenerator &) VTK_DELETE_FUNCTION;
};

#endif
