/*=========================================================================

 Program:   Visualization Toolkit
 Module:    VARSchemaManager.cxx

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

/*
 * VARSchemaManager.cxx
 *
 *  Created on: May 31, 2019
 *      Author: William F Godoy godoywf@ornl.gov
 */

#include "VARSchemaManager.h"

#include "schema/vtk/VARvtkVTI.h"
#include "schema/vtk/VARvtkVTU.h"

#include <vtk_pugixml.h>
#include <vtksys/SystemTools.hxx>

#include "common/VARHelper.h"

namespace var
{

// PUBLIC
void VARSchemaManager::Update(const std::string &streamName, const size_t step,
                              const std::string &schemaName)
{
    // can't do it in the constructor as it need MPI initialized
    if (!this->ADIOS)
    {
        this->ADIOS.reset(new adios2::ADIOS(helper::MPIGetComm()));
    }

    if (!this->IO && !this->Engine)
    {
        this->StreamName = streamName;
        this->SchemaName = schemaName;
        this->IO = this->ADIOS->DeclareIO(this->StreamName);
        this->Engine = this->IO.Open(this->StreamName, adios2::Mode::Read);
        InitReader();
    }
    else
    {
        // TODO: check if variables changed
    }
}

void VARSchemaManager::Fill(vtkMultiBlockDataSet *multiBlock, const size_t step)
{
    this->Reader->Fill(multiBlock, step);
}

// PRIVATE
const std::set<std::string> VARSchemaManager::SupportedTypes = {
    "ImageData", "UnstructuredGrid"};
// TODO: , "StructuredGrid", "PolyData" };

void VARSchemaManager::InitReader()
{
    if (InitReaderXMLVTK())
    {
        return;
    }
    // else if( InitReaderOther() ) {}
    // here we can make it extensible by trying to find other schema types
    // for now we stick with VTK XML schemas
}

bool VARSchemaManager::InitReaderXMLVTK()
{
    pugi::xml_document xmlDocument;
    std::string xmlContents;

    bool isSchemaFile = false;

    // check if it's file, not optimizing with MPI_Bcast
    std::string xmlFileName;
    if (vtksys::SystemTools::FileIsDirectory(this->Engine.Name()))
    {
        xmlFileName = this->Engine.Name() + "/" + this->SchemaName;
    }
    else if (vtksys::SystemTools::FileIsDirectory(this->Engine.Name() + ".dir"))
    {
        xmlFileName = this->Engine.Name() + ".dir/" + this->SchemaName;
    }

    if (!xmlFileName.empty())
    {
        if (vtksys::SystemTools::FileExists(xmlFileName))
        {
            xmlContents = helper::FileToString(xmlFileName);
            xmlDocument = helper::XMLDocument(xmlContents, true,
                                              "when reading " +
                                                  this->SchemaName + " file");
            isSchemaFile = true;
        }
    }

    if (!isSchemaFile)
    {
        const adios2::Attribute<std::string> vtkXMLAttribute =
            this->IO.InquireAttribute<std::string>(this->SchemaName);

        if (!vtkXMLAttribute)
        {
            throw std::runtime_error("ERROR: neither " + this->SchemaName +
                                     " file or bp attribute was found in " +
                                     this->Engine.Name() + "\n");
        }

        const std::vector<std::string> vtkAttributes = vtkXMLAttribute.Data();

        xmlContents = vtkAttributes.front();
        xmlDocument = helper::XMLDocument(xmlContents, true,
                                          "when reading " + this->SchemaName +
                                              " attribute");
    }

    constexpr bool isDebug = true;
    constexpr bool isMandatory = true;
    constexpr bool isUnique = true;

    const pugi::xml_node vtkXMLFileNode =
        helper::XMLNode("VTKFile", xmlDocument, isDebug,
                        "when reading VTKFile node in " + this->Engine.Name(),
                        isMandatory, isUnique);

    const pugi::xml_attribute typeXML = helper::XMLAttribute(
        "type", vtkXMLFileNode, true,
        "when reading type xml attribute in vtk.xml " + this->Engine.Name(),
        isMandatory);

    const std::string type = std::string(typeXML.value());

    if (this->SupportedTypes.count(type) == 0)
    {
        throw std::runtime_error("ERROR: ADIOS2Reader only supports types= " +
                                 helper::SetToCSV(this->SupportedTypes) +
                                 " when reading type xml attribute in " +
                                 this->SchemaName + " from " +
                                 this->Engine.Name() + "\n");
    }

    if (type == "ImageData")
    {
        this->Reader.reset(
            new schema::VARvtkVTI(xmlContents, this->IO, this->Engine));
    }
    else if (type == "UnstructuredGrid")
    {
        this->Reader.reset(
            new schema::VARvtkVTU(xmlContents, this->IO, this->Engine));
    }

    const bool success = this->Reader ? true : false;
    return success;
}

} // end var namespace
