/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWrapPythonNamespace.c

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkWrapPythonNamespace.h"
#include "vtkWrapPythonConstant.h"
#include "vtkWrapPythonEnum.h"

#include "vtkWrap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


/* -------------------------------------------------------------------- */
/* Wrap the namespace */
int vtkWrapPython_WrapNamespace(FILE *fp, NamespaceInfo *data)
{
  int i;

  /* create any enum types defined in the namespace */
  for (i = 0; i < data->NumberOfEnums; i++)
    {
    vtkWrapPython_GenerateEnumType(
      fp, data->Name, data->Enums[i]);
    }

  fprintf(fp,
          "static PyObject *PyVTKNamespace_%s()\n"
          "{\n"
          "  PyObject *m = PyModule_New(\"%s\");\n"
          "\n",
          data->Name, data->Name);

  if (data->NumberOfEnums ||
      data->NumberOfConstants)
    {
    fprintf(fp,
            "  PyObject *d = PyModule_GetDict(m);\n"
            "  PyObject *o;\n"
            "\n");

    /* add any enum types defined in the namespace */
    for (i = 0; i < data->NumberOfEnums; i++)
      {
      vtkWrapPython_AddEnumType(
        fp, "  ", "d", "o", data->Name, data->Enums[i]);
      fprintf(fp, "\n");
      }

    /* add any constants defined in the namespace */
    for (i = 0; i < data->NumberOfConstants; i++)
      {
      vtkWrapPython_AddConstant(
        fp, "  ", "d", "o", data->Name, data->Constants[i]);
      fprintf(fp, "\n");
      }
    }

  fprintf(fp,
          "  return m;\n"
          "}\n"
          "\n");

  return 1;
}
