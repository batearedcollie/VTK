//=============================================================================
//
//  Copyright (c) Kitware, Inc.
//  All rights reserved.
//  See LICENSE.txt for details.
//
//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.  See the above copyright notice for more information.
//
//  Copyright 2012 Sandia Corporation.
//  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
//  the U.S. Government retains certain rights in this software.
//
//=============================================================================

#ifndef vtkToDax_MarchingCubes_h
#define vtkToDax_MarchingCubes_h

#include "vtkDispatcher.h"
#include "vtkPolyData.h"

#include "DataSetTypeToType.h"
#include "CellTypeToType.h"
#include "DataSetConverters.h"
#include "daxToVtk/CellTypeToType.h"
#include "daxToVtk/DataSetConverters.h"

#include <dax/cont/DispatcherGenerateInterpolatedCells.h>
#include <dax/cont/DispatcherMapCell.h>
#include <dax/worklet/MarchingCubes.h>

namespace
{
template <typename T> struct MarchingCubesOuputType
{
  typedef dax::CellTagTriangle type;
};

}

namespace vtkToDax
{
template<typename DispatcherType, int NumComponents>
struct InterpolateEdges
{
  static int Try(DispatcherType &dispatcher,
                 vtkDataArray *inputFieldVTKArray,
                 vtkDataSet *output)
  {
    InterpolateEdges<DispatcherType,NumComponents> interpolator =
        InterpolateEdges<DispatcherType,NumComponents>(dispatcher, output);

    vtkDispatcher<vtkAbstractArray,int> fieldDispatcher;
    fieldDispatcher.Add<vtkFloatArray>(interpolator);
    fieldDispatcher.Add<vtkDoubleArray>(interpolator);
    return fieldDispatcher.Go(inputFieldVTKArray);
  }

  DispatcherType *Dispatcher;
  vtkDataSet *Output;

  InterpolateEdges(DispatcherType &dispatcher,
                   vtkDataSet *output)
    : Dispatcher(&dispatcher), Output(output) {  }

  template<typename InputVTKArrayType>
  int operator()(InputVTKArrayType &inputFieldVTKArray)
  {
    typedef vtkToDax::vtkArrayContainerTag<InputVTKArrayType> ContainerTag;
    typedef typename vtkToDax::FieldTypeToType<InputVTKArrayType,NumComponents>
        ::DaxValueType DaxValueType;
    typedef dax::cont::ArrayHandle<DaxValueType, ContainerTag>
        FieldHandleType;
    typedef typename FieldHandleType::PortalConstControl PortalType;

    if (inputFieldVTKArray.GetNumberOfComponents() != NumComponents)
      {
      return 0;
      }

    FieldHandleType daxOriginalField =
        FieldHandleType(PortalType(&inputFieldVTKArray,
                                   inputFieldVTKArray.GetNumberOfTuples()));

    FieldHandleType daxInterpolatedField;

    this->Dispatcher->CompactPointField(daxOriginalField, daxInterpolatedField);

    daxToVtk::addPointData(this->Output,
                           daxInterpolatedField,
                           inputFieldVTKArray.GetName());

    return 1;
  }
};

template<int B>
struct DoMarchingCubes
{
  template<class InGridType,
           class OutGridType,
           typename ValueType,
           class Container1,
           class Adapter>
  int operator()(const InGridType &,
                 vtkDataSet *,
                 OutGridType &,
                 vtkPolyData *,
                 ValueType,
                 const dax::cont::ArrayHandle<ValueType,Container1,Adapter> &,
                 bool)
  {
    return 0;
  }
};
template<>
struct DoMarchingCubes<1>
{
  template<class InGridType,
           class OutGridType,
           typename ValueType,
           class Container1,
           class Adapter>
  int operator()(
      const InGridType &inDaxGrid,
      vtkDataSet *inVTKGrid,
      OutGridType &outDaxGeom,
      vtkPolyData *outVTKGrid,
      ValueType isoValue,
      const dax::cont::ArrayHandle<ValueType,Container1,Adapter> &mcHandle,
      bool computeScalars)
  {
    int result=1;

    dax::Scalar isoValueT(isoValue);

    try
      {

      typedef dax::cont::DispatcherGenerateInterpolatedCells<
                  dax::worklet::MarchingCubesGenerate,
                  dax::cont::ArrayHandle< dax::Id >,
                  Adapter >                             DispatchIC;

      typedef typename DispatchIC::CountHandleType CountHandleType;

      dax::worklet::MarchingCubesCount countWorklet(isoValueT);
      dax::cont::DispatcherMapCell<
                            dax::worklet::MarchingCubesCount,
                            Adapter>       dispatchCount( countWorklet );

      CountHandleType count;
      dispatchCount.Invoke(inDaxGrid, mcHandle, count);


      dax::worklet::MarchingCubesGenerate generateWorklet(isoValueT);
      DispatchIC generateSurface(count, generateWorklet);
      generateSurface.SetRemoveDuplicatePoints(true);
      generateSurface.Invoke(inDaxGrid,outDaxGeom,mcHandle);

      // Convert output geometry to VTK.
      daxToVtk::dataSetConverter(outDaxGeom, outVTKGrid);

      // Interpolate arrays where possible.
      if (computeScalars)
        {
        vtkPointData *pd = inVTKGrid->GetPointData();
        for (int arrayIndex = 0;
             arrayIndex < pd->GetNumberOfArrays();
             arrayIndex++)
          {
          vtkDataArray *array = pd->GetArray(arrayIndex);
          if (array == NULL) { continue; }

          InterpolateEdges<DispatchIC,1>::Try(generateSurface, array, outVTKGrid);
          InterpolateEdges<DispatchIC,2>::Try(generateSurface, array, outVTKGrid);
          InterpolateEdges<DispatchIC,3>::Try(generateSurface, array, outVTKGrid);
          InterpolateEdges<DispatchIC,4>::Try(generateSurface, array, outVTKGrid);
          }

        // Pass information about attributes.
        for (int attributeType = 0;
             attributeType < vtkDataSetAttributes::NUM_ATTRIBUTES;
             attributeType++)
          {
          vtkDataArray *attribute = pd->GetAttribute(attributeType);
          if (attribute == NULL) { continue; }
          outVTKGrid->GetPointData()->SetActiveAttribute(attribute->GetName(),
                                                         attributeType);
          }
        } //computeScalars
      }
    catch(dax::cont::ErrorControlOutOfMemory error)
      {
      std::cerr << "Ran out of memory trying to use the GPU" << std::endl;
      std::cerr << error.GetMessage() << std::endl;
      result = 0;
      }
    catch(dax::cont::ErrorExecution error)
      {
      std::cerr << "Got ErrorExecution from Dax." << std::endl;
      std::cerr << error.GetMessage() << std::endl;
      result = 0;
      }
    return result;
  }
};

