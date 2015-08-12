/*==============================================================================

  Program:   Visualization Toolkit
  Module:    TestDataArrayAPI.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

==============================================================================*/

#include "vtkDataArray.h"

// Helpers:
#include "vtkAoSDataArrayTemplate.h"
#include "vtkIdList.h"
#include "vtkMath.h"
#include "vtkNew.h"
#include "vtkSmartPointer.h"
#include "vtkType.h"
#include "vtkTypeTraits.h"
#include "vtkVariant.h"
#include "vtkVariantCast.h"

#include <algorithm>
#include <cassert>
#include <map>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

// Concrete classes for testing:
#include "vtkAoSDataArrayTemplate.h"
#include "vtkCharArray.h"
#include "vtkFloatArray.h"
#include "vtkDoubleArray.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"
#include "vtkLongArray.h"
#include "vtkLongLongArray.h"
#include "vtkShortArray.h"
#include "vtkSignedCharArray.h"
#include "vtkSoADataArrayTemplate.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnsignedIntArray.h"
#include "vtkUnsignedLongArray.h"
#include "vtkUnsignedLongLongArray.h"
#include "vtkUnsignedShortArray.h"

// About this test:
//
// This test runs a battery of unit tests that exercise the vtkDataArray API
// on concrete implementations of their subclasses. It is designed to be easily
// extended to cover new array implementations and additional unit tests.
//
// This also tests the vtkAbstractArray API. The vtkGenericDataArray API is
// tested in TestGenericDataArrayAPI.
//
// This test has three main components:
// - Entry point: TestDataArrayAPI(). Add new array classes here.
// - Unit test caller: ExerciseDataArray(). Templated on value and array types.
//   Calls individual unit test functions to excerise the array methods.
//   Add new unit test calls here. Note that the ExerciseGetRange runner has
//   been split out to fix compilation on less robust compilers that struggle
//   with the heavy templating (known issue on llvm-gcc 4.2.1).
// - Unit test functions: Test_[methodSignature](). Templated on value type,
//   array type, and possibly other parameters to simplify implementations.
//   These should use the DataArrayAPI macros as needed

// Forward declare the test function:
namespace {
template <typename ScalarT, typename ArrayT>
int ExerciseDataArray();
template <typename ScalarT, typename ArrayT>
int ExerciseGetRange();
} // end anon namespace

//------------------------------------------------------------------------------
//-------------Test Entry Point-------------------------------------------------
//------------------------------------------------------------------------------

