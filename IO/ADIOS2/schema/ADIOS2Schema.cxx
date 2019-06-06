/*=========================================================================

 Program:   Visualization Toolkit
 Module:    ADIOS2Schema.cxx

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

/*
 * ADIOS2Schema.cxx
 *
 *  Created on: May 6, 2019
 *      Author: William F Godoy godoywf@ornl.gov
 */

#include "ADIOS2Schema.h"
#include "ADIOS2Schema.txx"

#include "ADIOS2Types.h"

namespace adios2vtk
{

ADIOS2Schema::ADIOS2Schema(
  const std::string type, const std::string& schema, adios2::IO& io, adios2::Engine& engine)
  : Type(type)
  , Schema(schema)
  , IO(io)
  , Engine(engine)
{
}

ADIOS2Schema::~ADIOS2Schema() {}

void ADIOS2Schema::Fill(vtkMultiBlockDataSet* multiBlock, const size_t step)
{
  DoFill(multiBlock, step);
}

void ADIOS2Schema::GetDataArray(const std::string& variableName, types::DataArray& dataArray,
  const size_t step, const std::string mode)
{
  const std::string type = this->IO.VariableType(variableName);

  if (type.empty())
  {
  }
#define declare_type(T)                                                                            \
  else if (type == adios2::GetType<T>())                                                           \
  {                                                                                                \
    adios2::Variable<T> variable = this->IO.InquireVariable<T>(variableName);                      \
    GetDataArrayCommon<T>(variable, dataArray, step, mode);                                        \
  }
  ADIOS2_VTK_ARRAY_TYPE(declare_type)
#undef declare_type
}

void ADIOS2Schema::GetTimes(const std::string& variableName)
{
  if (variableName.empty())
  {
    // set default steps as "timesteps"
    const size_t steps = this->Engine.Steps();
    for (size_t step = 0; step < steps; ++step)
    {
      const double timeDbl = static_cast<double>(step);
      this->Times[timeDbl] = step;
    }
    return;
  }

  // if variable is found
  const std::string type = this->IO.VariableType(variableName);

  if (type.empty())
  {
    throw std::invalid_argument("ERROR: time variable " + variableName + " not present " +
      " in Engine " + this->Engine.Name() + " when reading time data\n");
  }
#define declare_type(T)                                                                            \
  else if (type == adios2::GetType<T>()) { GetTimesCommon<T>(variableName); }
  ADIOS2_VTK_TIME_TYPE(declare_type)
#undef declare_type
}

} // end namespace adios2vtk
