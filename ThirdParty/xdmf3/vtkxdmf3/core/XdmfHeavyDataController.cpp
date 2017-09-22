/*****************************************************************************/
/*                                    XDMF                                   */
/*                       eXtensible Data Model and Format                    */
/*                                                                           */
/*  Id : XdmfHeavyDataController.cpp                                         */
/*                                                                           */
/*  Author:                                                                  */
/*     Kenneth Leiter                                                        */
/*     kenneth.leiter@arl.army.mil                                           */
/*     US Army Research Laboratory                                           */
/*     Aberdeen Proving Ground, MD                                           */
/*                                                                           */
/*     Copyright @ 2011 US Army Research Laboratory                          */
/*     All Rights Reserved                                                   */
/*     See Copyright.txt for details                                         */
/*                                                                           */
/*     This software is distributed WITHOUT ANY WARRANTY; without            */
/*     even the implied warranty of MERCHANTABILITY or FITNESS               */
/*     FOR A PARTICULAR PURPOSE.  See the above copyright notice             */
/*     for more information.                                                 */
/*                                                                           */
/*****************************************************************************/

#include <functional>
#include <numeric>
#include "string.h"
#include "XdmfArrayType.hpp"
#include "XdmfError.hpp"
#include "XdmfHeavyDataController.hpp"
#include "XdmfSystemUtils.hpp"

XdmfHeavyDataController::XdmfHeavyDataController(const std::string & filePath,
                                                 const shared_ptr<const XdmfArrayType> & type,
                                                 const std::vector<unsigned int> & starts,
                                                 const std::vector<unsigned int> & strides,
                                                 const std::vector<unsigned int> & dimensions,
                                                 const std::vector<unsigned int> & dataspaces) :
  mStart(starts),
  mStride(strides),
  mDimensions(dimensions),
  mDataspaceDimensions(dataspaces),
  mFilePath(filePath),
  mArrayStartOffset(0),
  mType(type)
{
}

XdmfHeavyDataController::~XdmfHeavyDataController()
{
}

unsigned int
XdmfHeavyDataController::getArrayOffset() const
{
  return mArrayStartOffset;
}

std::string
XdmfHeavyDataController::getDataspaceDescription() const
{
  std::stringstream dimensionStream;

  for (unsigned int j = 0; j < this->getStart().size(); ++j) {
    dimensionStream << this->getStart()[j];
    if (j < this->getStart().size() - 1) {
      dimensionStream << " ";
    }
  }
  dimensionStream << ":";
  for (unsigned int j = 0; j < this->getStride().size(); ++j) {
    dimensionStream << this->getStride()[j];
    if (j < this->getStride().size() - 1) {
      dimensionStream << " ";
    }
  }
  dimensionStream << ":";
  for (unsigned int j = 0; j < this->getDimensions().size(); ++j) {
    dimensionStream << this->getDimensions()[j];
    if (j < this->getDimensions().size() - 1) {
      dimensionStream << " ";
    }
  }
  dimensionStream << ":";
  for (unsigned int j = 0; j < this->getDataspaceDimensions().size(); ++j) {
    dimensionStream << this->getDataspaceDimensions()[j];
    if (j < this->getDataspaceDimensions().size() - 1) {
      dimensionStream << " ";
    }
  }
  return dimensionStream.str();
}

std::vector<unsigned int>
XdmfHeavyDataController::getDataspaceDimensions() const
{
  return mDataspaceDimensions;
}

unsigned int
XdmfHeavyDataController::getDataspaceSize() const
{
  return std::accumulate(mDataspaceDimensions.begin(),
                         mDataspaceDimensions.end(),
                         1,
                         std::multiplies<unsigned int>());
}

std::string
XdmfHeavyDataController::getDescriptor() const
{
  return "";
}

std::vector<unsigned int> 
XdmfHeavyDataController::getDimensions() const
{
  return mDimensions;
}

std::string
XdmfHeavyDataController::getFilePath() const
{
  return mFilePath;
}

std::vector<unsigned int>
XdmfHeavyDataController::getStart() const
{
  return mStart;
}

std::vector<unsigned int>
XdmfHeavyDataController::getStride() const
{
  return mStride;
}

unsigned int
XdmfHeavyDataController::getSize() const
{
  return std::accumulate(mDimensions.begin(),
                         mDimensions.end(),
                         1,
                         std::multiplies<unsigned int>());
}

shared_ptr<const XdmfArrayType>
XdmfHeavyDataController::getType() const
{
  return mType;
}

void
XdmfHeavyDataController::setArrayOffset(unsigned int newOffset)
{
  mArrayStartOffset = newOffset;
}

// C Wrappers

void XdmfHeavyDataControllerFree(XDMFHEAVYDATACONTROLLER * item)
{
  if (item != NULL) {
    delete (shared_ptr<XdmfHeavyDataController> *)item;
    item = NULL;
  }
}