int TestDataArrayAPI(int, char *[])
{
  int errors = 0;

#define TEST_ARRAY(scalarT, arrayT) \
  errors += ExerciseDataArray<scalarT, arrayT>(); \
  errors += ExerciseGetRange<scalarT, arrayT>()

  // Add array classes here:
  // Defaults:
  TEST_ARRAY(char,               vtkCharArray);
  TEST_ARRAY(double,             vtkDoubleArray);
  TEST_ARRAY(float,              vtkFloatArray);
  TEST_ARRAY(int,                vtkIntArray);
  TEST_ARRAY(long long,          vtkLongLongArray);
  TEST_ARRAY(long,               vtkLongArray);
  TEST_ARRAY(short,              vtkShortArray);
  TEST_ARRAY(signed char,        vtkSignedCharArray);
  TEST_ARRAY(unsigned char,      vtkUnsignedCharArray);
  TEST_ARRAY(unsigned int,       vtkUnsignedIntArray);
  TEST_ARRAY(unsigned long long, vtkUnsignedLongLongArray);
  TEST_ARRAY(unsigned long,      vtkUnsignedLongArray);
  TEST_ARRAY(unsigned short,     vtkUnsignedShortArray);
  TEST_ARRAY(vtkIdType,          vtkIdTypeArray);

  // Explicit AoS arrays:
  TEST_ARRAY(char,               vtkAoSDataArrayTemplate<char>);
  TEST_ARRAY(double,             vtkAoSDataArrayTemplate<double>);
  TEST_ARRAY(float,              vtkAoSDataArrayTemplate<float>);
  TEST_ARRAY(int,                vtkAoSDataArrayTemplate<int>);
  TEST_ARRAY(long long,          vtkAoSDataArrayTemplate<long long>);
  TEST_ARRAY(long,               vtkAoSDataArrayTemplate<long>);
  TEST_ARRAY(short,              vtkAoSDataArrayTemplate<short>);
  TEST_ARRAY(signed char,        vtkAoSDataArrayTemplate<signed char>);
  TEST_ARRAY(unsigned char,      vtkAoSDataArrayTemplate<unsigned char>);
  TEST_ARRAY(unsigned int,       vtkAoSDataArrayTemplate<unsigned int>);
  TEST_ARRAY(unsigned long long, vtkAoSDataArrayTemplate<unsigned long long>);
  TEST_ARRAY(unsigned long,      vtkAoSDataArrayTemplate<unsigned long>);
  TEST_ARRAY(unsigned short,     vtkAoSDataArrayTemplate<unsigned short>);
  TEST_ARRAY(vtkIdType,          vtkAoSDataArrayTemplate<vtkIdType>);

  // Explicit SoA arrays:
  TEST_ARRAY(char,               vtkSoADataArrayTemplate<char>);
  TEST_ARRAY(double,             vtkSoADataArrayTemplate<double>);
  TEST_ARRAY(float,              vtkSoADataArrayTemplate<float>);
  TEST_ARRAY(int,                vtkSoADataArrayTemplate<int>);
  TEST_ARRAY(long long,          vtkSoADataArrayTemplate<long long>);
  TEST_ARRAY(long,               vtkSoADataArrayTemplate<long>);
  TEST_ARRAY(short,              vtkSoADataArrayTemplate<short>);
  TEST_ARRAY(signed char,        vtkSoADataArrayTemplate<signed char>);
  TEST_ARRAY(unsigned char,      vtkSoADataArrayTemplate<unsigned char>);
  TEST_ARRAY(unsigned int,       vtkSoADataArrayTemplate<unsigned int>);
  TEST_ARRAY(unsigned long long, vtkSoADataArrayTemplate<unsigned long long>);
  TEST_ARRAY(unsigned long,      vtkSoADataArrayTemplate<unsigned long>);
  TEST_ARRAY(unsigned short,     vtkSoADataArrayTemplate<unsigned short>);
  TEST_ARRAY(vtkIdType,          vtkSoADataArrayTemplate<vtkIdType>);

#undef TEST_ARRAY

  if (errors > 0)
    {
    std::cerr << "Test failed! Error count: " << errors << std::endl;
    }

  return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

//------------------------------------------------------------------------------
//------------Unit Test Macros--------------------------------------------------
//------------------------------------------------------------------------------

#define DataArrayAPIInit(_signature) \
  int errors = 0; \
  std::string signature = _signature

#define DataArrayAPIUpdateSignature(_signature) \
  signature = _signature

#define DataArrayAPIFinish() return errors

#define DataArrayAPICreateTestArray(name) vtkNew<ArrayT> name

#define DataArrayAPICreateReferenceArray(name) \
  vtkSmartPointer<vtkDataArray> name##DA = CreateDataArray<ScalarT>(); \
  vtkAoSDataArrayTemplate<ScalarT> *name = \
  vtkAoSDataArrayTemplate<ScalarT>::SafeDownCast(name##DA.GetPointer()); \
  assert("Reference array is vtkAoSDataArrayTemplate" && name != NULL)

#define DataArrayAPICreateReferenceArrayWithType(name, valueType) \
  vtkSmartPointer<vtkDataArray> name##DA = CreateDataArray<valueType>(); \
  vtkAoSDataArrayTemplate<valueType> *name = \
    vtkAoSDataArrayTemplate<valueType>::SafeDownCast(name##DA.GetPointer()); \
  assert("Reference array is vtkAoSDataArrayTemplate" && name != NULL)

#define DataArrayAPINonFatalError(x) \
  { \
    ArrayT *errorTempArray = ArrayT::New(); \
    std::cerr << "Line " << __LINE__ << ": " \
              << "Failure in test of '" << signature << "' " \
              << "for array type '" << errorTempArray->GetClassName() << "'" \
              << ":\n" << x << std::endl; \
    errorTempArray->Delete(); \
    ++errors; \
  }

#define DataArrayAPIError(x) \
  DataArrayAPINonFatalError(x) \
  DataArrayAPIFinish();

#define DataArrayAPIAssert(x, mess) \
  if (x) {} \
  else { DataArrayAPIError("Test assertion '" #x "' failed: " << mess) }

namespace {

// Convenience function to create a concrete data array from a template type:
template <typename ScalarT>
vtkSmartPointer<vtkDataArray> CreateDataArray()
{
  vtkSmartPointer<vtkDataArray> array;
  array.TakeReference(vtkDataArray::CreateDataArray(
                        vtkTypeTraits<ScalarT>::VTK_TYPE_ID));
  assert("CreateArray failed for scalar type." && array.GetPointer());
  return array;
}

//------------------------------------------------------------------------------
//------------------Unit Test Implementations-----------------------------------
//------------------------------------------------------------------------------

//------------------------vtkAbstactArray API-----------------------------------

// int Allocate(vtkIdType numValues, vtkIdType ext=1000)
// Sets MaxId == -1
// Sets Size >= numValues
// If numValues == 0, delete the array.
// ext is not used.
// Result is 1/0 for success/failure.
template <typename ScalarT, typename ArrayT>
int Test_int_Allocate_numValues_ext()
{
  DataArrayAPIInit("int Allocate(vtkIdType numValues, vtkIdType ext)");
  DataArrayAPICreateTestArray(array);

  int success = array->Allocate(0);
  DataArrayAPIAssert(success, "Allocation failed.");
  DataArrayAPIAssert(array->GetSize() == 0,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  success = array->Allocate(100);
  DataArrayAPIAssert(success, "Allocation failed.");
  DataArrayAPIAssert(array->GetSize() >= 100,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  array->SetNumberOfComponents(1);
  array->SetNumberOfTuples(50);
  DataArrayAPIAssert(array->GetMaxId() == 49,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  success = array->Allocate(0);
  DataArrayAPIAssert(success, "Allocation failed.");
  DataArrayAPIAssert(array->GetSize() == 0,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  DataArrayAPIFinish();
}

// void Initialize()
// Release data and reset state.
template <typename ScalarT, typename ArrayT>
int Test_void_Initialize()
{
  DataArrayAPIInit("void Initialize()");
  DataArrayAPICreateTestArray(array);

  array->SetNumberOfComponents(1);
  array->SetNumberOfTuples(50);
  DataArrayAPIAssert(array->GetMaxId() == 49,
                     "Invalid MaxId (" << array->GetMaxId() << ").");
  DataArrayAPIAssert(array->GetSize() >= 50,
                     "Invalid Size (" << array->GetSize() << ").");

  array->Initialize();
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");
  DataArrayAPIAssert(array->GetSize() == 0,
                     "Invalid Size (" << array->GetSize() << ").");

  DataArrayAPIFinish();
}

// int GetDataType()
template <typename ScalarT, typename ArrayT>
int Test_int_GetDataType()
{
  DataArrayAPIInit("int GetDataType()");
  DataArrayAPICreateTestArray(array);

  DataArrayAPIAssert(vtkDataTypesCompare(array->GetDataType(),
                                         vtkTypeTraits<ScalarT>::VTKTypeID()),
                     "Incorrect data type. Expected "
                     << vtkTypeTraits<ScalarT>::VTKTypeID() << ", got "
                     << array->GetDataType() << ".");

  // Special case: vtkIdTypeArray must return VTK_ID_TYPE, not the actual type:
  if (typeid(ArrayT) == typeid(vtkIdTypeArray))
    {
    DataArrayAPIAssert(vtkDataTypesCompare(array->GetDataType(), VTK_ID_TYPE),
                       "Incorrect data type for vtkIdTypeArray. Expected "
                       "VTK_ID_TYPE (" << VTK_ID_TYPE << "), got "
                       << array->GetDataType() << ".");
    }

  DataArrayAPIFinish();
}

// int GetDataTypeSize()
template <typename ScalarT, typename ArrayT>
int Test_int_GetDataTypeSize()
{
  DataArrayAPIInit("int GetDataTypeSize()");
  DataArrayAPICreateTestArray(array);

  DataArrayAPIAssert(array->GetDataTypeSize() == sizeof(ScalarT),
                     "Incorrect data type size. Expected "
                     << sizeof(ScalarT) << ", got "
                     << array->GetDataTypeSize() << ".");

  DataArrayAPIFinish();
}

// int GetElementComponentSize()
// Same as GetDataTypeSize for data arrays:
template <typename ScalarT, typename ArrayT>
int Test_int_GetElementComponentSize()
{
  DataArrayAPIInit("int GetElementComponentSize()");
  DataArrayAPICreateTestArray(array);

  DataArrayAPIAssert(array->GetElementComponentSize() == sizeof(ScalarT),
                     "Incorrect element component size. Expected "
                     << sizeof(ScalarT) << ", got "
                     << array->GetElementComponentSize() << ".");

  DataArrayAPIFinish();
}

// void SetNumberOfComponents(int comps)
// int GetNumberOfComponents()
template <typename ScalarT, typename ArrayT>
int Test_NumberOfComponents()
{
  DataArrayAPIInit("void SetNumberOfComponents(int comps)");
  DataArrayAPICreateTestArray(array);
  DataArrayAPIAssert(array->GetNumberOfComponents() == 1,
                     "Initial number of components expected to be 1, but is "
                     << array->GetNumberOfComponents() << ".");

  array->SetNumberOfComponents(12);
  DataArrayAPIAssert(array->GetNumberOfComponents() == 12,
                     "Number of components expected to be 12, but is "
                     << array->GetNumberOfComponents() << ".");

  DataArrayAPIFinish();
}

// void SetComponentName( vtkIdType component, const char *name )
// const char* GetComponentName( vtkIdType component )
// bool HasAComponentName()
// int CopyComponentNames( vtkAbstractArray *da )
template <typename ScalarT, typename ArrayT>
int Test_ComponentNames()
{
  DataArrayAPIInit("bool HasAComponentName()");
  DataArrayAPICreateTestArray(array);
  DataArrayAPIAssert(!array->HasAComponentName(),
                     "New array has a named components.");

  int comps = 5;
  array->SetNumberOfComponents(comps);

  std::ostringstream str;

  for (vtkIdType c = 0; c < static_cast<vtkIdType>(comps); ++c)
    {
    DataArrayAPIUpdateSignature("const char* GetComponentName(vtkIdType comp)");
    DataArrayAPIAssert(array->GetComponentName(c) == NULL,
                       "New array has a named component.");
    str.str("");
    str << "Component " << c;
    DataArrayAPIUpdateSignature("const char* SetComponentName(vtkIdType comp)");
    array->SetComponentName(c, str.str().c_str());
    DataArrayAPIAssert(str.str() == array->GetComponentName(c),
                       "Component " << c << " name is '"
                       << array->GetComponentName(c) << "', expected "
                       << str.str() << "'.");
    DataArrayAPIUpdateSignature("bool HasAComponentName()");
    DataArrayAPIAssert(array->HasAComponentName(), "Invalid result.");
    }

  DataArrayAPICreateTestArray(copy);
  copy->SetNumberOfComponents(comps);
  DataArrayAPIUpdateSignature("int CopyComponentNames(vtkAbstractArray*)");
  int success = copy->CopyComponentNames(array.GetPointer());

  DataArrayAPIAssert(success != 0,
                     "CopyComponentNames returned " << success << ".");
  DataArrayAPIAssert(array->HasAComponentName(), "Invalid result.");

  for (vtkIdType c = 0; c < static_cast<vtkIdType>(comps); ++c)
    {
    str.str("");
    str << "Component " << c;
    DataArrayAPIAssert(str.str() == copy->GetComponentName(c),
                       "Copied array component " << c << " name is '"
                       << array->GetComponentName(c) << "', expected "
                       << str.str() << "'.");
    }

  DataArrayAPIFinish();
}

// void SetNumberOfTuples(vtkIdType number)
// vtkIdType GetNumberOfTuples()
template <typename ScalarT, typename ArrayT>
int Test_NumberOfTuples()
{
  DataArrayAPIInit("void SetNumberOfTuples(int comps)");
  DataArrayAPICreateTestArray(array);
  DataArrayAPIAssert(array->GetNumberOfTuples() == 0,
                     "Initial number of tuples expected to be 0, but is "
                     << array->GetNumberOfTuples() << ".");

  vtkIdType tuples = 12;
  int comps = 5;
  array->SetNumberOfComponents(comps);
  array->SetNumberOfTuples(tuples);
  DataArrayAPIAssert(array->GetNumberOfTuples() == tuples,
                     "Number of tuples expected to be " << tuples
                     << ", but is " << array->GetNumberOfTuples() << ".");

  DataArrayAPIAssert(array->GetSize() >= tuples * comps,
                     "Size expected to be at least " << tuples * comps
                     << ", but is " << array->GetSize() << ".");

  DataArrayAPIAssert(array->GetMaxId() == tuples * comps - 1,
                     "MaxId expected to be " << tuples * comps - 1
                     << ", but is " << array->GetMaxId() << ".");

  array->SetNumberOfTuples(0);
  DataArrayAPIAssert(array->GetNumberOfTuples() == 0,
                     "Number of tuples expected to be 0, but is "
                     << array->GetNumberOfTuples() << ".");

  DataArrayAPIAssert(array->GetSize() >= 0,
                     "Size expected to be at least 0, but is "
                     << array->GetSize() << ".");

  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "MaxId expected to be " << -1
                     << ", but is " << array->GetMaxId() << ".");

  DataArrayAPIFinish();
}

// void SetNumberOfValues(vtkIdType number)
template <typename ScalarT, typename ArrayT>
int Test_void_SetNumberOfValues_number()
{
  DataArrayAPIInit("void SetNumberOfValues(vtkIdType number)");
  DataArrayAPICreateTestArray(array);
  DataArrayAPIAssert(array->GetNumberOfTuples() == 0,
                     "Initial number of tuples expected to be 0, but is "
                     << array->GetNumberOfTuples() << ".");

  vtkIdType tuples = 12;
  int comps = 5;
  array->SetNumberOfComponents(comps);
  array->SetNumberOfTuples(tuples);
  DataArrayAPIAssert(array->GetNumberOfTuples() == tuples,
                     "Number of tuples expected to be " << tuples
                     << ", but is " << array->GetNumberOfTuples() << ".");

  DataArrayAPIAssert(array->GetSize() >= tuples * comps,
                     "Size expected to be at least " << tuples * comps
                     << ", but is " << array->GetSize() << ".");

  DataArrayAPIAssert(array->GetMaxId() == tuples * comps - 1,
                     "MaxId expected to be " << tuples * comps - 1
                     << ", but is " << array->GetMaxId() << ".");

  array->SetNumberOfTuples(0);
  DataArrayAPIAssert(array->GetNumberOfTuples() == 0,
                     "Number of tuples expected to be 0, but is "
                     << array->GetNumberOfTuples() << ".");

  DataArrayAPIAssert(array->GetSize() >= 0,
                     "Size expected to be at least 0, but is "
                     << array->GetSize() << ".");

  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "MaxId expected to be " << -1
                     << ", but is " << array->GetMaxId() << ".");

  DataArrayAPIFinish();
}

// void SetTuple(vtkIdType i, vtkIdType j, vtkAbstractArray *source);
// Set the tuple at i in this array using tuple j from source.
// Types must match.
// No range checking/allocation.
template <typename ScalarT, typename ArrayT>
int Test_void_SetTuple_i_j_source()
{
  DataArrayAPIInit(
        "void SetTuple(vtkIdType i, vtkIdType j, vtkAbstractArray *source)");

  DataArrayAPICreateTestArray(dest);
  DataArrayAPICreateReferenceArray(source);
  vtkIdType comps = 9;
  vtkIdType tuples = 5;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);

  // Initialize source:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 16));
    }

  // Use SetTuple to populate dest. Tuple ordering is changed according to
  // tupleMap (destTuple = tupleMap[srcTuple])
  vtkIdType tupleMap[5] = {1, 0, 3, 4, 2};
  dest->SetNumberOfComponents(comps);
  dest->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    dest->SetTuple(tupleMap[i], i, source);
    }

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> destTuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTupleValue(i, &srcTuple[0]);
    dest->GetTupleValue(tupleMap[i], &destTuple[0]);
    if (!std::equal(srcTuple.begin(), srcTuple.end(), destTuple.begin()))
      {
      std::ostringstream srcTupleStr;
      std::ostringstream destTupleStr;
      for (int j = 0; j < comps; ++j)
        {
        srcTupleStr << srcTuple[j] << " ";
        destTupleStr << destTuple[j] << " ";
        }
      DataArrayAPIError("Data mismatch at source tuple '" << i << "' and "
                        "destination tuple '" << tupleMap[i] << "':\n"
                        "src: " << srcTupleStr.str() << "\n"
                        "dest: " << destTupleStr.str() << "\n");
      }
    }

  DataArrayAPIFinish();
}

// void InsertTuple(vtkIdType i, vtkIdType j, vtkAbstractArray *source)
// Insert the jth tuple in the source array, at ith location in this array.
// Allocates memory as needed.
template <typename ScalarT, typename ArrayT>
int Test_void_InsertTuple_i_j_source()
{
  DataArrayAPIInit("void InsertTuple(vtkIdType i, vtkIdType j, vtkAbstractArray *source)");

  DataArrayAPICreateTestArray(dest);
  DataArrayAPICreateReferenceArray(source);
  vtkIdType comps = 9;
  vtkIdType tuples = 5;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);

  // Initialize source:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 16));
    }

  // Use InsertTuple to populate dest. Tuple ordering is changed according to
  // tupleMap (destTuple = tupleMap[srcTuple])
  vtkIdType tupleMap[5] = {1, 0, 3, 4, 2};

  // dest is empty -- this call should allocate memory as needed.
  dest->SetNumberOfComponents(comps);
  vtkIdType maxTuple = 0;
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    dest->InsertTuple(tupleMap[t], t, source);
    maxTuple = std::max(maxTuple, tupleMap[t]);
    if (dest->GetSize() < ((maxTuple + 1) * comps))
      {
      DataArrayAPIError("Size should be at least " << (maxTuple * comps)
                        << " values, but is only " << dest->GetSize() << ".");
      }
    if (dest->GetMaxId() != ((maxTuple + 1) * comps) - 1)
      {
      DataArrayAPIError("MaxId should be " << (maxTuple * comps) - 1
                        << ", but is " << dest->GetMaxId() << " instead.");
      }
    }

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> destTuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTupleValue(i, &srcTuple[0]);
    dest->GetTupleValue(tupleMap[i], &destTuple[0]);
    if (!std::equal(srcTuple.begin(), srcTuple.end(), destTuple.begin()))
      {
      std::ostringstream srcTupleStr;
      std::ostringstream destTupleStr;
      for (int j = 0; j < comps; ++j)
        {
        srcTupleStr << srcTuple[j] << " ";
        destTupleStr << destTuple[j] << " ";
        }
      DataArrayAPIError("Data mismatch at source tuple '" << i << "' and "
                        "destination tuple '" << tupleMap[i] << "':\n"
                        "src: " << srcTupleStr.str() << "\n"
                        "dest: " << destTupleStr.str() << "\n");
      }
    }

  DataArrayAPIFinish();
}

