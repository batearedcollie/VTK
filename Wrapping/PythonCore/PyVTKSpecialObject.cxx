/*=========================================================================

  Program:   Visualization Toolkit
  Module:    PyVTKSpecialObject.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/*-----------------------------------------------------------------------
  The PyVTKSpecialObject was created in Feb 2001 by David Gobbi.
  It was substantially updated in April 2010 by David Gobbi.

  A PyVTKSpecialObject is a python object that represents an object
  that belongs to one of the special classes in VTK, that is, classes
  that are not derived from vtkObjectBase.  Unlike vtkObjects, these
  special objects are not reference counted: a PyVTKSpecialObject
  always contains its own copy of the C++ object.

  The PyVTKSpecialType is a simple structure that contains information
  about the PyVTKSpecialObject type that cannot be stored in python's
  PyTypeObject struct.  Each PyVTKSpecialObject contains a pointer to
  its PyVTKSpecialType. The PyVTKSpecialTypes are also stored in a map
  in vtkPythonUtil.cxx, so that they can be lookup up by name.
-----------------------------------------------------------------------*/

#include "PyVTKSpecialObject.h"
#include "PyVTKMethodDescriptor.h"
#include "vtkPythonUtil.h"

#include <vtksys/ios/sstream>

// Silence warning like
// "dereferencing type-punned pointer will break strict-aliasing rules"
// it happens because this kind of expression: (long *)&ptr
// pragma GCC diagnostic is available since gcc>=4.2
#if defined(__GNUC__) && (__GNUC__>4) || (__GNUC__==4 && __GNUC_MINOR__>=2)
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

//--------------------------------------------------------------------
PyVTKSpecialType::PyVTKSpecialType(
    PyTypeObject *typeobj, PyMethodDef *cmethods, PyMethodDef *ccons,
    vtkcopyfunc copyfunc)
{
  this->py_type = typeobj;
  this->vtk_methods = cmethods;
  this->vtk_constructors = ccons;
  this->vtk_copy = copyfunc;
}

//--------------------------------------------------------------------
// Object protocol

//--------------------------------------------------------------------
PyObject *PyVTKSpecialObject_Repr(PyObject *self)
{
  PyVTKSpecialObject *obj = (PyVTKSpecialObject *)self;
  PyTypeObject *type = self->ob_type;
  const char *name = self->ob_type->tp_name;

  PyObject *s = PyString_FromString("(");
  PyString_ConcatAndDel(&s, PyString_FromString(name));
  PyString_ConcatAndDel(&s, PyString_FromString(")"));

  while (type->tp_base && !type->tp_str)
    {
    type = type->tp_base;
    }

  // use str() if available
  if (type->tp_str && type->tp_str != (&PyBaseObject_Type)->tp_str)
    {
    PyObject *t = type->tp_str(self);
    if (t == NULL)
      {
      Py_XDECREF(s);
      s = NULL;
      }
    else
      {
      PyString_ConcatAndDel(&s, t);
      }
    }
  // otherwise just print address of object
  else if (obj->vtk_ptr)
    {
    char buf[256];
    sprintf(buf, "%p", obj->vtk_ptr);
    PyString_ConcatAndDel(&s, PyString_FromString(buf));
    }

  return s;
}

//--------------------------------------------------------------------
PyObject *PyVTKSpecialObject_SequenceString(PyObject *self)
{
  Py_ssize_t n, i;
  PyObject *s = NULL;
  PyObject *t, *o, *comma;
  const char *bracket = "[...]";

  if (self->ob_type->tp_as_sequence &&
      self->ob_type->tp_as_sequence->sq_item != NULL &&
      self->ob_type->tp_as_sequence->sq_ass_item == NULL)
    {
    bracket = "(...)";
    }

  i = Py_ReprEnter(self);
  if (i < 0)
    {
    return NULL;
    }
  else if (i > 0)
    {
    return PyString_FromString(bracket);
    }

  n = PySequence_Size(self);
  if (n >= 0)
    {
    comma = PyString_FromString(", ");
    s = PyString_FromStringAndSize(bracket, 1);

    for (i = 0; i < n && s != NULL; i++)
      {
      if (i > 0)
        {
        PyString_Concat(&s, comma);
        }
      o = PySequence_GetItem(self, i);
      t = NULL;
      if (o)
        {
        t = PyObject_Repr(o);
        Py_DECREF(o);
        }
      if (t)
        {
        PyString_ConcatAndDel(&s, t);
        }
      else
        {
        Py_DECREF(s);
        s = NULL;
        }
      n = PySequence_Size(self);
      }

    if (s)
      {
      PyString_ConcatAndDel(&s,
        PyString_FromStringAndSize(&bracket[4], 1));
      }

    Py_DECREF(comma);
    }

  Py_ReprLeave(self);

  return s;
}

//--------------------------------------------------------------------
// C API

//--------------------------------------------------------------------
// Create a new python object from the pointer to a C++ object
PyObject *PyVTKSpecialObject_New(const char *classname, void *ptr)
{
  // would be nice if "info" could be passed instead if "classname",
  // but this way of doing things is more dynamic if less efficient
  PyVTKSpecialType *info = vtkPythonUtil::FindSpecialType(classname);

  PyVTKSpecialObject *self = PyObject_New(PyVTKSpecialObject, info->py_type);

  self->vtk_info = info;
  self->vtk_ptr = ptr;
  self->vtk_hash = -1;

  return (PyObject *)self;
}

//--------------------------------------------------------------------
// Create a new python object via the copy constructor of the C++ object
PyObject *PyVTKSpecialObject_CopyNew(const char *classname, const void *ptr)
{
  PyVTKSpecialType *info = vtkPythonUtil::FindSpecialType(classname);

  if (info == 0)
    {
    char buf[256];
    sprintf(buf,"cannot create object of unknown type \"%s\"",classname);
    PyErr_SetString(PyExc_ValueError,buf);
    return NULL;
    }

  PyVTKSpecialObject *self = PyObject_New(PyVTKSpecialObject, info->py_type);

  self->vtk_info = info;
  self->vtk_ptr = info->vtk_copy(ptr);
  self->vtk_hash = -1;

  return (PyObject *)self;
}

//--------------------------------------------------------------------
// Add a special type, add methods and members to its type object.
// A return value of NULL signifies that it was already added.
PyVTKSpecialType *PyVTKSpecialType_Add(PyTypeObject *pytype,
  PyMethodDef *methods, PyMethodDef *constructors,
  const char *docstring[], vtkcopyfunc copyfunc)
{
  // Add this type to the special type map
  PyVTKSpecialType *info =
    vtkPythonUtil::AddSpecialTypeToMap(
      pytype, methods, constructors, copyfunc);

  if (info == 0)
    {
    // The type was already in the map, so do nothing
    return info;
    }

  // Create the dict
  if (pytype->tp_dict == 0)
    {
    pytype->tp_dict = PyDict_New();
    }

  // Add the docstring to the type
  PyObject *doc = vtkPythonUtil::BuildDocString(docstring);
  PyDict_SetItemString(pytype->tp_dict, "__doc__", doc);
  Py_DECREF(doc);

  // Add all of the methods
  for (PyMethodDef *meth = methods; meth && meth->ml_name; meth++)
    {
    PyObject *func = PyVTKMethodDescriptor_New(pytype, meth);
    PyDict_SetItemString(pytype->tp_dict, meth->ml_name, func);
    Py_DECREF(func);
    }

  return info;
}
