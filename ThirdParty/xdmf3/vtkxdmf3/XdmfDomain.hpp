/*****************************************************************************/
/*                                    XDMF                                   */
/*                       eXtensible Data Model and Format                    */
/*                                                                           */
/*  Id : XdmfDomain.hpp                                                      */
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

#ifndef XDMFDOMAIN_HPP_
#define XDMFDOMAIN_HPP_

// C Compatible Includes
#include "Xdmf.hpp"
#include "XdmfItem.hpp"
#include "XdmfCurvilinearGrid.hpp"
#include "XdmfGraph.hpp"
#include "XdmfRectilinearGrid.hpp"
#include "XdmfRegularGrid.hpp"
#include "XdmfUnstructuredGrid.hpp"

#ifdef __cplusplus

// Forward Declarations
class XdmfGridCollection;

/**
 * @brief The root XdmfItem that holds XdmfGrids.
 *
 * XdmfDomain is the top XdmfItem in an Xdmf structure.  It can store
 * a number of grids and provides methods to insert, retrieve, and
 * remove these grids.
 */
class XDMF_EXPORT XdmfDomain : public virtual XdmfItem {

public:

  /**
   * Create a new XdmfDomain.
   *
   * Example of use:
   *
   * C++
   *
   * @dontinclude ExampleXdmfDomain.cpp
   * @skipline //#initialization
   * @until //#initialization
   *
   * Python
   *
   * @dontinclude XdmfExampleDomain.py
   * @skipline #//initialization
   * @until #//initialization
   *
   * @return    Constructed XdmfDomain.
   */
  static shared_ptr<XdmfDomain> New();

  virtual ~XdmfDomain();

  LOKI_DEFINE_VISITABLE(XdmfDomain, XdmfItem)
  XDMF_CHILDREN(XdmfDomain, XdmfGridCollection, GridCollection, Name)
  XDMF_CHILDREN(XdmfDomain, XdmfGraph, Graph, Name)
  XDMF_CHILDREN(XdmfDomain, XdmfCurvilinearGrid, CurvilinearGrid, Name)
  XDMF_CHILDREN(XdmfDomain, XdmfRectilinearGrid, RectilinearGrid, Name)
  XDMF_CHILDREN(XdmfDomain, XdmfRegularGrid, RegularGrid, Name)
  XDMF_CHILDREN(XdmfDomain, XdmfUnstructuredGrid, UnstructuredGrid, Name)
  static const std::string ItemTag;

  std::map<std::string, std::string> getItemProperties() const;

  virtual std::string getItemTag() const;

  using XdmfItem::insert;

  virtual void traverse(const shared_ptr<XdmfBaseVisitor> visitor);

protected:

  XdmfDomain();
  virtual void
  populateItem(const std::map<std::string, std::string> & itemProperties,
               const std::vector<shared_ptr<XdmfItem> > & childItems,
               const XdmfCoreReader * const reader);

private:

  XdmfDomain(const XdmfDomain &);  // Not implemented.
  void operator=(const XdmfDomain &);  // Not implemented.

};

#endif

