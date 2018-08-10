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
/**
 * @class   vtkPythonInterpreter
 * @brief   wrapper for an embedded Python interpreter.
 *
 * vtkPythonInterpreter only offers static methods. However, there may be need
 * to register callbacks to call after the Python interpreter is initialized
 * and before the interpreter is finalized. One can register observers for
 * vtkCommand:EnterEvent and vtkCommand::ExitEvent to a local instance of
 * vtkPythonInterpreter. vtkPythonInterpreter keeps tracks of all instances and
 * invokes those events on all instances at appropriate times.
 *
 * Same is true for obtaining outputs/errors generated by Python.
 * vtkCommand::ErrorEvent and vtkCommand::SetOutputEvent will be fired with
 * calldata being const char* to the messages. Errors and output messages will
 * also be forwarded to the vtkOutputWindow singleton (via
 * `vtkOutputWindowDisplayErrorText` and `vtkOutputWindowDisplayText` calls).
 * Changing the output window temporarily (e.g. using a
 * vtkStringOutputWindow) is another way of capturing messages generated through
 * Python scripts.
 *
 * To capture stdin, especially for non-terminal applications, set CaptureStdin
 * to true. In that case vtkCommand::UpdateEvent will be fired with the calldata
 * being a reference to a vtkStdString that should be filled in with the text to
 * be passed in as the input.
 *
 * A few of the methods on this class implicitly call
 * vtkPythonInterpreter::Initialize() to ensure Python is initialized viz.
 * vtkPythonInterpreter::PyMain() and vtkPythonInterpreter::RunSimpleString().
 * These implicit initialization always calls
 * vtkPythonInterpreter::Initialize(1). If that's not what is expected,
 * ensure that you call vtkPythonInterpreter::Initialize(0) before calling such
 * methods. Refer to Py_InitializeEx() documentation for details on the
 * differences between the two.
 *
 * Notes on calling Initialize/Finalize multiple times: Although applications
 * are free to call Initialize/Finalize pairs multiple times, there are subtle
 * differences between the first Initialize and subsequence Initialize calls
 * after Finalize especially when concerning with imported modules. Refer to
 * Python docs for details. In short, modules like numpy don't continue to work
 * after a re-initialize. Hence use it with caution.
*/

#ifndef vtkPythonInterpreter_h
#define vtkPythonInterpreter_h

#include "vtkObject.h"
#include "vtkPythonInterpreterModule.h" // For export macro
#include "vtkStdString.h"               // needed for vtkStdString.

class VTKPYTHONINTERPRETER_EXPORT vtkPythonInterpreter : public vtkObject
{
public:
  static vtkPythonInterpreter* New();
  vtkTypeMacro(vtkPythonInterpreter, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * Call this method to initialize Python. This has no effect if Python is
   * already initialized. Returns true if Python was initialized by this call,
   * or false if Python was already initialized.
   * Although, one can call Initialize()/Finalize() pair multiple times, Python
   * documentation warns that "Some extensions may not work properly if their
   * initialization routine is called more than once; this can happen if an
   * application calls Py_InitializeEx() and Py_Finalize() more than once."
   */
  static bool Initialize(int initsigs = 1);

  /**
   * Call this method to finalize Python. This has no effect if Python hasn't
   * been initialized already.
   */
  static void Finalize();

  /**
   * Returns true is Python is initialized.
   */
  static bool IsInitialized();

  /**
   * Set the program name. This internally calls `Py_SetProgramName`.
   * Python uses the program name to determine values for prefix and exec_prefix
   * paths that are used to locate Python standard libraries and hence call this
   * if you if you know what you are doing.
   *
   * If not explicitly overridden, `Initialize` will try to guess a good default
   * for the `Py_SetProgramName` to  help find Python standard libraries based
   * on Python libraries used to build VTK.
   */
  static void SetProgramName(const char* programname);

  /**
   * Call this method to start the Python event loop (Py_Main()).
   * This will initialize Python if not already initialized.
   *
   * @note This function handles `--enable-bt` command line argument before
   * passing it on to the `Py_Main(..)` call. Thus, the `--enable-bt` flag is
   * also removed from the arguments passed to `Py_Main`.
   */
  static int PyMain(int argc, char** argv);

  /**
   * Developers are free to call Python C API directly. This convenience method
   * is provided to overcome an issue with the Python interpreter with handling
   * of DOS line endings.
   * This will initialize Python if not already initialized.
   * Returns 0 on success or -1 if a python exception was raised.
   */
  static int RunSimpleString(const char* script);

  /**
   * Prepends the path to the sys.path variable. If Python has been
   * initialized, this call will update the sys.path variable otherwise the same
   * will be done once Python has been initialized. The paths added are saved so
   * that if Python is initialized again (by calls to Initialize()), then these
   * paths will be re-added.
   */
  static void PrependPythonPath(const char*);

  //@{
  /**
   * To capture stdin, especially for non-terminal applications, set CaptureStdin
   * to true. In that case vtkCommand::UpdateEvent will be fired with the calldata
   * being a reference to a vtkStdString that should be filled in with the text to
   * be passed in as the input.
   */
  static void SetCaptureStdin(bool);
  static bool GetCaptureStdin();
  //@}

  static int GetPythonVerboseFlag() { return vtkPythonInterpreter::PythonVerboseFlag; }

protected:
  vtkPythonInterpreter();
  ~vtkPythonInterpreter() override;

  friend struct vtkPythonStdStreamCaptureHelper;

  //@{
  /**
   * Internal methods used by Python. Don't call directly.
   */
  static void WriteStdOut(const char* txt);
  static void FlushStdOut();
  static void WriteStdErr(const char* txt);
  static void FlushStdErr();
  static vtkStdString ReadStdin();
  //@}

private:
  vtkPythonInterpreter(const vtkPythonInterpreter&) = delete;
  void operator=(const vtkPythonInterpreter&) = delete;

  static bool InitializedOnce;
  static bool CaptureStdin;
  /**
   * If true, buffer output to console and sent it to other modules at
   * the end of the operation. If false, send the output as it becomes available.
   */
  static bool ConsoleBuffering;
  //@{
  /**
   * Accumulate here output printed to console by the python interpreter.
   */
  static std::string StdErrBuffer;
  static std::string StdOutBuffer;
  //@}

  /**
   * Since vtkPythonInterpreter is often used outside CPython executable, e.g.
   * vtkpython, the default logic to locate Python standard libraries used by
   * Python (which depends on the executable path) may fail or pickup incorrect
   * Python libs. This methods address the issue by setting program name to help
   * guide Python's default prefix/exec_prefix searching logic.
   */
  static void SetupPythonPrefix();

  /**
   * Add paths to VTK's Python modules.
   */
  static void SetupVTKPythonPaths();

  static int PythonVerboseFlag;
};

// For tracking global interpreters
class VTKPYTHONINTERPRETER_EXPORT vtkPythonGlobalInterpreters
{
public:
  vtkPythonGlobalInterpreters();
  ~vtkPythonGlobalInterpreters();
private:
  vtkPythonGlobalInterpreters(const vtkPythonGlobalInterpreters&);
  vtkPythonGlobalInterpreters& operator=(const vtkPythonGlobalInterpreters&);
};

// This is here to implement the Schwarz counter idiom.
static vtkPythonGlobalInterpreters vtkPythonInterpreters;

#endif