// void InsertTuples(vtkIdList *dstIds, vtkIdList *srcIds, vtkAbstractArray* source)
template <typename ScalarT, typename ArrayT>
int Test_void_InsertTuples_dstIds_srcIds_source()
{
  DataArrayAPIInit("void InsertTuples(vtkIdList *dstIds, vtkIdList *srcIds, "
                   "vtkAbstractArray* source)");
  DataArrayAPICreateTestArray(src);
  DataArrayAPICreateTestArray(dst);
  vtkIdType comps = 9;
  vtkIdType tuples = 14;
  src->SetNumberOfComponents(comps);
  src->SetNumberOfTuples(tuples);

  // Initialize source:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    src->SetValue(i, static_cast<ScalarT>(i % 16));
    }

  // Copy 5 tuples from src to dest
  vtkNew<vtkIdList> srcIds;
  vtkNew<vtkIdList> dstIds;

  srcIds->InsertNextId(5);
  dstIds->InsertNextId(0);

  srcIds->InsertNextId(2);
  dstIds->InsertNextId(1);

  srcIds->InsertNextId(4);
  dstIds->InsertNextId(3);

  srcIds->InsertNextId(9);
  dstIds->InsertNextId(2);

  srcIds->InsertNextId(11);
  dstIds->InsertNextId(4);

  // dest is empty -- this call should allocate memory as needed.
  vtkIdType numIds = srcIds->GetNumberOfIds();
  dst->SetNumberOfComponents(comps);
  dst->InsertTuples(dstIds.GetPointer(), srcIds.GetPointer(),
                     src.GetPointer());

  DataArrayAPIAssert(dst->GetNumberOfTuples() == numIds,
                     "Destination array too small! Expected " << numIds
                     << " tuples, got " << dst->GetNumberOfTuples() << ".");

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> dstTuple(comps);
  for (vtkIdType t = 0; t < numIds; ++t)
    {
    vtkIdType srcTupleId = srcIds->GetId(t);
    vtkIdType dstTupleId = dstIds->GetId(t);
    src->GetTupleValue(srcTupleId, &srcTuple[0]);
    dst->GetTupleValue(dstTupleId, &dstTuple[0]);
    DataArrayAPIAssert(std::equal(srcTuple.begin(), srcTuple.end(),
                                  dstTuple.begin()),
                       "Copied tuple does not match input.");
    }

  DataArrayAPIFinish();
}

// void InsertTuples(vtkIdType dstStart, vtkIdType n, vtkIdType srcStart, vtkAbstractArray* source)
template <typename ScalarT, typename ArrayT>
int Test_void_InsertTuples_dstStart_n_srcStart_source()
{
  DataArrayAPIInit("void InsertTuples(vtkIdList *dstIds, vtkIdList *srcIds, "
                   "vtkAbstractArray* source)");
  DataArrayAPICreateTestArray(src);
  DataArrayAPICreateTestArray(dst);
  vtkIdType comps = 9;
  vtkIdType tuples = 14;
  src->SetNumberOfComponents(comps);
  src->SetNumberOfTuples(tuples);

  // Initialize source:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    src->SetValue(i, static_cast<ScalarT>(i % 16));
    }

  // Copy 5 tuples from src to dest
  vtkIdType srcStart = 8;
  vtkIdType n = 5;
  vtkIdType dstStart = 0;

  // dest is empty -- this call should allocate memory as needed.
  dst->SetNumberOfComponents(comps);
  dst->InsertTuples(dstStart, n, srcStart, src.GetPointer());

  DataArrayAPIAssert(dst->GetNumberOfTuples() == n,
                     "Destination array too small! Expected " << n
                     << " tuples, got " << dst->GetNumberOfTuples() << ".");

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> dstTuple(comps);
  for (vtkIdType t = 0; t < n; ++t)
    {
    vtkIdType srcTupleId = srcStart + t;
    vtkIdType dstTupleId = dstStart + t;
    src->GetTupleValue(srcTupleId, &srcTuple[0]);
    dst->GetTupleValue(dstTupleId, &dstTuple[0]);
    DataArrayAPIAssert(std::equal(srcTuple.begin(), srcTuple.end(),
                                  dstTuple.begin()),
                       "Copied tuple does not match input.");
    }

  DataArrayAPIFinish();
}

// vtkIdType InsertNextTuple(vtkIdType j, vtkAbstractArray *source)
// Insert the jth tuple in the source array at the end of this array.
// Allocate memory as needed.
// Return the tuple index of the inserted data.
template <typename ScalarT, typename ArrayT>
int Test_vtkIdType_InsertNextTuple_j_source()
{
  DataArrayAPIInit(
        "vtkIdType InsertNextTuple(vtkIdType j, vtkAbstractArray *source)");

  DataArrayAPICreateTestArray(dest);
  DataArrayAPICreateReferenceArray(source);
  vtkIdType comps = 9;
  vtkIdType tuples = 5;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);

  // Initialize source:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 16));
    }

  // Tuple ordering is changed according to tupleMap via:
  // srcTuple = tupleMap[destTuple]
  vtkIdType tupleMap[5] = {1, 0, 3, 4, 2};

  // dest is empty -- this call should allocate memory as needed.
  dest->SetNumberOfComponents(comps);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    vtkIdType tupleIdx = dest->InsertNextTuple(tupleMap[t], source);
    if (t != tupleIdx)
      {
      DataArrayAPIError("Returned tuple index incorrect. Returned '"
                        << tupleIdx << "', expected '" << t << "'.");
      }
    if (dest->GetSize() < ((t + 1) * comps))
      {
      DataArrayAPIError("Size should be at least " << ((t + 1) * comps)
                        << " values, but is only " << dest->GetSize() << ".");
      }
    if (dest->GetMaxId() != ((t + 1) * comps) - 1)
      {
      DataArrayAPIError("MaxId should be " << ((t + 1) * comps) - 1
                        << ", but is " << dest->GetMaxId() << " instead.");
      }
    }

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> destTuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTupleValue(tupleMap[i], &srcTuple[0]);
    dest->GetTupleValue(i, &destTuple[0]);
    if (!std::equal(srcTuple.begin(), srcTuple.end(), destTuple.begin()))
      {
      std::ostringstream srcTupleStr;
      std::ostringstream destTupleStr;
      for (int j = 0; j < comps; ++j)
        {
        srcTupleStr << srcTuple[j] << " ";
        destTupleStr << destTuple[j] << " ";
        }
      DataArrayAPIError("Data mismatch at source tuple '" << tupleMap[i]
                        << "' and destination tuple '" << i << "':\n"
                        "src: " << srcTupleStr.str() << "\n"
                        "dest: " << destTupleStr.str() << "\n");
      }
    }

  DataArrayAPIFinish();
}


// void GetTuples(vtkIdList *ptIds, vtkAbstractArray *output)
// Output array is preallocated.
template <typename ScalarT, typename ArrayT>
int Test_void_GetTuples_ptIds_output()
{
  DataArrayAPIInit(
        "void GetTuples(vtkIdList *ptIds, vtkAbstractArray *output)");

  DataArrayAPICreateTestArray(source);
  DataArrayAPICreateReferenceArray(output);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Initialize the output array and id array. Grab tuples 1, 4, 7, & 10:
  vtkNew<vtkIdList> ids;
  for (vtkIdType tupleId = 1; tupleId < tuples; tupleId += 3)
    {
    ids->InsertNextId(tupleId);
    }
  output->SetNumberOfComponents(comps);
  output->SetNumberOfTuples(ids->GetNumberOfIds());

  // Test the call:
  source->GetTuples(ids.GetPointer(), output);

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> outTuple(comps);
  for (vtkIdType i = 0; i < ids->GetNumberOfIds(); ++i)
    {
    vtkIdType tupleIdx = ids->GetId(i);
    source->GetTupleValue(tupleIdx, &srcTuple[0]);
    output->GetTupleValue(i, &outTuple[0]);
    if (!std::equal(srcTuple.begin(), srcTuple.end(), outTuple.begin()))
      {
      std::ostringstream srcTupleStr;
      std::ostringstream outTupleStr;
      for (int j = 0; j < comps; ++j)
        {
        srcTupleStr << srcTuple[j] << " ";
        outTupleStr << outTuple[j] << " ";
        }
      DataArrayAPIError("Data mismatch at source tuple '" << tupleIdx
                        << "' and output tuple '" << i << "':\n"
                        "src: " << srcTupleStr.str() << "\n"
                        "dest: " << outTupleStr.str() << "\n");
      }
    }

  DataArrayAPIFinish();
}

// void GetTuples(vtkIdType p1, vtkIdType p2, vtkAbstractArray *output)
// Copies p1 --> p2 *inclusive*.
// Output array must be preallocated.
// void GetTuples(vtkIdList *ptIds, vtkAbstractArray *output)
// Output array is preallocated.
template <typename ScalarT, typename ArrayT>
int Test_void_GetTuples_p1_p2_output()
{
  DataArrayAPIInit(
        "void GetTuples(vtkIdType p1, vtkIdType p2, vtkAbstractArray *output)");

  DataArrayAPICreateTestArray(source);
  DataArrayAPICreateReferenceArray(output);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Initialize the output array. We'll be grabbing tuples 3 through 8.
  vtkIdType p1 = 3;
  vtkIdType p2 = 8;
  vtkIdType outTupleCount = p2 - p1 + 1; // +1 because the range is inclusive.
  output->SetNumberOfComponents(comps);
  output->SetNumberOfTuples(outTupleCount);

  // Test the call:
  source->GetTuples(p1, p2, output);

  // Verify:
  std::vector<ScalarT> srcTuple(comps);
  std::vector<ScalarT> outTuple(comps);
  for (vtkIdType i = p1; i < outTupleCount; ++i)
    {
    vtkIdType tupleIdx = p1 + i;
    source->GetTupleValue(tupleIdx, &srcTuple[0]);
    output->GetTupleValue(i, &outTuple[0]);
    if (!std::equal(srcTuple.begin(), srcTuple.end(), outTuple.begin()))
      {
      std::ostringstream srcTupleStr;
      std::ostringstream outTupleStr;
      for (int j = 0; j < comps; ++j)
        {
        srcTupleStr << srcTuple[j] << " ";
        outTupleStr << outTuple[j] << " ";
        }
      DataArrayAPIError("Data mismatch at source tuple '" << tupleIdx
                        << "' and output tuple '" << i << "':\n"
                        "src: " << srcTupleStr.str() << "\n"
                        "dest: " << outTupleStr.str() << "\n");
      }
    }

  DataArrayAPIFinish();
}

// void *GetVoidPointer(vtkIdType id)
template <typename ScalarT, typename ArrayT>
int Test_voidPtr_GetVoidPointer()
{
  DataArrayAPIInit("void* GetVoidPointer(vtkIdType id)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<ScalarT> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<ScalarT>(((t * comps) + c) % 17));
      }
    source->SetTupleValue(t, &tuple[0]);
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    // Silence the void pointer warnings for these calls
    const char *oldWarning = getenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS");
    setenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS", "1", 1);

    ScalarT *ptr = static_cast<ScalarT*>(source->GetVoidPointer(t * comps));

    // Restore state:
    if (oldWarning)
      {
      setenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS", oldWarning, 1);
      }
    else
      {
      unsetenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS");
      }

    DataArrayAPIAssert(ptr != NULL, "GetVoidPointer returned NULL!");
    for (int c = 0; c < comps; ++c)
      {
      ScalarT test = *ptr++;
      ScalarT ref = static_cast<ScalarT>(((t * comps) + c) % 17);
      DataArrayAPIAssert(test == ref,
                         "Data mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got " << test << ".");
        }
      }


  DataArrayAPIFinish();
}