#ifdef __cplusplus
extern "C" {
#endif

// C wrappers go here

struct XDMFDOMAIN; // Simply as a typedef to ensure correct typing
typedef struct XDMFDOMAIN XDMFDOMAIN;

#ifndef XDMFGRIDCOLLECTIONCDEFINE
#define XDMFGRIDCOLLECTIONCDEFINE

struct XDMFGRIDCOLLECTION; // Simply as a typedef to ensure correct typing

typedef struct XDMFGRIDCOLLECTION XDMFGRIDCOLLECTION;
#endif

XDMF_EXPORT XDMFDOMAIN * XdmfDomainNew();

XDMF_EXPORT XDMFGRIDCOLLECTION * XdmfDomainGetGridCollection(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT XDMFGRIDCOLLECTION * XdmfDomainGetGridCollectionByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT unsigned int XdmfDomainGetNumberGridCollections(XDMFDOMAIN * domain);

XDMF_EXPORT void XdmfDomainInsertGridCollection(XDMFDOMAIN * domain, XDMFGRIDCOLLECTION * GridCollection, int passControl);

XDMF_EXPORT void XdmfDomainRemoveGridCollection(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT void XdmfDomainRemoveGridCollectionByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT XDMFGRAPH * XdmfDomainGetGraph(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT XDMFGRAPH * XdmfDomainGetGraphByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT unsigned int XdmfDomainGetNumberGraphs(XDMFDOMAIN * domain);

XDMF_EXPORT void XdmfDomainInsertGraph(XDMFDOMAIN * domain, XDMFGRAPH * graph, int passControl);

XDMF_EXPORT void XdmfDomainRemoveGraph(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT void XdmfDomainRemoveGraphByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT XDMFCURVILINEARGRID * XdmfDomainGetCurvilinearGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT XDMFCURVILINEARGRID * XdmfDomainGetCurvilinearGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT unsigned int XdmfDomainGetNumberCurvilinearGrids(XDMFDOMAIN * domain);

XDMF_EXPORT void XdmfDomainInsertCurvilinearGrid(XDMFDOMAIN * domain, XDMFCURVILINEARGRID * CurvilinearGrid, int passControl);

XDMF_EXPORT void XdmfDomainRemoveCurvilinearGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT void XdmfDomainRemoveCurvilinearGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT XDMFRECTILINEARGRID * XdmfDomainGetRectilinearGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT XDMFRECTILINEARGRID * XdmfDomainGetRectilinearGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT unsigned int XdmfDomainGetNumberRectilinearGrids(XDMFDOMAIN * domain);

XDMF_EXPORT void XdmfDomainInsertRectilinearGrid(XDMFDOMAIN * domain, XDMFRECTILINEARGRID * RectilinearGrid, int passControl);

XDMF_EXPORT void XdmfDomainRemoveRectilinearGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT void XdmfDomainRemoveRectilinearGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT XDMFREGULARGRID * XdmfDomainGetRegularGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT XDMFREGULARGRID * XdmfDomainGetRegularGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT unsigned int XdmfDomainGetNumberRegularGrids(XDMFDOMAIN * domain);

XDMF_EXPORT void XdmfDomainInsertRegularGrid(XDMFDOMAIN * domain, XDMFREGULARGRID * RegularGrid, int passControl);

XDMF_EXPORT void XdmfDomainRemoveRegularGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT void XdmfDomainRemoveRegularGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT XDMFUNSTRUCTUREDGRID * XdmfDomainGetUnstructuredGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT XDMFUNSTRUCTUREDGRID * XdmfDomainGetUnstructuredGridByName(XDMFDOMAIN * domain, char * Name);

XDMF_EXPORT unsigned int XdmfDomainGetNumberUnstructuredGrids(XDMFDOMAIN * domain);

XDMF_EXPORT void XdmfDomainInsertUnstructuredGrid(XDMFDOMAIN * domain, XDMFUNSTRUCTUREDGRID * UnstructuredGrid, int passControl);

XDMF_EXPORT void XdmfDomainRemoveUnstructuredGrid(XDMFDOMAIN * domain, unsigned int index);

XDMF_EXPORT void XdmfDomainRemoveUnstructuredGridByName(XDMFDOMAIN * domain, char * Name);


XDMF_ITEM_C_CHILD_DECLARE(XdmfDomain, XDMFDOMAIN, XDMF)

#define XDMF_DOMAIN_C_CHILD_DECLARE(ClassName, CClassName, Level)                                                                     \
                                                                                                                                      \
Level##_EXPORT XDMFGRIDCOLLECTION * ClassName##GetGridCollection(CClassName * domain, unsigned int index);                            \
Level##_EXPORT XDMFGRIDCOLLECTION * ClassName##GetGridCollectionByName(CClassName * domain, char * Name);                             \
Level##_EXPORT unsigned int ClassName##GetNumberGridCollections(CClassName * domain);                                                 \
Level##_EXPORT void ClassName##InsertGridCollection(CClassName * domain, XDMFGRIDCOLLECTION * GridCollection, int passControl);       \
Level##_EXPORT void ClassName##RemoveGridCollection(CClassName * domain, unsigned int index);                                         \
Level##_EXPORT void ClassName##RemoveGridCollectionByName(CClassName * domain, char * Name);                                          \
Level##_EXPORT XDMFGRAPH * ClassName##GetGraph(CClassName * domain, unsigned int index);                                              \
Level##_EXPORT XDMFGRAPH * ClassName##GetGraphByName(CClassName * domain, char * Name);                                               \
Level##_EXPORT unsigned int ClassName##GetNumberGraphs(CClassName * domain);                                                          \
Level##_EXPORT void ClassName##InsertGraph(CClassName * domain, XDMFGRAPH * Graph, int passControl);                                  \
Level##_EXPORT void ClassName##RemoveGraph(CClassName * domain, unsigned int index);                                                  \
Level##_EXPORT void ClassName##RemoveGraphByName(CClassName * domain, char * Name);                                                   \
Level##_EXPORT XDMFCURVILINEARGRID * ClassName##GetCurvilinearGrid(CClassName * domain, unsigned int index);                          \
Level##_EXPORT XDMFCURVILINEARGRID * ClassName##GetCurvilinearGridByName(CClassName * domain, char * Name);                           \
Level##_EXPORT unsigned int ClassName##GetNumberCurvilinearGrids(CClassName * domain);                                                \
Level##_EXPORT void ClassName##InsertCurvilinearGrid(CClassName * domain, XDMFCURVILINEARGRID * CurvilinearGrid, int passControl);    \
Level##_EXPORT void ClassName##RemoveCurvilinearGrid(CClassName * domain, unsigned int index);                                        \
Level##_EXPORT void ClassName##RemoveCurvilinearGridByName(CClassName * domain, char * Name);                                         \
Level##_EXPORT XDMFRECTILINEARGRID * ClassName##GetRectilinearGrid(CClassName * domain, unsigned int index);                          \
Level##_EXPORT XDMFRECTILINEARGRID * ClassName##GetRectilinearGridByName(CClassName * domain, char * Name);                           \
Level##_EXPORT unsigned int ClassName##GetNumberRectilinearGrids(CClassName * domain);                                                \
Level##_EXPORT void ClassName##InsertRectilinearGrid(CClassName * domain, XDMFRECTILINEARGRID * RectilinearGrid, int passControl);    \
Level##_EXPORT void ClassName##RemoveRectilinearGrid(CClassName * domain, unsigned int index);                                        \
Level##_EXPORT void ClassName##RemoveRectilinearGridByName(CClassName * domain, char * Name);                                         \
Level##_EXPORT XDMFREGULARGRID * ClassName##GetRegularGrid(CClassName * domain, unsigned int index);                                  \
Level##_EXPORT XDMFREGULARGRID * ClassName##GetRegularGridByName(CClassName * domain, char * Name);                                   \
Level##_EXPORT unsigned int ClassName##GetNumberRegularGrids(CClassName * domain);                                                    \
Level##_EXPORT void ClassName##InsertRegularGrid(CClassName * domain, XDMFREGULARGRID * RegularGrid, int passControl);                \
Level##_EXPORT void ClassName##RemoveRegularGrid(CClassName * domain, unsigned int index);                                            \
Level##_EXPORT void ClassName##RemoveRegularGridByName(CClassName * domain, char * Name);                                             \
Level##_EXPORT XDMFUNSTRUCTUREDGRID * ClassName##GetUnstructuredGrid(CClassName * domain, unsigned int index);                        \
Level##_EXPORT XDMFUNSTRUCTUREDGRID * ClassName##GetUnstructuredGridByName(CClassName * domain, char * Name);                         \
Level##_EXPORT unsigned int ClassName##GetNumberUnstructuredGrids(CClassName * domain);                                               \
Level##_EXPORT void ClassName##InsertUnstructuredGrid(CClassName * domain, XDMFUNSTRUCTUREDGRID * UnstructuredGrid, int passControl); \
Level##_EXPORT void ClassName##RemoveUnstructuredGrid(CClassName * domain, unsigned int index);                                       \
Level##_EXPORT void ClassName##RemoveUnstructuredGridByName(CClassName * domain, char * Name);


#define XDMF_DOMAIN_C_CHILD_WRAPPER(ClassName, CClassName)                                                                            \
                                                                                                                                      \
XDMFGRIDCOLLECTION * ClassName##GetGridCollection(CClassName * domain, unsigned int index)                                            \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetGridCollection((XDMFDOMAIN *)((void *)&baseDomain), index);                                                     \
}                                                                                                                                     \
                                                                                                                                      \
XDMFGRIDCOLLECTION * ClassName##GetGridCollectionByName(CClassName * domain, char * Name)                                             \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetGridCollectionByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                \
}                                                                                                                                     \
                                                                                                                                      \
unsigned int ClassName##GetNumberGridCollections(CClassName * domain)                                                                 \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetNumberGridCollections((XDMFDOMAIN *)((void *)&baseDomain));                                                      \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##InsertGridCollection(CClassName * domain, XDMFGRIDCOLLECTION * GridCollection, int passControl)                       \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainInsertGridCollection((XDMFDOMAIN *)((void *)&baseDomain), GridCollection, passControl);                                        \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveGridCollection(CClassName * domain, unsigned int index)                                                         \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveGridCollection((XDMFDOMAIN *)((void *)&baseDomain), index);                                                              \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveGridCollectionByName(CClassName * domain, char * Name)                                                          \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveGridCollectionByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                         \
}                                                                                                                                     \
                                                                                                                                      \
XDMFGRAPH * ClassName##GetGraph(CClassName * domain, unsigned int index)                                                              \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetGraph((XDMFDOMAIN *)((void *)&baseDomain), index);                                                                   \
}                                                                                                                                     \
                                                                                                                                      \
XDMFGRAPH * ClassName##GetGraphByName(CClassName * domain, char * Name)                                                               \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetGraphByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                              \
}                                                                                                                                     \
                                                                                                                                      \
