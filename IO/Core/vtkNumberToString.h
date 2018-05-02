/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkNumberToString.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkNumberToString
 * @brief Convert floating and fixed point numbers to strings
 *
 * This class uses the double-conversion library to convert floating point and
 * fixed point numbers to ASCII versions that are represented without
 * numerical precision errors.
 *
 * Typical use:
 *
 * @code{cpp}
 *  #include "vtkNumberToString.h"
 *  vtkNumberToString convert;
 *  float a = 1.0f/3.0f;
 *  std::cout << convert(a) << std::endl;
 * @endcode
 */
#ifndef vtkNumberToString_h
#define vtkNumberToString_h

#include "vtkIOCoreModule.h" // For export macro
#include "vtkTypeTraits.h"

#include <ostream>
#include <string>

class VTKIOCORE_EXPORT vtkNumberToString
{
public:
  struct TagDouble
  {
    const double& Ref;
    TagDouble(const double& ref)
      : Ref(ref)
    {
    }
  };

  struct TagFloat
  {
    const float& Ref;
    TagFloat(const float& ref)
      : Ref(ref)
    {
    }
  };

  template <typename T>
  const T& operator()(const T& val) const
  {
    return val;
  }
  TagDouble operator()(const double& val) const { return TagDouble(val); }
  TagFloat operator()(const float& val) const { return TagFloat(val); }

private:
  void operator=(const vtkNumberToString&) = delete;
};

VTKIOCORE_EXPORT ostream& operator<<(ostream& stream, const vtkNumberToString::TagDouble& tag);
VTKIOCORE_EXPORT ostream& operator<<(ostream& stream, const vtkNumberToString::TagFloat& tag);

#endif
// VTK-HeaderTest-Exclude: vtkNumberToString.h
