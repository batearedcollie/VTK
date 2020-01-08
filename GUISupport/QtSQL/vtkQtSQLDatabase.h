/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkQtSQLDatabase.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-------------------------------------------------------------------------
  Copyright 2008 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000 with Sandia Corporation,
  the U.S. Government retains certain rights in this software.
-------------------------------------------------------------------------*/
/**
 * @class   vtkQtSQLDatabase
 * @brief   maintains a connection to an sql database
 *
 *
 * Implements a vtkSQLDatabase using an underlying Qt QSQLDatabase.
 */

#ifndef vtkQtSQLDatabase_h
#define vtkQtSQLDatabase_h

// Check for Qt SQL module before defining this class.
#include <qglobal.h> // Needed to check if SQL is available
#if (QT_EDITION & QT_MODULE_SQL)

#include "vtkGUISupportQtSQLModule.h" // For export macro
#include "vtkSQLDatabase.h"

#include <QtSql/QSqlDatabase> // For the database member

class vtkSQLQuery;
class vtkStringArray;

class VTKGUISUPPORTQTSQL_EXPORT vtkQtSQLDatabase : public vtkSQLDatabase
{
public:
  static vtkQtSQLDatabase* New();
  vtkTypeMacro(vtkQtSQLDatabase, vtkSQLDatabase);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Open a new connection to the database.
   * You need to set up any database parameters before calling this function.
   * Returns true is the database was opened successfully, and false otherwise.
   */
  bool Open(const char* password) override;

  /**
   * Close the connection to the database.
   */
  void Close() override;

  /**
   * Return whether the database has an open connection
   */
  bool IsOpen() override;

  /**
   * Return an empty query on this database.
   */
  vtkSQLQuery* GetQueryInstance() override;

  /**
   * Get the list of tables from the database
   */
  vtkStringArray* GetTables() override;

  /**
   * Get the list of fields for a particular table
   */
  vtkStringArray* GetRecord(const char* table) override;

  /**
   * Returns a list of columns for a particular table.
   * Note that this is mainly for use with the VTK parallel server.
   * Serial VTK developers should prefer to use GetRecord() instead.
   */
  vtkStringArray* GetColumns();

  /**
   * Set the table used by GetColumns()
   * Note that this is mainly for use with the VTK parallel server.
   * Serial VTK developers should prefer to use GetRecord() instead.
   */
  void SetColumnsTable(const char* table);

  /**
   * Return whether a feature is supported by the database.
   */
  bool IsSupported(int feature) override;

  /**
   * Did the last operation generate an error
   */
  bool HasError() override;

  /**
   * Get the last error text from the database
   */
  const char* GetLastErrorText() override;

  //@{
  /**
   * String representing Qt database type (e.g. "mysql").
   */
  const char* GetDatabaseType() override { return this->DatabaseType; }
  vtkSetStringMacro(DatabaseType);
  //@}

  //@{
  /**
   * The database server host name.
   */
  vtkSetStringMacro(HostName);
  vtkGetStringMacro(HostName);
  //@}

  //@{
  /**
   * The user name for connecting to the database server.
   */
  vtkSetStringMacro(UserName);
  vtkGetStringMacro(UserName);
  //@}

  //@{
  /**
   * The name of the database to connect to.
   */
  vtkSetStringMacro(DatabaseName);
  vtkGetStringMacro(DatabaseName);
  //@}

  //@{
  /**
   * Additional options for the database.
   */
  vtkSetStringMacro(ConnectOptions);
  vtkGetStringMacro(ConnectOptions);
  //@}

  //@{
  /**
   * The port used for connecting to the database.
   */
  vtkSetClampMacro(Port, int, 0, 65535);
  vtkGetMacro(Port, int);
  //@}

  /**
   * Create a the proper subclass given a URL.
   * The URL format for SQL databases is a true URL of the form:
   * 'protocol://'[[username[':'password]'@']hostname[':'port]]'/'[dbname] .
   */
  static vtkSQLDatabase* CreateFromURL(const char* URL);

  /**
   * Get the URL of the database.
   */
  vtkStdString GetURL() override;

protected:
  vtkQtSQLDatabase();
  ~vtkQtSQLDatabase() override;

  char* DatabaseType;
  char* HostName;
  char* UserName;
  char* DatabaseName;
  int Port;
  char* ConnectOptions;

  QSqlDatabase QtDatabase;

  friend class vtkQtSQLQuery;

  /**
   * Overridden to determine connection parameters given the URL.
   * This is called by CreateFromURL() to initialize the instance.
   * Look at CreateFromURL() for details about the URL format.
   */
  bool ParseURL(const char* url) override;

private:
  // Storing the tables in the database, this array
  // is accessible through GetTables() method
  vtkStringArray* myTables;

  // Storing the correct record list from any one
  // of the tables in the database, this array is
  // accessible through GetRecord(const char *table)
  vtkStringArray* currentRecord;

  // Used to assign unique identifiers for database instances
  static int id;

  vtkQtSQLDatabase(const vtkQtSQLDatabase&) = delete;
  void operator=(const vtkQtSQLDatabase&) = delete;
};

#endif // (QT_EDITION & QT_MODULE_SQL)
#endif // vtkQtSQLDatabase_h
// VTK-HeaderTest-Exclude: vtkQtSQLDatabase.h