unsigned int ClassName##GetNumberGraphs(CClassName * domain)                                                                          \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetNumberGraphs((XDMFDOMAIN *)((void *)&baseDomain));                                                                   \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##InsertGraph(CClassName * domain, XDMFGRAPH * Graph, int passControl)                                                  \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainInsertGraph((XDMFDOMAIN *)((void *)&baseDomain), Graph, passControl);                                                          \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveGraph(CClassName * domain, unsigned int index)                                                                  \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveGraph((XDMFDOMAIN *)((void *)&baseDomain), index);                                                                       \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveGraphByName(CClassName * domain, char * Name)                                                                   \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveGraphByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                                  \
}                                                                                                                                     \
                                                                                                                                      \
XDMFCURVILINEARGRID * ClassName##GetCurvilinearGrid(CClassName * domain, unsigned int index)                                          \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetCurvilinearGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                         \
}                                                                                                                                     \
                                                                                                                                      \
XDMFCURVILINEARGRID * ClassName##GetCurvilinearGridByName(CClassName * domain, char * Name)                                           \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetCurvilinearGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                    \
}                                                                                                                                     \
                                                                                                                                      \
unsigned int ClassName##GetNumberCurvilinearGrids(CClassName * domain)                                                                \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetNumberCurvilinearGrids((XDMFDOMAIN *)((void *)&baseDomain));                                                         \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##InsertCurvilinearGrid(CClassName * domain, XDMFCURVILINEARGRID * CurvilinearGrid, int passControl)                    \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainInsertCurvilinearGrid((XDMFDOMAIN *)((void *)&baseDomain), CurvilinearGrid, passControl);                                      \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveCurvilinearGrid(CClassName * domain, unsigned int index)                                                        \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveCurvilinearGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                             \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveCurvilinearGridByName(CClassName * domain, char * Name)                                                         \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveCurvilinearGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                        \
}                                                                                                                                     \
                                                                                                                                      \