unsigned int * XdmfHeavyDataControllerGetDataspaceDimensions(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  std::vector<unsigned int> tempVector = refController->getDataspaceDimensions();
  unsigned int returnSize = tempVector.size();
  unsigned int * returnArray = (unsigned int *)malloc(sizeof(unsigned int) * returnSize);
  for (unsigned int i = 0; i < returnSize; ++i) {
    returnArray[i] = tempVector[i];
  }
  return returnArray;
}

unsigned int * XdmfHeavyDataControllerGetDimensions(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  std::vector<unsigned int> tempVector = refController->getDimensions();
  unsigned int returnSize = tempVector.size();
  unsigned int * returnArray = (unsigned int *)malloc(sizeof(unsigned int) * returnSize);
  for (unsigned int i = 0; i < returnSize; ++i) {
    returnArray[i] = tempVector[i];
  }
  return returnArray;
}

char * XdmfHeavyDataControllerGetFilePath(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  char * returnPointer = strdup(refController->getFilePath().c_str());
  return returnPointer;
}

char * XdmfHeavyDataControllerGetName(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  char * returnPointer = strdup(refController->getName().c_str());
  return returnPointer;
}

unsigned int XdmfHeavyDataControllerGetNumberDimensions(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  return refController->getDimensions().size();
}

unsigned int XdmfHeavyDataControllerGetSize(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  return refController->getSize();
}

unsigned int * XdmfHeavyDataControllerGetStart(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  std::vector<unsigned int> tempVector = refController->getStart();
  unsigned int returnSize = tempVector.size();
  unsigned int * returnArray = (unsigned int *)malloc(sizeof(unsigned int) *returnSize);
  for (unsigned int i = 0; i < returnSize; ++i) {
    returnArray[i] = tempVector[i];
  }
  return returnArray;
}

unsigned int * XdmfHeavyDataControllerGetStride(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  std::vector<unsigned int> tempVector = refController->getStride();
  unsigned int returnSize = tempVector.size();
  unsigned int * returnArray = (unsigned int *)malloc(sizeof(unsigned int) * returnSize);
  for (unsigned int i = 0; i < returnSize; ++i) {
    returnArray[i] = tempVector[i];
  }
  return returnArray;
}

void XdmfHeavyDataControllerSetArrayOffset(XDMFHEAVYDATACONTROLLER * controller, unsigned int newOffset)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  refController->setArrayOffset(newOffset);
}

unsigned int XdmfHeavyDataControllerGetArrayOffset(XDMFHEAVYDATACONTROLLER * controller)
{
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  return refController->getArrayOffset();
}

int XdmfHeavyDataControllerGetType(XDMFHEAVYDATACONTROLLER * controller, int * status)
{
  XDMF_ERROR_WRAP_START(status)
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  shared_ptr<const XdmfArrayType> compareType = refController->getType();
  std::string typeName = compareType->getName();
  unsigned int typePrecision = compareType->getElementSize();
  if (typeName == XdmfArrayType::UInt8()->getName())
  {
      return XDMF_ARRAY_TYPE_UINT8;
  }
  else if (typeName == XdmfArrayType::UInt16()->getName())
  {
      return XDMF_ARRAY_TYPE_UINT16;
  }
  else if (typeName == XdmfArrayType::UInt32()->getName())
  {
      return XDMF_ARRAY_TYPE_UINT32;
  }
  else if (typeName == XdmfArrayType::Int8()->getName())
  {
      return XDMF_ARRAY_TYPE_INT8;
  }
  else if (typeName == XdmfArrayType::Int16()->getName())
  {
      return XDMF_ARRAY_TYPE_INT16;
  }
  else if (typeName == XdmfArrayType::Int32()->getName() || typeName == XdmfArrayType::Int64()->getName())
  {
    if (typePrecision == 4)
    {
      return XDMF_ARRAY_TYPE_INT32;
    }
    else if (typePrecision == 8)
    {
      return XDMF_ARRAY_TYPE_INT64;
    }
    else
    {
    }
  }
  else if (typeName == XdmfArrayType::Float32()->getName() || typeName == XdmfArrayType::Float64()->getName())
  {
    if (typePrecision == 4)
    {
      return XDMF_ARRAY_TYPE_FLOAT32;
    }
    else if (typePrecision == 8)
    {
      return XDMF_ARRAY_TYPE_FLOAT64;
    }
    else
    {
    }
  }
  else if (typeName == XdmfArrayType::String()->getName())
  {
    //This shouldn't be used from C bindings
    XdmfError::message(XdmfError::FATAL,
                       "Error: String type not usable from C.");
  }
  else
  {
    XdmfError::message(XdmfError::FATAL,
                       "Error: Invalid ArrayType.");
  }
  XDMF_ERROR_WRAP_END(status)
  return -1;
}

void XdmfHeavyDataControllerRead(XDMFHEAVYDATACONTROLLER * controller, void * array, int * status)
{
  XDMF_ERROR_WRAP_START(status)
  shared_ptr<XdmfHeavyDataController> & refController = *(shared_ptr<XdmfHeavyDataController> *)(controller);
  shared_ptr<XdmfArray> & refArray = *(shared_ptr<XdmfArray> *)(array);
  refController->read(refArray.get());
  XDMF_ERROR_WRAP_END(status)
}
