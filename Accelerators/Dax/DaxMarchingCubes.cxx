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

//really should be in UseDaxCuda.h
#define BOOST_SP_DISABLE_THREADS
#include <dax/cuda/cont/DeviceAdapterCuda.h>

#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkNew.h>

//fields we support
#include <vtkFloatArray.h>

#include <vtkDoubleArray.h>

//cell types we support
#include <vtkCellTypes.h>
#include <vtkGenericCell.h>
#include <vtkTriangle.h>
#include <vtkVoxel.h>

//datasets we support
#include <vtkDataObjectTypes.h>
#include <vtkImageData.h>
#include <vtkUniformGrid.h>
#include <vtkUnstructuredGrid.h>

//helpers that convert vtk to dax
#include "vtkDispatcher.h"
#include "vtkDoubleDispatcher.h"

#include "vtkToDax/Portals.h"
#include "vtkToDax/Containers.h"
#include "vtkToDax/CellTypeToType.h"
#include "vtkToDax/DataSetTypeToType.h"
#include "vtkToDax/FieldTypeToType.h"
#include "vtkToDax/MarchingCubes.h"



namespace vtkDax { namespace detail {
  struct CellTypeInDataSet
    {
    explicit CellTypeInDataSet(int cellType):
      Cell(vtkGenericCell::InstantiateCell(cellType)){}
    ~CellTypeInDataSet(){this->Cell->Delete();}
    vtkCell* Cell;
    };

  //returns if a dataset can be used from within Dax
  CellTypeInDataSet cellType(vtkDataSet* input)
  {
    //determine the cell types that the dataset has
    vtkNew<vtkCellTypes> cellTypes;
    input->GetCellTypes(cellTypes.GetPointer());

    if(cellTypes->GetNumberOfTypes() > 1)
      {
      //we currently only support a single cell type
      return CellTypeInDataSet(VTK_EMPTY_CELL);
      }

    return CellTypeInDataSet(cellTypes->GetCellType(0));
  }

  struct ValidMarchingCubesInput
  {
    typedef int ReturnType;
    vtkDataSet* Input;
    vtkCell* Cell;
    double Value;

    vtkUnstructuredGrid* Result;

    ValidMarchingCubesInput(vtkDataSet* in, vtkUnstructuredGrid* out,
                        vtkCell* cell, double value):
      Input(in),Cell(cell),Value(value),Result(out){}

    template<typename LHS>
    int operator()(LHS &arrayField) const
      {
      //we can derive the type of the field at compile time, but not the
      //length
      switch(arrayField.GetNumberOfComponents())
        {
          case 1:
            //first we extract the field type of the array
            //second we extract the number of components
            typedef typename vtkToDax::FieldTypeToType<LHS,1>::FieldType FT1;
            return dispatchOnFieldType<LHS,FT1>(arrayField);
          case 2:
            typedef typename vtkToDax::FieldTypeToType<LHS,2>::FieldType FT2;
          return dispatchOnFieldType<LHS,FT2>(arrayField);
          case 3:
            typedef typename vtkToDax::FieldTypeToType<LHS,3>::FieldType FT3;
            return dispatchOnFieldType<LHS,FT3>(arrayField);
          case 4:
            typedef typename vtkToDax::FieldTypeToType<LHS,4>::FieldType FT4;
            return dispatchOnFieldType<LHS,FT4>(arrayField);
        default:
          //currently only support 1 to 4 components
          //we need to make dispatch on field data smarter in that it does
          //this automagically
          return 0;
        }


      }

    template<typename VTKArrayType, typename DaxFieldType>
    int dispatchOnFieldType(VTKArrayType& vtkField) const
      {
      typedef DaxFieldType FieldType;
      typedef vtkToDax::vtkArrayContainerTag<VTKArrayType> FieldTag;
      typedef typename FieldTag::template Portal<FieldType>::Type PortalType;
      typedef dax::cont::ArrayHandle<FieldType,FieldTag> FieldHandle;

      FieldHandle field = FieldHandle( PortalType(&vtkField,
                                            vtkField.GetNumberOfTuples() ) );
      vtkToDax::MarchingCubes<FieldHandle> marching(field,
                                                 FieldType(Value));
      marching.setFieldName(vtkField.GetName());
      marching.setOutputGrid(Result);


      vtkDoubleDispatcher<vtkDataSet,vtkCell,int> dataDispatcher;
      dataDispatcher.Add<vtkImageData,vtkVoxel>(marching);
      dataDispatcher.Add<vtkUniformGrid,vtkVoxel>(marching);
      int validThreshold = dataDispatcher.Go(this->Input,this->Cell);
      return validThreshold;
      }
  private:
    void operator=(const ValidMarchingCubesInput&);
  };
} //namespace detail


//------------------------------------------------------------------------------
int MarchingCubes(vtkDataSet* input, vtkUnstructuredGrid *output,
              vtkDataArray* field, double isoValue)
{
  //we are doing a point threshold now verify we have suitable cells
  //Dax currently supports: hexs,lines,quads,tets,triangles,vertex,voxel,wedge
  //if something a cell that doesn't match that list we punt to the
  //VTK implementation.
  vtkDax::detail::CellTypeInDataSet cType = vtkDax::detail::cellType(input);

  //construct the object that holds all the state needed to do the threshold
  vtkDax::detail::ValidMarchingCubesInput validInput(input,output,cType.Cell,
                                                     isoValue);


  //setup the dispatch to only allow float and int array to go to the next step
  vtkDispatcher<vtkAbstractArray,int> fieldDispatcher;
  fieldDispatcher.Add<vtkFloatArray>(validInput);
  return fieldDispatcher.Go(field);
}

} //end vtkDax namespace