XDMFRECTILINEARGRID * ClassName##GetRectilinearGrid(CClassName * domain, unsigned int index)                                          \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetRectilinearGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                         \
}                                                                                                                                     \
                                                                                                                                      \
XDMFRECTILINEARGRID * ClassName##GetRectilinearGridByName(CClassName * domain, char * Name)                                           \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetRectilinearGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                    \
}                                                                                                                                     \
                                                                                                                                      \
unsigned int ClassName##GetNumberRectilinearGrids(CClassName * domain)                                                                \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetNumberRectilinearGrids((XDMFDOMAIN *)((void *)&baseDomain));                                                         \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##InsertRectilinearGrid(CClassName * domain, XDMFRECTILINEARGRID * RectilinearGrid, int passControl)                    \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainInsertRectilinearGrid((XDMFDOMAIN *)((void *)&baseDomain), RectilinearGrid, passControl); \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveRectilinearGrid(CClassName * domain, unsigned int index)                                                        \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveRectilinearGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                             \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveRectilinearGridByName(CClassName * domain, char * Name)                                                         \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveRectilinearGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                        \
}                                                                                                                                     \
                                                                                                                                      \
XDMFREGULARGRID * ClassName##GetRegularGrid(CClassName * domain, unsigned int index)                                                  \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetRegularGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                             \
}                                                                                                                                     \
                                                                                                                                      \