// void DeepCopy(vtkAbstractArray *aa)
// void DeepCopy(vtkDataArray *da)
// Test copying into and from the target array type.
// Allocates memory as needed.
// ArgT switches between the two overloads.
// OtherT is the type of array that will be copied to/from.
template <typename ScalarT, typename ArrayT, typename ArgT, typename OtherT>
int Test_void_DeepCopy_array()
{
  std::string argTName = (typeid(ArgT) == typeid(vtkAbstractArray)
                          ? "vtkAbstractArray" : "vtkDataArray");
  std::ostringstream sigBuilder;
  sigBuilder << "void DeepCopy(" << argTName << " *array)";
  DataArrayAPIInit(sigBuilder.str());

  std::string testType = vtkTypeTraits<ScalarT>::Name();
  std::string otherType = vtkTypeTraits<OtherT>::Name();

  DataArrayAPICreateTestArray(source);
  DataArrayAPICreateReferenceArrayWithType(middle, OtherT);
  DataArrayAPICreateTestArray(target);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 40;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      source->SetComponent(t, c, static_cast<double>(((t * comps) + c) % 17));
      }
    }

  // Copy to intermediate:
  middle->DeepCopy(static_cast<ArgT*>(source.GetPointer()));

  // Verify intermediate:
  if (middle->GetNumberOfComponents() != comps ||
      middle->GetNumberOfTuples() != tuples)
    {
    DataArrayAPIError("Incorrect size of array after copying from test array "
                      "(scalar type: '" << testType << "') to reference array "
                      "(scalar type: '" << otherType << "'): Expected number "
                      "of (tuples, components): " << "(" << tuples << ", "
                      << comps << "), got (" << middle->GetNumberOfTuples()
                      << ", " << middle->GetNumberOfComponents() << ").");
    }

  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      double ref = ((t * comps) + c) % 17;
      double test = middle->GetComponent(t, c);
      if (ref != test)
        {
        DataArrayAPIError("Data mismatch after copying from test array (scalar "
                          "type: '" << testType << "') to reference array "
                          "(scalar type: '" << otherType << "'): " "Data "
                          "mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got " << test << ".");
        }
      }
    }

  // Copy to final:
  target->DeepCopy(static_cast<ArgT*>(middle));

  // Verify final:
  if (target->GetNumberOfComponents() != comps ||
      target->GetNumberOfTuples() != tuples)
    {
    DataArrayAPIError("Incorrect size of array after copying from reference "
                      "array (scalar type: '" << otherType << "') to test "
                      "array (scalar type: '" << testType << "'): "
                      "Expected number of (tuples, components): "
                      << "(" << tuples << ", " << comps << "), got ("
                      << target->GetNumberOfTuples() << ", "
                      << target->GetNumberOfComponents() << ").");
    }

  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      double ref = ((t * comps) + c) % 17;
      double test = target->GetComponent(t, c);
      if (ref != test)
        {
        DataArrayAPIError("Data mismatch after copying from reference array "
                          "(scalar type: '" << otherType << "') to test array "
                          "(scalar type: '" << testType << "'): " "Data "
                          "mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got " << test << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void InterpolateTuple(vtkIdType i, vtkIdList *ptIndices,
//                       vtkAbstractArray *source, double *weights)
// Sets the ith tuple in this array, using the source data, indices, and weights
// provided.
// Should allocate memory.
template <typename ScalarT, typename ArrayT>
int Test_void_InterpolateTuple_i_indices_source_weights()
{
  DataArrayAPIInit("void InterpolateTuple(vtkIdType i, vtkIdList *ptIndices, "
                   "vtkAbstractArray *source, double *weights)");

  DataArrayAPICreateReferenceArray(source);
  DataArrayAPICreateTestArray(output);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Tuples to interpolate:
  vtkNew<vtkIdList> ids;
  ids->InsertNextId(0);
  ids->InsertNextId(1);
  ids->InsertNextId(5);
  ids->InsertNextId(7);
  ids->InsertNextId(8);
  double weights[] = {0.5, 1.0, 0.25, 1.0, 0.8};

  output->SetNumberOfComponents(comps);

  output->InterpolateTuple(0, ids.GetPointer(), source, weights);

  // Validate result:
  for (int c = 0; c < comps; ++c)
    {
    // Compute component:
    double ref = 0.;
    for (vtkIdType t = 0; t < ids->GetNumberOfIds(); ++t)
      {
      ref += weights[t] * source->GetComponent(ids->GetId(t), c);
      }

    // Clamp to ScalarT range:
    ref = std::max(ref, static_cast<double>(vtkTypeTraits<ScalarT>::Min()));
    ref = std::min(ref, static_cast<double>(vtkTypeTraits<ScalarT>::Max()));

    // Round for non-floating point types:
    ScalarT refT;
    if (vtkTypeTraits<ScalarT>::VTK_TYPE_ID == VTK_FLOAT ||
        vtkTypeTraits<ScalarT>::VTK_TYPE_ID == VTK_DOUBLE)
      {
      refT = static_cast<ScalarT>(ref);
      }
    else
      {
      refT = static_cast<ScalarT>((ref >= 0.) ? (ref + 0.5) : (ref - 0.5));
      }

    ScalarT testT = output->GetValue(c);

    if (refT != testT)
      {
      DataArrayAPIError("Interpolated value incorrect: Got '"
                        << static_cast<double>(testT) << "', expected '"
                        << static_cast<double>(refT) << "'.");
      }
    } // foreach component

  DataArrayAPIFinish();
}

// void InterpolateTuple(vtkIdType i,
//                       vtkIdType id1, vtkAbstractArray *source1,
//                       vtkIdType id2, vtkAbstractArray *source2, double t)
// Interpolate tuple id1 from source1 and id2 form source2 and store the result
// in this tuple at tuple index i. t belongs to [0,1] and is the interpolation
// weight, with t=0 meaning 100% from source1.
// Should allocate memory.
template <typename ScalarT, typename ArrayT>
int Test_void_InterpolateTuple_i_id1_source1_id2_source2_t()
{
  DataArrayAPIInit("void InterpolateTuple(vtkIdType i, "
                   "vtkIdType id1, vtkAbstractArray *source1, "
                   "vtkIdType id2, vtkAbstractArray *source2, double t)");

  DataArrayAPICreateReferenceArray(source1);
  DataArrayAPICreateReferenceArray(source2);
  DataArrayAPICreateTestArray(output);

  // Initialize source arrays:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source1->SetNumberOfComponents(comps);
  source1->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source1->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  source2->SetNumberOfComponents(comps);
  source2->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source2->SetValue(i, static_cast<ScalarT>(((i + 3) * 2) % 17));
    }

  output->SetNumberOfComponents(comps);

  vtkIdType id1 = 4;
  vtkIdType id2 = 8;
  double t = 0.25;
  output->InterpolateTuple(0, id1, source1, id2, source2, t);

  // Validate result:
  for (int c = 0; c < comps; ++c)
    {
    // Compute component:
    double ref = source1->GetValue(id1 * comps + c) * (1. - t) +
                 source2->GetValue(id2 * comps + c) * t;

    // Clamp to ScalarT range:
    ref = std::max(ref, static_cast<double>(vtkTypeTraits<ScalarT>::Min()));
    ref = std::min(ref, static_cast<double>(vtkTypeTraits<ScalarT>::Max()));

    // Round for non-floating point types:
    ScalarT refT;
    if (vtkTypeTraits<ScalarT>::VTK_TYPE_ID == VTK_FLOAT ||
        vtkTypeTraits<ScalarT>::VTK_TYPE_ID == VTK_DOUBLE)
      {
      refT = static_cast<ScalarT>(ref);
      }
    else
      {
      refT = static_cast<ScalarT>((ref >= 0.) ? (ref + 0.5) : (ref - 0.5));
      }

    ScalarT test = output->GetValue(c);

    if (refT != test)
      {
      DataArrayAPIError("Interpolated value incorrect: Got '"
                        << static_cast<double>(test) << "', expected '"
                        << static_cast<double>(refT) << "'.");
      }
    } // foreach component

  DataArrayAPIFinish();
}

// int Resize(vtkIdType numTuples)
// Preserve data
// Return 1 on success, 0 otherwise
template <typename ScalarT, typename ArrayT>
int Test_int_Resize_numTuples()
{
  DataArrayAPIInit("int Resize(vtkIdType numTuples)");
  DataArrayAPICreateTestArray(array);
  int comps = 5;
  vtkIdType tuples = 0;
  array->SetNumberOfComponents(comps);

  int success = array->Resize(tuples);
  DataArrayAPIAssert(success, "Resize failed.");
  DataArrayAPIAssert(array->GetSize() == tuples * comps,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  tuples = 100;
  success = array->Resize(tuples);
  DataArrayAPIAssert(success, "Resize failed.");
  // May be larger than requested for size increases:
  DataArrayAPIAssert(array->GetSize() >= tuples * comps,
                     "Invalid Size (" << array->GetSize() << ").");
  // MaxId should not be changed:
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  // Fill with data
  vtkIdType filledTuples = tuples;
  array->SetNumberOfTuples(filledTuples);
  DataArrayAPIAssert(array->GetSize() >= filledTuples * comps,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == filledTuples * comps - 1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");
  for (vtkIdType t = 0; t < filledTuples; t++)
    {
    for (int c = 0; c < comps; ++c)
      {
      array->SetComponentValue(t, c,
                               static_cast<ScalarT>((t * comps + c) % 17));
      }
    }

  // resize larger
  tuples = 200;
  success = array->Resize(tuples);
  DataArrayAPIAssert(success, "Resize failed.");
  // May be larger than requested for size increases:
  DataArrayAPIAssert(array->GetSize() >= tuples * comps,
                     "Invalid Size (" << array->GetSize() << ").");
  // MaxId should not change:
  DataArrayAPIAssert(array->GetMaxId() == filledTuples * comps - 1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  // validate original data
  for (vtkIdType t = 0; t < filledTuples; t++)
    {
    for (int c = 0; c < comps; ++c)
      {
      ScalarT ref = static_cast<ScalarT>((t * comps + c) % 17);
      ScalarT test = array->GetComponentValue(t, c);
      DataArrayAPIAssert(ref == test,
                         "Data changed after resize for tuple " << t
                         << " component " << c << ": Expected " << ref
                         << " got " << test << ".");
      }
    }

  // resize smaller
  filledTuples = tuples = 50;
  success = array->Resize(tuples);
  DataArrayAPIAssert(success, "Resize failed.");
  // Both Size and MaxId should be truncated to the exact new size.
  DataArrayAPIAssert(array->GetSize() == tuples * comps,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == filledTuples * comps - 1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  // validate truncated data
  for (vtkIdType t = 0; t < filledTuples; t++)
    {
    for (int c = 0; c < comps; ++c)
      {
      ScalarT ref = static_cast<ScalarT>((t * comps + c) % 17);
      ScalarT test = array->GetComponentValue(t, c);
      DataArrayAPIAssert(ref == test,
                         "Data changed after resize for tuple " << t
                         << " component " << c << ": Expected " << ref
                         << " got " << test << ".");
      }
    }

  // Set to zero, should delete array
  filledTuples = tuples = 0;
  success = array->Resize(tuples);
  DataArrayAPIAssert(success, "Resize failed.");
  DataArrayAPIAssert(array->GetSize() == 0,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  DataArrayAPIFinish();
}

// void Reset()
// Don't free memory, just reset MaxId.
template <typename ScalarT, typename ArrayT>
int Test_void_Reset()
{
  DataArrayAPIInit("void Reset()");
  DataArrayAPICreateTestArray(array);
  int comps = 5;
  vtkIdType tuples = 0;
  array->SetNumberOfComponents(comps);

  // Reseting a new array shouldn't cause issues
  array->Reset();
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  tuples = 100;
  array->SetNumberOfTuples(tuples);
  DataArrayAPIAssert(array->GetSize() >= tuples * comps,
                     "Invalid Size (" << array->GetSize() << ").");
  int oldSize = array->GetSize();
  DataArrayAPIAssert(array->GetMaxId() == tuples * comps - 1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  array->Reset();
  tuples = 0;
  // Size should not change, but MaxId should
  DataArrayAPIAssert(array->GetSize() == oldSize,
                     "Invalid Size (" << array->GetSize() << ").");
  DataArrayAPIAssert(array->GetMaxId() == -1,
                     "Invalid MaxId (" << array->GetMaxId() << ").");

  DataArrayAPIFinish();
}

// void ExportToVoidPointer(void *out_ptr)
template <typename ScalarT, typename ArrayT>
int Test_void_ExportToVoidPointer_voidPtr()
{
  DataArrayAPIInit("void ExportToVoidPointer(void *out_ptr)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<ScalarT> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<ScalarT>(((t * comps) + c) % 17));
      }
    source->SetTupleValue(t, &tuple[0]);
    }

  std::vector<ScalarT> buffer(comps * tuples);
  source->ExportToVoidPointer(static_cast<void*>(&buffer[0]));

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      ScalarT test = buffer[t * comps + c];
      ScalarT ref = static_cast<ScalarT>(((t * comps) + c) % 17);
      DataArrayAPIAssert(test == ref,
                         "Data mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got " << test << ".");
        }
      }

  DataArrayAPIFinish();
}

// unsigned long GetActualMemorySize()
// Returns size in kibibytes (1024 bytes) for MaxId + 1 elements of ValueType.
template <typename ScalarT, typename ArrayT>
int Test_ulong_GetActualMemorySize()
{
  DataArrayAPIInit("unsigned long GetActualMemorySize()");

  DataArrayAPICreateTestArray(source);

  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);

  DataArrayAPIAssert(source->GetActualMemorySize() >=
                     (source->GetMaxId() + 1) * sizeof(ScalarT) / 1024,
                     "Invalid result. Expected at least "
                     << (source->GetMaxId() + 1) * sizeof(ScalarT) / 1024
                     << " kB, but got " << source->GetActualMemorySize()
                     << " kB.");

  DataArrayAPIFinish();
}

// int IsNumeric()
// Should be true for all data array subclasses:
template <typename ScalarT, typename ArrayT>
int Test_int_IsNumeric()
{
  DataArrayAPIInit("int IsNumeric()");

  DataArrayAPICreateTestArray(source);

  if (!source->IsNumeric())
    {
    DataArrayAPIError("IsNumeric() is false.");
    }

  DataArrayAPIFinish();
}

// vtkArrayIterator* NewIterator()
// Just test that the returned iterator is non-NULL and the data types match.
// The base vtkArrayIterator API doesn't really have much utility.
// TODO should we allow this to return NULL and deprecate? This mechanism is
// an inferior version of vtkGenericDataArrayMacro.
template <typename ScalarT, typename ArrayT>
int Test_vtkArrayIteratorPtr_NewIterator()
{
  DataArrayAPIInit("vtkArrayIterator* NewIterator()");

  DataArrayAPICreateTestArray(source);

  // Silence the void pointer warnings for these calls. The
  // vtkArrayIteratorTemplate implementation relies on GetVoidPointer.
  const char *oldWarning = getenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS");
  setenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS", "1", 1);

  vtkArrayIterator *iter = source->NewIterator();

  // Restore state:
  if (oldWarning)
    {
    setenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS", oldWarning, 1);
    }
  else
    {
    unsetenv("VTK_SILENCE_GET_VOID_POINTER_WARNINGS");
    }


  DataArrayAPIAssert(iter != NULL,
                     "NewIterator() returns NULL.");

  int iterDataType = iter->GetDataType();
  iter->Delete();

  DataArrayAPIAssert(iterDataType == source->GetDataType(),
                     "Iterator datatype does not match array.");

  DataArrayAPIFinish();
}

// vtkIdType GetDataSize()
// Same as GetNumberOfValues()
// TODO this is another good candidate for deprecation.
template <typename ScalarT, typename ArrayT>
int Test_vtkIdType_GetDataSize()
{
  DataArrayAPIInit("vtkIdType GetDataSize()");

  DataArrayAPICreateTestArray(source);

  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);

  DataArrayAPIAssert(source->GetDataSize() == comps * tuples,
                     "Invalid data size (number of values). Expected "
                     << comps * tuples << ", but got " << source->GetDataSize()
                     << ".");

  DataArrayAPIFinish();
}

// vtkIdType LookupValue(vtkVariant value)
// void LookupValue(vtkVariant value, vtkIdList* ids)
template <typename ScalarT, typename ArrayT>
int Test_LookupValue_allSigs()
{
  DataArrayAPIInit("LookupValue");

  DataArrayAPICreateTestArray(array);

  // Map Value --> ValueIdxs. We'll use this to validate the lookup results.
  typedef std::map<ScalarT, vtkIdList*> RefMap;
  typedef typename RefMap::iterator RefMapIterator;
  RefMap refMap;
  // These are the values we'll be looking for.
  for (ScalarT val = 0; val < 17; ++val)
    {
    refMap.insert(std::make_pair(val, vtkIdList::New()));
    }

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  array->SetNumberOfComponents(comps);
  array->SetNumberOfTuples(tuples);
  for (vtkIdType valIdx = 0; valIdx < comps * tuples; ++valIdx)
    {
    ScalarT val = static_cast<ScalarT>(valIdx % 17);
    array->SetValue(valIdx, val);
    // Update our reference map:
    RefMapIterator it = refMap.find(val);
    assert("Value exists in reference map." && it != refMap.end());
    it->second->InsertNextId(valIdx);
    }

  // Test the lookup functions.
  vtkNew<vtkIdList> testIdList;
  for (RefMapIterator it = refMap.begin(), itEnd = refMap.end(); it != itEnd;
       ++it)
    {
    const ScalarT &val = it->first;
    vtkIdList *refIdList = it->second; // Presorted due to insertion order
    vtkIdType *refIdBegin = refIdList->GetPointer(0);
    vtkIdType *refIdEnd = refIdList->GetPointer(refIdList->GetNumberOfIds());

    // Docs are unclear about this. Does it return the first value, or just any?
    // We'll assume any since it's unspecified.
    DataArrayAPIUpdateSignature("vtkIdType LookupValue(vtkVariant value)");
    vtkIdType testId = array->LookupValue(vtkVariant(val));
    if (!std::binary_search(refIdBegin, refIdEnd, testId))
      {
      // NonFatal + break so we can clean up.
      DataArrayAPINonFatalError("Looking up value '" << val
                                << "' returned valueIdx '" << testId
                                << "', which maps to value '"
                                << array->GetValue(testId) << "'.");
      break;
      }

    // Now for the list overload:
    DataArrayAPIUpdateSignature(
          "void LookupValue(vtkVariant value, vtkIdList* ids)");
    array->LookupValue(vtkVariant(val), testIdList.GetPointer());
    if (testIdList->GetNumberOfIds() != refIdList->GetNumberOfIds())
      {
      // NonFatal + break so we can clean up.
      DataArrayAPINonFatalError("Looking up value '" << val << "' returned "
                                << testIdList->GetNumberOfIds() << " ids, but "
                                << refIdList->GetNumberOfIds()
                                << "were expected.");
      break;
      }
    vtkIdType *testIdBegin = testIdList->GetPointer(0);
    vtkIdType *testIdEnd = testIdList->GetPointer(refIdList->GetNumberOfIds());
    // Ensure the test ids are sorted
    std::sort(testIdBegin, testIdEnd);
    if (!std::equal(testIdBegin, testIdEnd, refIdBegin))
      {
      // NonFatal + break so we can clean up.
      DataArrayAPINonFatalError("Looking up all value indices for value '"
                                << val
                                << "' did not return the expected result.");
      break;
      }
    }

  // Cleanup:
  for (RefMapIterator it = refMap.begin(), itEnd = refMap.end(); it != itEnd;
       ++it)
    {
    it->second->Delete();
    it->second = NULL;
    }

  DataArrayAPIFinish();
}

// vtkVariant GetVariantValue(vtkIdType idx)
template <typename ScalarT, typename ArrayT>
int Test_vtkVariant_GetVariantValue_valueIdx()
{
  DataArrayAPIInit("vtkVariant GetVariantValue(vtkIdType valueIdx)");

  DataArrayAPICreateTestArray(array);
  vtkIdType comps = 9;
  vtkIdType tuples = 5;
  array->SetNumberOfComponents(comps);
  array->SetNumberOfTuples(tuples);

  // Initialize:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    array->SetValue(i, static_cast<ScalarT>(i % 16));
    }

  // Verify:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    vtkVariant testVar = array->GetVariantValue(i);
    bool valid = false;
    ScalarT test = vtkVariantCast<ScalarT>(testVar, &valid);
    DataArrayAPIAssert(valid == true,
                       "vtkVariantCast failed.");
    ScalarT ref = static_cast<ScalarT>(i % 16);
    if (test != ref)
      {
      DataArrayAPIError("Data mismatch at value index '" << i << "'. Expected '"
                        << ref << "', got '" << test << "'.");
      }
    }

  DataArrayAPIFinish();
}

// void InsertVariantValue(vtkIdType idx, vtkVariant value)
template <typename ScalarT, typename ArrayT>
int Test_void_InsertVariantValue_idx_v()
{
  DataArrayAPIInit("void InsertVariantValue(vtkIdType idx, vtkVariant v)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array using tested function:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->InsertVariantValue(i, vtkVariant(static_cast<ScalarT>(i % 17)));

    if (source->GetSize() < i + 1)
      {
      DataArrayAPIError("Size should be at least " << i + 1
                        << " values, but is only " << source->GetSize() << ".");
      }
    if (source->GetMaxId() != i)
      {
      DataArrayAPIError("MaxId should be " << i << ", but is "
                        << source->GetMaxId() << " instead.");
      }
    }

  // Validate:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    ScalarT ref = static_cast<ScalarT>(i % 17);
    const typename ArrayT::ValueType test = source->GetValue(i);
    if (ref != test)
      {
      DataArrayAPIError("Data mismatch at value " << i << ": Expected '"
                        << ref << "', got '" << test << "'.");
      }
    }

  DataArrayAPIFinish();
}

