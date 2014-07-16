/*****************************************************************************/
/*                                    XDMF                                   */
/*                       eXtensible Data Model and Format                    */
/*                                                                           */
/*  Id : XdmfArrayReference.hpp                                              */
/*                                                                           */
/*  Author:                                                                  */
/*     Andrew Burns                                                          */
/*     andrew.j.burns2@us.army.mil                                           */
/*     US Army Research Laboratory                                           */
/*     Aberdeen Proving Ground, MD                                           */
/*                                                                           */
/*     Copyright @ 2013 US Army Research Laboratory                          */
/*     All Rights Reserved                                                   */
/*     See Copyright.txt for details                                         */
/*                                                                           */
/*     This software is distributed WITHOUT ANY WARRANTY; without            */
/*     even the implied warranty of MERCHANTABILITY or FITNESS               */
/*     FOR A PARTICULAR PURPOSE.  See the above copyright notice             */
/*     for more information.                                                 */
/*                                                                           */
/*****************************************************************************/

#ifndef XDMFARRAYREFERENCE_HPP_
#define XDMFARRAYREFERENCE_HPP_

// Includes
#include "XdmfCore.hpp"
#include "XdmfArray.hpp"

class XdmfArray;

/**
 * @brief Serves as a link between an array and one or more arrays containing data it pulls from.
 *
 * The Array Reference class provides the basic framework for the writing and
 * reading of the Function and Subset classes by allowing properties and tags
 * to be migrated to them.
 */
class XDMFCORE_EXPORT XdmfArrayReference : public XdmfItem {

public:

  virtual ~XdmfArrayReference();

  LOKI_DEFINE_VISITABLE(XdmfArrayReference, XdmfItem)

  /**
   * Gets the properties of the array that the reference will generate when read from file.
   *
   * Example of use:
   *
   * C++
   * 
   * @dontinclude ExampleXdmfFunction.cpp
   * @skipline //#initexpression
   * @until //#initexpression
   * @skipline //#setConstructedProperties
   * @until //#setConstructedProperties
   * @skipline //#getConstructedProperties
   * @until //#getConstructedProperties
   *
   * Python
   *
   * @dontinclude XdmfExampleFunction.py
   * @skipline #//initexpression
   * @until #//initexpression
   * @skipline #//setConstructedProperties
   * @until #//setConstructedProperties
   * @skipline #//getConstructedProperties
   * @until #//getConstructedProperties
   *
   * @return    The properties of the array to be generated
   */
  std::map<std::string, std::string> getConstructedProperties();

  /**
   * Gets the type of array that the reference will generate when read from file.
   *
   * Example of use:
   *
   * C++
   * 
   * @dontinclude ExampleXdmfFunction.cpp
   * @skipline //#initexpression
   * @until //#initexpression
   * @skipline //#setConstructedType
   * @until //#setConstructedType
   * @skipline //#getConstructedType
   * @until //#getConstructedType
   *
   * Python
   *
   * @dontinclude XdmfExampleFunction.py
   * @skipline #//initexpression
   * @until #//initexpression
   * @skipline #//setConstructedType
   * @until #//setConstructedType
   * @skipline #//getConstructedType
   * @until #//getConstructedType
   *
   * @return    The tag of the type to be generated
   */
  std::string getConstructedType() const;

  /**
   * Parses the reference and generates an array containing the values that
   * the reference produces.
   *
   * Example of use:
   *
   * C++
   * 
   * @dontinclude ExampleXdmfFunction.cpp
   * @skipline //#initexpression
   * @until //#initexpression
   * @skipline //#read
   * @until //#read
   *
   * Python
   *
   * @dontinclude XdmfExampleFunction.py
   * @skipline #//initexpression
   * @until #//initexpression
   * @skipline #//read
   * @until #//read
   *
   * @return    The array generated by the reference
   */
  virtual shared_ptr<XdmfArray> read() const = 0;

  /**
   * Sets the properties of array that the reference will generate when read from file.
   *
   * Example of use:
   *
   * C++
   * 
   * @dontinclude ExampleXdmfFunction.cpp
   * @skipline //#initexpression
   * @until //#initexpression
   * @skipline //#setConstructedProperties
   * @until //#setConstructedProperties
   *
   * Python
   *
   * @dontinclude XdmfExampleFunction.py
   * @skipline #//initexpression
   * @until #//initexpression
   * @skipline #//setConstructedProperties
   * @until #//setConstructedProperties
   *
   * @param     newProperties   The properties of the array to be generated
   */
  void
  setConstructedProperties(std::map<std::string, std::string> newProperties);

  /**
   * Sets the type of array that the reference will generate when read from file.
   *
   * Example of use:
   *
   * C++
   * 
   * @dontinclude ExampleXdmfFunction.cpp
   * @skipline //#initexpression
   * @until //#initexpression
   * @skipline //#setConstructedType
   * @until //#setConstructedType
   *
   * Python
   *
   * @dontinclude XdmfExampleFunction.py
   * @skipline #//initexpression
   * @until #//initexpression
   * @skipline #//setConstructedType
   * @until #//setConstructedType
   *
   * @param     newType         The tag of the type to be generated
   */
  void setConstructedType(std::string newType);

protected:

  XdmfArrayReference();

  std::string mConstructedType;
  std::map<std::string, std::string> mConstructedProperties;

private:

  XdmfArrayReference(const XdmfArrayReference &);  // Not implemented.
  void operator=(const XdmfArrayReference &);  // Not implemented.
};

#endif /* XDMFARRAYREFERENCE_HPP_ */