XDMFREGULARGRID * ClassName##GetRegularGridByName(CClassName * domain, char * Name)                                                   \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetRegularGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                        \
}                                                                                                                                     \
                                                                                                                                      \
unsigned int ClassName##GetNumberRegularGrids(CClassName * domain)                                                                    \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetNumberRegularGrids((XDMFDOMAIN *)((void *)&baseDomain));                                                        \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##InsertRegularGrid(CClassName * domain, XDMFREGULARGRID * RegularGrid, int passControl)                                \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainInsertRegularGrid((XDMFDOMAIN *)((void *)&baseDomain), RegularGrid, passControl);                                         \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveRegularGrid(CClassName * domain, unsigned int index)                                                            \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveRegularGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                            \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveRegularGridByName(CClassName * domain, char * Name)                                                             \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveRegularGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                       \
}                                                                                                                                     \
                                                                                                                                      \
XDMFUNSTRUCTUREDGRID * ClassName##GetUnstructuredGrid(CClassName * domain, unsigned int index)                                        \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetUnstructuredGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                   \
}                                                                                                                                     \
                                                                                                                                      \
XDMFUNSTRUCTUREDGRID * ClassName##GetUnstructuredGridByName(CClassName * domain, char * Name)                                         \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetUnstructuredGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                              \
}                                                                                                                                     \
                                                                                                                                      \
unsigned int ClassName##GetNumberUnstructuredGrids(CClassName * domain)                                                               \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  return XdmfDomainGetNumberUnstructuredGrids((XDMFDOMAIN *)((void *)&baseDomain));                                                   \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##InsertUnstructuredGrid(CClassName * domain, XDMFUNSTRUCTUREDGRID * UnstructuredGrid, int passControl)                 \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainInsertUnstructuredGrid((XDMFDOMAIN *)((void *)&baseDomain), UnstructuredGrid, passControl);                               \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveUnstructuredGrid(CClassName * domain, unsigned int index)                                                       \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveUnstructuredGrid((XDMFDOMAIN *)((void *)&baseDomain), index);                                                       \
}                                                                                                                                     \
                                                                                                                                      \
void ClassName##RemoveUnstructuredGridByName(CClassName * domain, char * Name)                                                        \
{                                                                                                                                     \
  shared_ptr< XdmfDomain > baseDomain = *(shared_ptr< ClassName > *) domain;                                                          \
  XdmfDomainRemoveUnstructuredGridByName((XDMFDOMAIN *)((void *)&baseDomain), Name);                                                  \
}

#ifdef __cplusplus
}
#endif

#endif /* XDMFDOMAIN_HPP_ */