// void SetVariantValue(vtkIdType idx, vtkVariant value)
template <typename ScalarT, typename ArrayT>
int Test_void_SetVariantValue_idx_v()
{
  DataArrayAPIInit("void SetVariantValue(vtkIdType valueIdx, vtkVariant v)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array using tested function:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetVariantValue(i, vtkVariant(
                              static_cast<ScalarT>(((i + 1) * (i + 2)) % 17)));
    }

  // Validate:
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    ScalarT ref = static_cast<ScalarT>(((i + 1) * (i + 2)) % 17);
    const typename ArrayT::ValueType test = source->GetValue(i);
    if (ref != test)
      {
      DataArrayAPIError("Data mismatch at value " << i << ": Expected '"
                        << ref << "', got '" << test << "'.");
      }
    }

  DataArrayAPIFinish();
}

//------------------------vtkDataArray API--------------------------------------

// double* GetTuple(vtkIdType i)
template <typename ScalarT, typename ArrayT>
int Test_doubleptr_GetTuple_i()
{
  DataArrayAPIInit("double* GetTuple(vtkIdType i)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Test the returned tuples:
  vtkIdType refValue = 0;
  for (vtkIdType tupleIdx = 0; tupleIdx < tuples; ++tupleIdx)
    {
    double *tuple = source->GetTuple(tupleIdx);
    for (int compIdx = 0; compIdx < comps; ++compIdx)
      {
      if (tuple[compIdx] != static_cast<double>(refValue))
        {
        DataArrayAPIError("Data mismatch at tuple " << tupleIdx << ", "
                          "component " << compIdx << ": Expected '" << refValue
                          << "', got '" << tuple[compIdx] << "'.");
        }
      ++refValue;
      refValue %= 17;
      }
    }

  DataArrayAPIFinish();
}

// void GetTuple(vtkIdType i, double *tuple)
// tuple must be large enough, of course
// double* GetTuple(vtkIdType i)
template <typename ScalarT, typename ArrayT>
int Test_void_GetTuple_i_tuple()
{
  DataArrayAPIInit("void GetTuple(vtkIdType i, double *tuple)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Test the returned tuples:
  vtkIdType refValue = 0;
  std::vector<double> tuple(comps);
  for (vtkIdType tupleIdx = 0; tupleIdx < tuples; ++tupleIdx)
    {
    source->GetTuple(tupleIdx, &tuple[0]);
    for (int compIdx = 0; compIdx < comps; ++compIdx)
      {
      if (tuple[compIdx] != static_cast<double>(refValue))
        {
        DataArrayAPIError("Data mismatch at tuple " << tupleIdx << ", "
                          "component " << compIdx << ": Expected '" << refValue
                          << "', got '" << tuple[compIdx] << "'.");
        }
      ++refValue;
      refValue %= 17;
      }
    }

  DataArrayAPIFinish();
}

// double GetComponent(vtkIdType i, int j)
// Return the value at tuple i, component j
template <typename ScalarT, typename ArrayT>
int Test_double_GetComponent_i_j()
{
  DataArrayAPIInit("double GetComponent(vtkIdType i, int j)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Test the returned tuples:
  vtkIdType refValue = 0;
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    for (int j = 0; j < comps; ++j)
      {
      if (source->GetComponent(i, j) != static_cast<double>(refValue))
        {
        DataArrayAPIError("Data mismatch at tuple " << i << ", "
                          "component " << j << ": Expected '" << refValue
                          << "', got '" << source->GetComponent(i, j) << "'.");
        }
      ++refValue;
      refValue %= 17;
      }
    }

  DataArrayAPIFinish();
}

// void SetComponent(vtkIdType i, int j, double c)
// Set tuple i, component j to value c.
// Must preallocate.
template <typename ScalarT, typename ArrayT>
int Test_void_SetComponent_i_j_c()
{
  DataArrayAPIInit("void SetComponent(vtkIdType i, int j, double c)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array using tested function:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    for (int j = 0; j < comps; ++j)
      {
      source->SetComponent(i, j, static_cast<double>(((i + 1) * (j + 1)) % 17));
      }
    }

  // Test the returned tuples:
  std::vector<double> tuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTuple(i, &tuple[0]);
    for (int j = 0; j < comps; ++j)
      {
      if (tuple[j] != static_cast<double>((i + 1) * (j + 1) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << i << ", component " << j
                          << ": Expected '" << ((i + 1) * (j + 1) % 17)
                          << "', got '" << tuple[j] << "'.");
        }
      }
    }

  DataArrayAPIFinish();
}

// void InsertComponent(vtkIdType i, int j, double c)
// Set tuple i component j to value c.
// Allocates memory as needed.
template <typename ScalarT, typename ArrayT>
int Test_void_InsertComponent_i_j_c()
{
  DataArrayAPIInit("void InsertComponent(vtkIdType i, int j, double c)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array using tested function:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      source->InsertComponent(t, c,
                              static_cast<double>(((t + 1) * (c + 1)) % 17));

      if (source->GetSize() < (t * comps) + c + 1)
        {
        DataArrayAPIError("Size should be at least " << (t * comps) + c + 1
                          << " values, but is only " << source->GetSize()
                          << ".");
        }
      if (source->GetMaxId() != (t * comps) + c)
        {
        DataArrayAPIError("MaxId should be " << (t * comps) + c
                          << ", but is " << source->GetMaxId() << " instead.");
        }
      }
    }

  // Test the returned tuples:
  std::vector<double> tuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTuple(i, &tuple[0]);
    for (int j = 0; j < comps; ++j)
      {
      if (tuple[j] != static_cast<double>((i + 1) * (j + 1) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << i << ", component " << j
                          << ": Expected '" << ((i + 1) * (j + 1) % 17)
                          << "', got '" << tuple[j] << "'.");
        }
      }
    }

  DataArrayAPIFinish();
}

