/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPythonInterpreter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPythonInterpreter - wrapper for an embedded Python interpreter.
// .SECTION Description
// vtkPythonInterpreter only offers static methods. However, there may be need
// to register callbacks to call after the Python interpreter is initialized
// and before the interpreter is finalized. One can register observers for
// vtkCommand:EnterEvent and vtkCommand::ExitEvent to a local instance of
// vtkPythonInterpreter. vtkPythonInterpreter keeps tracks of all instances and
// invokes those events on all instances at appropriate times.
//
// Same is true for obtaining outputs/errors generated by Python.
// vtkCommand::ErrorEvent and vtkCommand::SetOutputEvent will be fired with
// calldata being const char* to the messages. Note that by default, the texts
// will be dumped on stdout/stderr in any case.
//
// To capture stdin, especially for non-terminal applications, set CaptureStdin
// to true. In that case vtkCommand::UpdateEvent will be fired with the calldata
// being a reference to a vtkStdString that should be filled in with the text to
// be passed in as the input.
//
// A few of the methods on this class implicitly call
// vtkPythonInterpreter::Initialize() to ensure Python is initialized viz.
// vtkPythonInterpreter::PyMain() and vtkPythonInterpreter::RunSimpleString().
// These implicit initialization always calls
// vtkPythonInterpreter::Initialize(1). If that's not what is expected,
// ensure that you call vtkPythonInterpreter::Initialize(0) before calling such
// methods. Refer to Py_InitializeEx() documentation for details on the
// differences between the two.
//
// Notes on calling Initialize/Finalize multiple times: Although applications
// are free to call Initialize/Finalize pairs multiple times, there are subtle
// differences between the first Initialize and subsequence Initialize calls
// after Finalize especially when concerning with imported modules. Refer to
// Python docs for details. In short, modules like numpy don't continue to work
// after a re-initialize. Hence use it with caution.
#ifndef vtkPythonInterpreter_h
#define vtkPythonInterpreter_h

#include "vtkObject.h"
#include "vtkPythonInterpreterModule.h" // For export macro
#include "vtkStdString.h" // needed for vtkStdString.

class VTKPYTHONINTERPRETER_EXPORT vtkPythonInterpreter : public vtkObject
{
public:
  static vtkPythonInterpreter* New();
  vtkTypeMacro(vtkPythonInterpreter, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Call this method to initialize Python. This has no effect if Python is
  // already initialized. Returns true if Python was initialized by this call,
  // or false if Python was already initialized.
  // Although, one can call Initialize()/Finalize() pair multiple times, Python
  // documentation warns that "Some extensions may not work properly if their
  // initialization routine is called more than once; this can happen if an
  // application calls Py_InitializeEx() and Py_Finalize() more than once."
  static bool Initialize(int initsigs=1);

  // Description:
  // Call this method to finalize Python. This has no effect if Python hasn't
  // been initialized already.
  static void Finalize();

  // Description:
  // Returns true is Python is initialized.
  static bool IsInitialized();

  // Description:
  // Set the program name. This must be called before the first Initialize()
  // call. If called afterwords, this will raise a warning.
  static void SetProgramName(const char* programname);

  // Description:
  // Call this method to start the Python event loop (Py_Main()).
  // This will initialize Python if not already initialized.
  static int PyMain(int argc, char** argv);

  // Description:
  // Developers are free to call Python C API directly. This convenience method
  // is provided to overcome an issue with the Python interpreter with handling
  // of DOS line endings.
  // This will initialize Python if not already initialized.
  static void RunSimpleString(const char* script);

  // Description:
  // Prepends the path to the sys.path variable. If Python has been
  // initialized, this call will update the sys.path variable otherwise the same
  // will be done once Python has been initialized. The paths added are saved so
  // that if Python is initialized again (by calls to Initialize()), then these
  // paths will be re-added.
  static void PrependPythonPath(const char*);

  // Description:
  // To capture stdin, especially for non-terminal applications, set CaptureStdin
  // to true. In that case vtkCommand::UpdateEvent will be fired with the calldata
  // being a reference to a vtkStdString that should be filled in with the text to
  // be passed in as the input.
  static void SetCaptureStdin(bool);
  static bool GetCaptureStdin();

//BTX
protected:
  vtkPythonInterpreter();
  ~vtkPythonInterpreter();

  friend struct vtkPythonStdStreamCaptureHelper;

  // Description:
  // Internal methods used by Python. Don't call directly.
  static void WriteStdOut(const char* txt);
  static void FlushStdOut();
  static void WriteStdErr(const char* txt);
  static void FlushStdErr();
  static vtkStdString ReadStdin();

private:
  vtkPythonInterpreter(const vtkPythonInterpreter&); // Not implemented.
  void operator=(const vtkPythonInterpreter&); // Not implemented.

  static bool InitializedOnce;
  static bool CaptureStdin;
//ETX
};

#endif