  template<typename FieldType_>
  struct MarchingCubes
  {
    public:
    typedef FieldType_ FieldType;
    //we expect FieldType_ to be an dax::cont::ArrayHandle
    typedef typename FieldType::ValueType T;

    MarchingCubes(const FieldType& f, T value, bool computeScalars):
      Result(NULL),
      Field(f),
      Value(value),
      ComputeScalars(computeScalars),
      Name()
      {
      }

    void setOutputGrid(vtkPolyData* grid)
      {
      this->Result=grid;
      }

    void setFieldName(const char* name)
      {
      this->Name=std::string(name);
      }

    template<typename LHS, typename RHS>
    int operator()(LHS &dataSet, const RHS&) const
      {
      typedef CellTypeToType<RHS> VTKCellTypeStruct;
      typedef DataSetTypeToType<CellTypeToType<RHS>,LHS> DataSetTypeToTypeStruct;

      //get the mapped output type of this operation(MarchingCubes)
      //todo make this a typedef on the MarchingCubes
      typedef typename MarchingCubesOuputType< typename VTKCellTypeStruct::DaxCellType >::type OutCellType;

      //get the input dataset type
      typedef typename DataSetTypeToTypeStruct::DaxDataSetType InputDataSetType;

      //construct the output grid type to use the vtk containers
      //as we know we are going back to vtk. In a more general framework
      //we would want a tag to say what the destination container tag types
      //are. We don't need the points container be
      typedef daxToVtk::CellTypeToType<OutCellType> VTKCellType;
      dax::cont::UnstructuredGrid<OutCellType,
                 vtkToDax::vtkTopologyContainerTag<VTKCellType>,
                 vtkToDax::vtkPointsContainerTag > resultGrid;

      InputDataSetType inputDaxData = vtkToDax::dataSetConverter(&dataSet,
                                                     DataSetTypeToTypeStruct());

      vtkToDax::DoMarchingCubes<DataSetTypeToTypeStruct::Valid> mc;
      int result = mc(inputDaxData,
                      &dataSet,
                      resultGrid,
                      this->Result,
                      this->Value,
                      this->Field,
                      this->ComputeScalars);

      return result;
      }
  private:
    vtkPolyData* Result;
    FieldType Field;
    T Value;
    bool ComputeScalars;
    std::string Name;

  };
}

#endif //vtkToDax_MarchingCubes_h