// void FillComponent(int j, double c)
// For each tuple, set component j to value c.
template <typename ScalarT, typename ArrayT>
int Test_void_FillComponent_j_c()
{
  DataArrayAPIInit("void FillComponent(int j, double c)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array using tested function:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType j = 0; j < comps; ++j)
    {
    source->FillComponent(j, static_cast<double>(((j + 1) * j) % 17));
    }

  // Test the returned tuples:
  std::vector<double> tuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTuple(i, &tuple[0]);
    for (int j = 0; j < comps; ++j)
      {
      if (tuple[j] != static_cast<double>(((j + 1) * j) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << i << ", component " << j
                          << ": Expected '" << (((j + 1) * j) % 17)
                          << "', got '" << tuple[j] << "'.");
        }
      }
    }

  DataArrayAPIFinish();
}

// void* WriteVoidPointer(vtkIdType id, vtkIdType number)
// Ensure that there are at least (id + number) values allocated in the array.
// Update MaxId to ensure that any new values are marked as in-use.
// Return a void pointer to the value at index id.
// TODO This couldn't really work with the new vtkGenericDataArray stuff.
// Deprecate?
template <typename ScalarT, typename ArrayT>
int Test_voidptr_WriteVoidPointer_id_number()
{
  DataArrayAPIInit("void* WriteVoidPointer(vtkIdType id, vtkIdType number)");

  // Skip SoA arrays, as they do not allow this:
  if (typeid(ArrayT) == typeid(vtkSoADataArrayTemplate<ScalarT>))
    {
    std::cerr << "Skipping WriteVoidPointer for "
              << "vtkSoADataArrayTemplate<" << vtkTypeTraits<ScalarT>::Name()
              << ">.\n";
    DataArrayAPIFinish();
    }

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 10;
  vtkIdType values = comps * tuples;
  source->SetNumberOfComponents(comps);

  // Fill the array by writing to the returned void pointer. Trying various
  // write lengths that aren't just multiples of the tuple size.
  vtkIdType pos = 0;
  int writeLength = 1;
  while (true)
    {
    if (pos + writeLength > values)
      {
      writeLength = values - pos;
      }
    if (writeLength <= 0)
      {
      break;
      }

    void *voidPtr = source->WriteVoidPointer(pos, writeLength);

    // Verify that conditions are met:
    if (source->GetMaxId() != pos + writeLength - 1)
      {
      DataArrayAPIError("MaxId was not incremented to account for write length."
                        " MaxId is: " << source->GetMaxId() << ", expected: "
                        << (pos + writeLength - 1) << ".");
      }
    if (source->GetSize() < pos + writeLength)
      {
      DataArrayAPIError("Size was not increased to account for write length. "
                        "Size is: " << source->GetSize() << ", expected: "
                        << (pos + writeLength) << ".");
      }

    // Cast the pointer and write to it:
    ScalarT *ptr = static_cast<ScalarT*>(voidPtr);
    for (int i = 0; i < writeLength; ++i)
      {
      ptr[i] = static_cast<ScalarT>(((pos + 1) * pos) % 17);
      ++pos;
      }
    writeLength += 1;
    }

  // Test the returned tuples:
  vtkIdType v = 0;
  std::vector<double> tuple(comps);
  for (vtkIdType i = 0; i < tuples; ++i)
    {
    source->GetTuple(i, &tuple[0]);
    for (int j = 0; j < comps; ++j)
      {
      if (tuple[j] != static_cast<double>(((v + 1) * v) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << i << ", component " << j
                          << ": Expected '" << (((v + 1) * v) % 17)
                          << "', got '" << tuple[j] << "'.");
        }
      ++v;
      }
    }

  DataArrayAPIFinish();
}

// void CreateDefaultLookupTable()
// GetLookupTable should be non-NULL after calling.
template <typename ScalarT, typename ArrayT>
int Test_void_CreateDefaultLookupTable()
{
  DataArrayAPIInit("void CreateDefaultLookupTable()");

  DataArrayAPICreateTestArray(source);

  source->CreateDefaultLookupTable();
  if (source->GetLookupTable() == NULL)
    {
    DataArrayAPIError("Lookup table was not created.");
    }

  DataArrayAPIFinish();
}

