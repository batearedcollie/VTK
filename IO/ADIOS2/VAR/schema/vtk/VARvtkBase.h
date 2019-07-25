/*=========================================================================

 Program:   Visualization Toolkit
 Module:    VARvtkBase.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

/*
 * VARvtkBase.h : abstract class for schemas of type
 *                  [VTK XML file formats schemas]
 *                  (https://vtk.org/wp-content/uploads/2015/04/file-formats.pdf)
 *                  Provide common functionality.
 *
 *  Created on: May 6, 2019
 *      Author: William F Godoy godoywf@ornl.gov
 */

#ifndef VTK_IO_ADIOS2_VAR_SCHEMA_VTK_VARvtkBase_h
#define VTK_IO_ADIOS2_VAR_SCHEMA_VTK_VARvtkBase_h

#include "VAR/common/VARTypes.h"
#include "VAR/schema/VARSchema.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace var
{
namespace schema
{

class VARvtkBase : public VARSchema
{
public:
  VARvtkBase(
    const std::string type, const std::string& schema, adios2::IO& io, adios2::Engine& engine);

  // can't use = default, due to forward class not defined
  virtual ~VARvtkBase();

protected:
  std::vector<types::Piece> Pieces;

  const static std::set<std::string> TIMENames;
  const static std::set<std::string> SpecialNames;
  const static std::map<types::DataSetType, std::string> DataSetTypes;

  virtual void DoFill(vtkMultiBlockDataSet* multiBlock, const size_t step) = 0;
  virtual void ReadPiece(const size_t step, const size_t pieceID) = 0;

  bool ReadDataSets(const types::DataSetType type, const size_t step, const size_t pieceID,
    const std::string& hint);

  virtual void Init() = 0;
  void InitTimes() final;

  std::string DataSetType(const types::DataSetType type) const noexcept;
};

} // end namespace schema
} // end namespace var

#endif /* VTK_IO_ADIOS2_VAR_SCHEMA_VTK_VARvtkBase_h */
