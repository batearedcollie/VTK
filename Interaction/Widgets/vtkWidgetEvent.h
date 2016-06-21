/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWidgetEvent.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkWidgetEvent - define widget events
// .SECTION Description
// vtkWidgetEvent defines widget events. These events are processed by
// subclasses of vtkInteractorObserver.

#ifndef vtkWidgetEvent_h
#define vtkWidgetEvent_h

#include "vtkInteractionWidgetsModule.h" // For export macro
#include "vtkObject.h"

class VTKINTERACTIONWIDGETS_EXPORT vtkWidgetEvent : public vtkObject
{
public:
  // Description:
  // The object factory constructor.
  static vtkWidgetEvent *New() ;

  // Description:
  // Standard macros.
  vtkTypeMacro(vtkWidgetEvent,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // All the widget events are defined here.
  enum WidgetEventIds {
    NoEvent = 0,
    Select,
    EndSelect,
    Delete,
    Translate,
    EndTranslate,
    Scale,
    EndScale,
    Resize,
    EndResize,
    Rotate,
    EndRotate,
    Move,
    SizeHandles,
    AddPoint,
    AddFinalPoint,
    Completed,
    TimedOut,
    ModifyEvent,
    Reset,
    Up,
    Down,
    Left,
    Right
  };

  // Description:
  // Convenience methods for translating between event names and event ids.
  static const char *GetStringFromEventId(unsigned long event);
  static unsigned long GetEventIdFromString(const char *event);

protected:
  vtkWidgetEvent() {}
  virtual ~vtkWidgetEvent() {}

private:
  vtkWidgetEvent(const vtkWidgetEvent&);  //Not implemented
  void operator=(const vtkWidgetEvent&) VTK_DELETE_FUNCTION;

};

#endif