//  double 	GetTuple1 (vtkIdType i)
//  double * 	GetTuple2 (vtkIdType i)
//  double * 	GetTuple3 (vtkIdType i)
//  double * 	GetTuple4 (vtkIdType i)
//  double * 	GetTuple6 (vtkIdType i)
//  double * 	GetTuple9 (vtkIdType i)
// Returns the ith tuple.
template <typename ScalarT, typename ArrayT, int N>
int Test_doubleptr_GetTupleN_i()
{
  std::ostringstream sigBuilder;
  sigBuilder << "double" << ((N == 1) ? "" : "*") << " GetTuple" << N
             << "(vtkIdType i)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = N;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType i = 0; i < comps * tuples; ++i)
    {
    source->SetValue(i, static_cast<ScalarT>(i % 17));
    }

  // Validate API:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<double> tuple(N);
    switch(N)
      {
      case 1:
        tuple[0] = source->GetTuple1(t);
        break;
#define vtkDataArrayAPIGetTupleNCase(_N) \
      case _N: \
        { \
        double *tmpPtr = source->GetTuple##_N(t); \
        std::copy(tmpPtr, tmpPtr + (_N), tuple.begin()); \
        } \
        break
      vtkDataArrayAPIGetTupleNCase(2);
      vtkDataArrayAPIGetTupleNCase(3);
      vtkDataArrayAPIGetTupleNCase(4);
      vtkDataArrayAPIGetTupleNCase(6);
      vtkDataArrayAPIGetTupleNCase(9);
#undef vtkDataArrayAPIGetTupleNCase
      default:
        DataArrayAPIError("Unrecognized tuple size: GetTuple" << N << "().");
      }

    for (int c = 0; c < comps; ++c)
      {
      double test = tuple[c];
      double ref = static_cast<double>((t * comps + c) % 17);
      if (test != ref)
        {
        DataArrayAPIError("Incorrect value returned for tuple " << t
                          << "component " << c << ": Got " << test
                          << ", expected " << ref << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void SetTuple(vtkIdType i, const float *tuple)
// void SetTuple(vtkIdType i, const double *tuple)
template <typename ScalarT, typename ArrayT, typename TupleArgT>
int Test_void_SetTuple_i_tuple()
{
  std::ostringstream sigBuilder;
  sigBuilder << "void SetTuple(vtkIdType i, "
             << vtkTypeTraits<TupleArgT>::Name() << " *tuple)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<TupleArgT> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<TupleArgT>(((t * comps) + c) % 17));
      }
    source->SetTuple(t, &tuple[0]);
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      if (source->GetComponent(t, c) !=
          static_cast<double>(((t * comps) + c) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected "
                          << static_cast<double>(((t * comps) + c) % 17)
                          << ", got " << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void SetTuple1(vtkIdType i, double value)
// void SetTuple2(vtkIdType i, double val0, double val1)
// void SetTuple3(vtkIdType i, double val0, double val1, ...)
// void SetTuple4(vtkIdType i, double val0, double val1, ...)
// void SetTuple6(vtkIdType i, double val0, double val1, ...)
// void SetTuple9(vtkIdType i, double val0, double val1, ...)
template <typename ScalarT, typename ArrayT, int N>
int Test_void_SetTupleN_i()
{
  std::ostringstream sigBuilder;
  sigBuilder << "void SetTuple" << N << "(vtkIdType i, double val0, ...)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = N;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<double> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<double>(((t * comps) + c) % 17));
      }
    switch (N)
      {
      case 1:
        source->SetTuple1(t, tuple[0]);
        break;
      case 2:
        source->SetTuple2(t, tuple[0], tuple[1]);
        break;
      case 3:
        source->SetTuple3(t, tuple[0], tuple[1], tuple[2]);
        break;
      case 4:
        source->SetTuple4(t, tuple[0], tuple[1], tuple[2], tuple[3]);
        break;
      case 6:
        source->SetTuple6(t, tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
                          tuple[5]);
        break;
      case 9:
        source->SetTuple9(t, tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
                          tuple[5], tuple[6], tuple[7], tuple[8]);
        break;
      default:
        DataArrayAPIError("Invalid N: " << N << ".");
      }
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      if (source->GetComponent(t, c) !=
          static_cast<double>(((t * comps) + c) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected "
                          << static_cast<double>(((t * comps) + c) % 17)
                          << ", got " << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void InsertTuple(vtkIdType i, const float *tuple)
// void InsertTuple(vtkIdType i, const double *tuple)
// Allocates memory as needed.
template <typename ScalarT, typename ArrayT, typename TupleArgT>
int Test_void_InsertTuple_i_tuple()
{
  std::ostringstream sigBuilder;
  sigBuilder << "void InsertTuple(vtkIdType i, "
             << vtkTypeTraits<TupleArgT>::Name() << " *tuple)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<TupleArgT> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<TupleArgT>(((t * comps) + c) % 17));
      }
    source->InsertTuple(t, &tuple[0]);
    if (source->GetSize() < ((t + 1) * comps))
      {
      DataArrayAPIError("Size should be at least " << ((t + 1) * comps)
                        << " values, but is only " << source->GetSize() << ".");
      }
    if (source->GetMaxId() != ((t + 1) * comps) - 1)
      {
      DataArrayAPIError("MaxId should be " << ((t + 1) * comps) - 1
                        << ", but is " << source->GetMaxId() << " instead.");
      }
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      if (source->GetComponent(t, c) !=
          static_cast<double>(((t * comps) + c) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected "
                          << static_cast<double>(((t * comps) + c) % 17)
                          << ", got " << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void InsertTuple1(vtkIdType i, double value)
// void InsertTuple2(vtkIdType i, double val0, double val1)
// void InsertTuple3(vtkIdType i, double val0, double val1, ...)
// void InsertTuple4(vtkIdType i, double val0, double val1, ...)
// void InsertTuple6(vtkIdType i, double val0, double val1, ...)
// void InsertTuple9(vtkIdType i, double val0, double val1, ...)
// Allocates memory as needed.
template <typename ScalarT, typename ArrayT, int N>
int Test_void_InsertTupleN_i()
{
  std::ostringstream sigBuilder;
  sigBuilder << "void InsertTuple" << N << "(vtkIdType i, double val0, ...)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = N;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<double> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<double>(((t * comps) + c) % 17));
      }
    switch (N)
      {
      case 1:
        source->InsertTuple1(t, tuple[0]);
        break;
      case 2:
        source->InsertTuple2(t, tuple[0], tuple[1]);
        break;
      case 3:
        source->InsertTuple3(t, tuple[0], tuple[1], tuple[2]);
        break;
      case 4:
        source->InsertTuple4(t, tuple[0], tuple[1], tuple[2], tuple[3]);
        break;
      case 6:
        source->InsertTuple6(t, tuple[0], tuple[1], tuple[2], tuple[3],
                             tuple[4], tuple[5]);
        break;
      case 9:
        source->InsertTuple9(t, tuple[0], tuple[1], tuple[2], tuple[3],
                             tuple[4], tuple[5], tuple[6], tuple[7], tuple[8]);
        break;
      default:
        DataArrayAPIError("Invalid N: " << N << ".");
      }
    if (source->GetSize() < ((t + 1) * comps))
      {
      DataArrayAPIError("Size should be at least " << ((t + 1) * comps)
                        << " values, but is only " << source->GetSize() << ".");
      }
    if (source->GetMaxId() != ((t + 1) * comps) - 1)
      {
      DataArrayAPIError("MaxId should be " << ((t + 1) * comps) - 1
                        << ", but is " << source->GetMaxId() << " instead.");
      }
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      if (source->GetComponent(t, c) !=
          static_cast<double>(((t * comps) + c) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected "
                          << static_cast<double>(((t * comps) + c) % 17)
                          << ", got " << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// vtkIdType InsertNextTuple(const float *tuple)
// vtkIdType InsertNextTuple(const double *tuple)
// Allocates memory as needed.
template <typename ScalarT, typename ArrayT, typename TupleArgT>
int Test_void_InsertNextTuple_tuple()
{
  std::ostringstream sigBuilder;
  sigBuilder << "void InsertNextTuple(" << vtkTypeTraits<TupleArgT>::Name()
             << " *tuple)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 5;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<TupleArgT> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<TupleArgT>(((t * comps) + c) % 17));
      }
    source->InsertNextTuple(&tuple[0]);
    if (source->GetSize() < ((t + 1) * comps))
      {
      DataArrayAPIError("Size should be at least " << ((t + 1) * comps)
                        << " values, but is only " << source->GetSize() << ".");
      }
    if (source->GetMaxId() != ((t + 1) * comps) - 1)
      {
      DataArrayAPIError("MaxId should be " << ((t + 1) * comps) - 1
                        << ", but is " << source->GetMaxId() << " instead.");
      }
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      if (source->GetComponent(t, c) !=
          static_cast<double>(((t * comps) + c) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected "
                          << static_cast<double>(((t * comps) + c) % 17)
                          << ", got " << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void InsertNextTuple1(double value)
// void InsertNextTuple2(double val0, double val1)
// void InsertNextTuple3(double val0, double val1, ...)
// void InsertNextTuple4(double val0, double val1, ...)
// void InsertNextTuple6(double val0, double val1, ...)
// void InsertNextTuple9(double val0, double val1, ...)
// Allocates memory as needed
template <typename ScalarT, typename ArrayT, int N>
int Test_void_InsertNextTupleN()
{
  std::ostringstream sigBuilder;
  sigBuilder << "void InsertNextTuple" << N << "(double val0, ...)";
  DataArrayAPIInit(sigBuilder.str());

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = N;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    std::vector<double> tuple;
    for (int c = 0; c < comps; ++c)
      {
      tuple.push_back(static_cast<double>(((t * comps) + c) % 17));
      }
    switch (comps)
      {
      case 1:
        source->InsertNextTuple1(tuple[0]);
        break;
      case 2:
        source->InsertNextTuple2(tuple[0], tuple[1]);
        break;
      case 3:
        source->InsertNextTuple3(tuple[0], tuple[1], tuple[2]);
        break;
      case 4:
        source->InsertNextTuple4(tuple[0], tuple[1], tuple[2], tuple[3]);
        break;
      case 6:
        source->InsertNextTuple6(tuple[0], tuple[1], tuple[2], tuple[3],
                                 tuple[4], tuple[5]);
        break;
      case 9:
        source->InsertNextTuple9(tuple[0], tuple[1], tuple[2], tuple[3],
                                 tuple[4], tuple[5], tuple[6], tuple[7],
                                 tuple[8]);
        break;
      default:
        DataArrayAPIError("Invalid N: " << N << ".");
      }
    if (source->GetSize() < ((t + 1) * comps))
      {
      DataArrayAPIError("Size should be at least " << ((t + 1) * comps)
                        << " values, but is only " << source->GetSize() << ".");
      }
    if (source->GetMaxId() != ((t + 1) * comps) - 1)
      {
      DataArrayAPIError("MaxId should be " << ((t + 1) * comps) - 1
                        << ", but is " << source->GetMaxId() << " instead.");
      }
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      if (source->GetComponent(t, c) !=
          static_cast<double>(((t * comps) + c) % 17))
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected "
                          << static_cast<double>(((t * comps) + c) % 17)
                          << ", got " << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void RemoveTuple(vtkIdType id)
template <typename ScalarT, typename ArrayT>
int Test_void_RemoveTuple_id()
{
  DataArrayAPIInit("void RemoveTuple(vtkIdType id)");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 6;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      source->SetComponent(t, c, static_cast<double>(((t * comps) + c) % 17));
      }
    }

  vtkIdType id = 3; // Tuple index to remove
  source->RemoveTuple(id);

  tuples -= 1;
  if (source->GetNumberOfTuples() != tuples)
    {
    DataArrayAPIError("Number of tuples did not change after RemoveTuple.");
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      double ref = ((t < id ? 0 : comps) + (t * comps) + c) % 17;
      if (source->GetComponent(t, c) != ref)
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got "
                          << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void RemoveFirstTuple()
template <typename ScalarT, typename ArrayT>
int Test_void_RemoveFirstTuple()
{
  DataArrayAPIInit("void RemoveFirstTuple()");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 6;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      source->SetComponent(t, c, static_cast<double>(((t * comps) + c) % 17));
      }
    }

  source->RemoveFirstTuple();

  tuples -= 1;
  if (source->GetNumberOfTuples() != tuples)
    {
    DataArrayAPIError("Number of tuples did not change after RemoveTuple.");
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      double ref = (comps + (t * comps) + c) % 17;
      if (source->GetComponent(t, c) != ref)
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got "
                          << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void RemoveLastTuple()
template <typename ScalarT, typename ArrayT>
int Test_void_RemoveLastTuple()
{
  DataArrayAPIInit("void RemoveLastTuple()");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 6;
  vtkIdType tuples = 10;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      source->SetComponent(t, c, static_cast<double>(((t * comps) + c) % 17));
      }
    }

  source->RemoveLastTuple();

  tuples -= 1;
  if (source->GetNumberOfTuples() != tuples)
    {
    DataArrayAPIError("Number of tuples did not change after RemoveTuple.");
    }

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      double ref = ((t * comps) + c) % 17;
      if (source->GetComponent(t, c) != ref)
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got "
                          << source->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void GetData(vtkIdType tupleMin, vtkIdType tupleMax, int compMin,
//              int compMax, vtkDoubleArray *data)
template <typename ScalarT, typename ArrayT>
int Test_void_GetData_tupleMin_tupleMax_compMin_compMax_data()
{
  DataArrayAPIInit("void RemoveLastTuple()");

  DataArrayAPICreateTestArray(source);

  // Initialize source array:
  vtkIdType comps = 9;
  vtkIdType tuples = 40;
  source->SetNumberOfComponents(comps);
  source->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      source->SetComponent(t, c, static_cast<double>(((t * comps) + c) % 17));
      }
    }

  // Extract some data:
  int compMin = 2;
  int compMax = 7;
  int dataComps = compMax - compMin + 1;
  vtkIdType tupleMin = 7;
  vtkIdType tupleMax = 32;
  vtkIdType dataTuples = tupleMax - tupleMin + 1;
  vtkNew<vtkDoubleArray> data;
  data->SetNumberOfComponents(dataComps);
  data->SetNumberOfTuples(dataTuples);
  source->GetData(tupleMin, tupleMax, compMin, compMax, data.GetPointer());

  // Verify:
  for (vtkIdType t = 0; t < dataTuples; ++t)
    {
    vtkIdType sourceTuple = t + tupleMin;
    for (int c = 0; c < dataComps; ++c)
      {
      int sourceComp = c + compMin;
      double ref = source->GetComponent(sourceTuple, sourceComp);
      double test = data->GetComponent(t, c);
      if (ref != test)
        {
        DataArrayAPIError("Mismatch at data tuple " << t << " component " << c
                          << ": Expected " << ref << ", got " << test << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// void CopyComponent(int j, vtkDataArray *from, int fromComponent)
// For all tuples, copy the fromComponent component from the 'from' array
// into the jth component in this.
template <typename ScalarT, typename ArrayT>
int Test_void_CopyComponent_j_from_fromComponent()
{
  DataArrayAPIInit(
        "void CopyComponent(int j, vtkDataArray *from, int fromComponent)");

  DataArrayAPICreateTestArray(target);
  DataArrayAPICreateReferenceArray(from);

  // Initialize arrays:
  vtkIdType comps = 11;
  vtkIdType tuples = 10;
  from->SetNumberOfComponents(comps);
  from->SetNumberOfTuples(tuples);
  target->SetNumberOfComponents(comps);
  target->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      target->SetComponent(t, c, static_cast<double>(((t * comps) + c) % 17));
      from->SetComponent(t, c, static_cast<double>(
                           (((t + 1) * comps) + (c + 1)) % 17));
      }
    }

  int j = 2;
  int fromComponent = 8;

  target->CopyComponent(j, from, fromComponent);

  // Verify:
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      double ref;
      if (c == j) // Use the target's value formula
        {
        ref = static_cast<double>(
              (((t + 1) * comps) + (fromComponent + 1)) % 17);
        }
      else // Use the source's value formula
        {
        ref = ((t * comps) + c) % 17;
        }
      if (target->GetComponent(t, c) != ref)
        {
        DataArrayAPIError("Data mismatch at tuple " << t << " component " << c
                          << ": Expected " << ref << ", got "
                          << target->GetComponent(t, c) << ".");
        }
      }
    }

  DataArrayAPIFinish();
}

// double* GetRange()
// void GetRange(double range[2])
// double* GetRange(int comp)
// void GetRange(double range[2], int comp)
template <typename ScalarT, typename ArrayT>
int Test_GetRange_all_overloads()
{
  DataArrayAPIInit("GetRange");

  DataArrayAPICreateTestArray(array);

  // Initialize arrays:
  vtkIdType comps = 6;
  vtkIdType tuples = 9;
  array->SetNumberOfComponents(comps);
  array->SetNumberOfTuples(tuples);
  for (vtkIdType t = 0; t < tuples; ++t)
    {
    for (int c = 0; c < comps; ++c)
      {
      array->SetComponent(t, c, (t + 1) * (c + 1));
      }
    }

  // Just the range of the first component:
  DataArrayAPIUpdateSignature("double* GetRange()");
  double *rangePtr = array->GetRange();
  double expectedRange[2] = { 1., static_cast<double>(tuples) };
  if (rangePtr[0] != expectedRange[0] ||
      rangePtr[1] != expectedRange[1])
    {
    DataArrayAPINonFatalError("First component range expected to be: ["
                              << expectedRange[0] << ", " << expectedRange[1]
                              << "], got [" << rangePtr[0] << ", "
                              << rangePtr[1] << "].");
    }

  DataArrayAPIUpdateSignature("void GetRange(double range[2])");
  double rangeArray[2];
  array->GetRange(rangeArray);
  if (rangeArray[0] != expectedRange[0] ||
      rangeArray[1] != expectedRange[1])
    {
    DataArrayAPINonFatalError("First component range expected to be: ["
                              << expectedRange[0] << ", " << expectedRange[1]
                              << "], got [" << rangeArray[0] << ", "
                              << rangeArray[1] << "].");
    }

  DataArrayAPIUpdateSignature("double* GetRange(int comp)");
  for (int c = 0; c < comps; ++c)
    {
    expectedRange[0] = c + 1;
    expectedRange[1] = tuples * (c + 1);
    rangePtr = array->GetRange(c);
    if (rangePtr[0] != expectedRange[0] ||
        rangePtr[1] != expectedRange[1])
      {
      DataArrayAPINonFatalError("Component " << c << " range expected to be: ["
                                << expectedRange[0] << ", " << expectedRange[1]
                                << "], got [" << rangePtr[0] << ", "
                                << rangePtr[1] << "].");
      }
    }

  DataArrayAPIUpdateSignature("void GetRange(double range[2], int comp)");
  for (int c = 0; c < comps; ++c)
    {
    expectedRange[0] = c + 1;
    expectedRange[1] = tuples * (c + 1);
    array->GetRange(rangeArray, c);
    if (rangeArray[0] != expectedRange[0] ||
        rangeArray[1] != expectedRange[1])
      {
      DataArrayAPINonFatalError("Component " << c << " range expected to be: ["
                                << expectedRange[0] << ", " << expectedRange[1]
                                << "], got [" << rangeArray[0] << ", "
                                << rangeArray[1] << "].");
      }
    }
  DataArrayAPIFinish();
}

//------------------------------------------------------------------------------
//-----------Unit Test Function Caller------------------------------------------
//------------------------------------------------------------------------------
template <typename ScalarT, typename ArrayT>
int ExerciseDataArray()
{
  int errors = 0;

  // vtkAbstractArray API:
  errors += Test_int_Allocate_numValues_ext<ScalarT, ArrayT>();
  errors += Test_void_Initialize<ScalarT, ArrayT>();
  errors += Test_int_GetDataType<ScalarT, ArrayT>();
  errors += Test_int_GetDataTypeSize<ScalarT, ArrayT>();
  errors += Test_int_GetElementComponentSize<ScalarT, ArrayT>();
  errors += Test_NumberOfComponents<ScalarT, ArrayT>();
  errors += Test_ComponentNames<ScalarT, ArrayT>();
  errors += Test_NumberOfTuples<ScalarT, ArrayT>();
  errors += Test_void_SetNumberOfValues_number<ScalarT, ArrayT>();
  errors += Test_void_SetTuple_i_j_source<ScalarT, ArrayT>();
  errors += Test_void_InsertTuple_i_j_source<ScalarT, ArrayT>();
  errors += Test_void_InsertTuples_dstIds_srcIds_source<ScalarT, ArrayT>();
  errors += Test_void_InsertTuples_dstStart_n_srcStart_source<ScalarT, ArrayT>();
  errors += Test_vtkIdType_InsertNextTuple_j_source<ScalarT, ArrayT>();
  errors += Test_void_GetTuples_ptIds_output<ScalarT, ArrayT>();
  errors += Test_void_GetTuples_p1_p2_output<ScalarT, ArrayT>();
  errors += Test_voidPtr_GetVoidPointer<ScalarT, ArrayT>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, char>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, float>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, double>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, vtkIdType>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, int>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, long long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, short>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, signed char>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, unsigned char>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, unsigned int>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, unsigned long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, unsigned long long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkAbstractArray, unsigned short>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, char>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, float>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, double>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, vtkIdType>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, int>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, long long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, short>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, signed char>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, unsigned char>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, unsigned int>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, unsigned long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, unsigned long long>();
  errors += Test_void_DeepCopy_array<ScalarT, ArrayT, vtkDataArray, unsigned short>();
  errors += Test_void_InterpolateTuple_i_indices_source_weights<ScalarT, ArrayT>();
  errors += Test_void_InterpolateTuple_i_id1_source1_id2_source2_t<ScalarT, ArrayT>();
  errors += Test_int_Resize_numTuples<ScalarT, ArrayT>();
  errors += Test_void_Reset<ScalarT, ArrayT>();
  errors += Test_void_ExportToVoidPointer_voidPtr<ScalarT, ArrayT>();
  errors += Test_ulong_GetActualMemorySize<ScalarT, ArrayT>();
  errors += Test_int_IsNumeric<ScalarT, ArrayT>();
  errors += Test_vtkArrayIteratorPtr_NewIterator<ScalarT, ArrayT>();
  errors += Test_vtkIdType_GetDataSize<ScalarT, ArrayT>();
  errors += Test_LookupValue_allSigs<ScalarT, ArrayT>();
  errors += Test_vtkVariant_GetVariantValue_valueIdx<ScalarT, ArrayT>();
  errors += Test_void_InsertVariantValue_idx_v<ScalarT, ArrayT>();
  errors += Test_void_SetVariantValue_idx_v<ScalarT, ArrayT>();

  // vtkDataArray API:
  errors += Test_doubleptr_GetTuple_i<ScalarT, ArrayT>();
  errors += Test_void_GetTuple_i_tuple<ScalarT, ArrayT>();
  errors += Test_double_GetComponent_i_j<ScalarT, ArrayT>();
  errors += Test_void_SetComponent_i_j_c<ScalarT, ArrayT>();
  errors += Test_void_InsertComponent_i_j_c<ScalarT, ArrayT>();
  errors += Test_voidptr_WriteVoidPointer_id_number<ScalarT, ArrayT>();
  errors += Test_void_CreateDefaultLookupTable<ScalarT, ArrayT>();
  errors += Test_doubleptr_GetTupleN_i<ScalarT, ArrayT, 1>();
  errors += Test_doubleptr_GetTupleN_i<ScalarT, ArrayT, 2>();
  errors += Test_doubleptr_GetTupleN_i<ScalarT, ArrayT, 3>();
  errors += Test_doubleptr_GetTupleN_i<ScalarT, ArrayT, 4>();
  errors += Test_doubleptr_GetTupleN_i<ScalarT, ArrayT, 6>();
  errors += Test_doubleptr_GetTupleN_i<ScalarT, ArrayT, 9>();
  errors += Test_void_SetTuple_i_tuple<ScalarT, ArrayT, float>();
  errors += Test_void_SetTuple_i_tuple<ScalarT, ArrayT, double>();
  errors += Test_void_SetTupleN_i<ScalarT, ArrayT, 1>();
  errors += Test_void_SetTupleN_i<ScalarT, ArrayT, 2>();
  errors += Test_void_SetTupleN_i<ScalarT, ArrayT, 3>();
  errors += Test_void_SetTupleN_i<ScalarT, ArrayT, 4>();
  errors += Test_void_SetTupleN_i<ScalarT, ArrayT, 6>();
  errors += Test_void_SetTupleN_i<ScalarT, ArrayT, 9>();
  errors += Test_void_InsertTuple_i_tuple<ScalarT, ArrayT, float>();
  errors += Test_void_InsertTuple_i_tuple<ScalarT, ArrayT, double>();
  errors += Test_void_InsertTupleN_i<ScalarT, ArrayT, 1>();
  errors += Test_void_InsertTupleN_i<ScalarT, ArrayT, 2>();
  errors += Test_void_InsertTupleN_i<ScalarT, ArrayT, 3>();
  errors += Test_void_InsertTupleN_i<ScalarT, ArrayT, 4>();
  errors += Test_void_InsertTupleN_i<ScalarT, ArrayT, 6>();
  errors += Test_void_InsertTupleN_i<ScalarT, ArrayT, 9>();
  errors += Test_void_InsertNextTuple_tuple<ScalarT, ArrayT, float>();
  errors += Test_void_InsertNextTuple_tuple<ScalarT, ArrayT, double>();
  errors += Test_void_InsertNextTupleN<ScalarT, ArrayT, 1>();
  errors += Test_void_InsertNextTupleN<ScalarT, ArrayT, 2>();
  errors += Test_void_InsertNextTupleN<ScalarT, ArrayT, 3>();
  errors += Test_void_InsertNextTupleN<ScalarT, ArrayT, 4>();
  errors += Test_void_InsertNextTupleN<ScalarT, ArrayT, 6>();
  errors += Test_void_InsertNextTupleN<ScalarT, ArrayT, 9>();
  errors += Test_void_RemoveTuple_id<ScalarT, ArrayT>();
  errors += Test_void_RemoveFirstTuple<ScalarT, ArrayT>();
  errors += Test_void_RemoveLastTuple<ScalarT, ArrayT>();
  errors += Test_void_GetData_tupleMin_tupleMax_compMin_compMax_data<ScalarT, ArrayT>();
  errors += Test_void_CopyComponent_j_from_fromComponent<ScalarT, ArrayT>();

  return errors;
} // end ExerciseDataArray

template <typename ScalarT, typename ArrayT>
int ExerciseGetRange()
{
  int errors = 0;

  errors += Test_GetRange_all_overloads<ScalarT, ArrayT>();

  return errors;
} // end ExerciseGetRange

} // end anon namespace

#undef DataArrayAPIInit
#undef DataArrayAPIUpdateSignature
#undef DataArrayAPIFinish
#undef DataArrayAPICreateTestArray
#undef DataArrayAPICreateReferenceArray
#undef DataArrayAPICreateReferenceArrayWithType
#undef DataArrayAPINonFatalError
#undef DataArrayAPIError
#undef DataArrayAPIAssert
