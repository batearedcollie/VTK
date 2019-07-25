/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkImplicitCylinderWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkImplicitCylinderWidget.h"
#include "vtkImplicitCylinderRepresentation.h"
#include "vtkCommand.h"
#include "vtkCallbackCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkStdString.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkObjectFactory.h"
#include "vtkWidgetEventTranslator.h"
#include "vtkWidgetCallbackMapper.h"
#include "vtkEvent.h"
#include "vtkWidgetEvent.h"
#include "vtkRenderWindow.h"


vtkStandardNewMacro(vtkImplicitCylinderWidget);

//----------------------------------------------------------------------------
vtkImplicitCylinderWidget::vtkImplicitCylinderWidget()
{
  this->WidgetState = vtkImplicitCylinderWidget::Start;

  // Define widget events
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonPressEvent,
                                          vtkWidgetEvent::Select,
                                          this, vtkImplicitCylinderWidget::SelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::LeftButtonReleaseEvent,
                                          vtkWidgetEvent::EndSelect,
                                          this, vtkImplicitCylinderWidget::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonPressEvent,
                                          vtkWidgetEvent::Translate,
                                          this, vtkImplicitCylinderWidget::TranslateAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MiddleButtonReleaseEvent,
                                          vtkWidgetEvent::EndTranslate,
                                          this, vtkImplicitCylinderWidget::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonPressEvent,
                                          vtkWidgetEvent::Scale,
                                          this, vtkImplicitCylinderWidget::ScaleAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::RightButtonReleaseEvent,
                                          vtkWidgetEvent::EndScale,
                                          this, vtkImplicitCylinderWidget::EndSelectAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::MouseMoveEvent,
                                          vtkWidgetEvent::Move,
                                          this, vtkImplicitCylinderWidget::MoveAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
                                          vtkEvent::AnyModifier, 30, 1, "Up",
                                          vtkWidgetEvent::Up,
                                          this, vtkImplicitCylinderWidget::MoveCylinderAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
                                          vtkEvent::AnyModifier, 28, 1, "Right",
                                          vtkWidgetEvent::Up,
                                          this, vtkImplicitCylinderWidget::MoveCylinderAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
                                          vtkEvent::AnyModifier, 31, 1, "Down",
                                          vtkWidgetEvent::Down,
                                          this, vtkImplicitCylinderWidget::MoveCylinderAction);
  this->CallbackMapper->SetCallbackMethod(vtkCommand::KeyPressEvent,
                                          vtkEvent::AnyModifier, 29, 1, "Left",
                                          vtkWidgetEvent::Down,
                                          this, vtkImplicitCylinderWidget::MoveCylinderAction);

  this->KeyEventCallbackCommand = vtkCallbackCommand::New();
  this->KeyEventCallbackCommand->SetClientData(this);
  this->KeyEventCallbackCommand->SetCallback(vtkImplicitCylinderWidget::ProcessKeyEvents);
}

//----------------------------------------------------------------------------
vtkImplicitCylinderWidget::~vtkImplicitCylinderWidget()
{
  this->KeyEventCallbackCommand->Delete();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::SelectAction(vtkAbstractWidget *w)
{
  vtkImplicitCylinderWidget *self = reinterpret_cast<vtkImplicitCylinderWidget*>(w);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // We want to update the radius, axis and center as appropriate
  reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkImplicitCylinderRepresentation::Moving);
  int interactionState = self->WidgetRep->ComputeInteractionState(X, Y);
  self->UpdateCursorShape(interactionState);

  if ( self->WidgetRep->GetInteractionState() == vtkImplicitCylinderRepresentation::Outside )
  {
    return;
  }

  if (self->Interactor->GetControlKey() &&
    interactionState == vtkImplicitCylinderRepresentation::MovingCenter)
  {
    reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
      SetInteractionState(vtkImplicitCylinderRepresentation::TranslatingCenter);
  }

  // We are definitely selected
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  self->WidgetState = vtkImplicitCylinderWidget::Active;
  self->WidgetRep->StartWidgetInteraction(eventPos);

  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::TranslateAction(vtkAbstractWidget *w)
{
  vtkImplicitCylinderWidget *self = reinterpret_cast<vtkImplicitCylinderWidget*>(w);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // We want to compute an orthogonal vector to the pane that has been selected
  reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkImplicitCylinderRepresentation::Moving);
  int interactionState = self->WidgetRep->ComputeInteractionState(X, Y);
  self->UpdateCursorShape(interactionState);

  if ( self->WidgetRep->GetInteractionState() == vtkImplicitCylinderRepresentation::Outside )
  {
    return;
  }

  // We are definitely selected
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  self->WidgetState = vtkImplicitCylinderWidget::Active;
  self->WidgetRep->StartWidgetInteraction(eventPos);

  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::ScaleAction(vtkAbstractWidget *w)
{
  vtkImplicitCylinderWidget *self = reinterpret_cast<vtkImplicitCylinderWidget*>(w);

  // Get the event position
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];

  // We want to compute an orthogonal vector to the pane that has been selected
  reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkImplicitCylinderRepresentation::Scaling);
  int interactionState = self->WidgetRep->ComputeInteractionState(X, Y);
  self->UpdateCursorShape(interactionState);

  if ( self->WidgetRep->GetInteractionState() == vtkImplicitCylinderRepresentation::Outside )
  {
    return;
  }

  // We are definitely selected
  self->GrabFocus(self->EventCallbackCommand);
  double eventPos[2];
  eventPos[0] = static_cast<double>(X);
  eventPos[1] = static_cast<double>(Y);
  self->WidgetState = vtkImplicitCylinderWidget::Active;
  self->WidgetRep->StartWidgetInteraction(eventPos);

  self->EventCallbackCommand->SetAbortFlag(1);
  self->StartInteraction();
  self->InvokeEvent(vtkCommand::StartInteractionEvent,nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::MoveAction(vtkAbstractWidget *w)
{
  vtkImplicitCylinderWidget *self = reinterpret_cast<vtkImplicitCylinderWidget*>(w);

  // So as to change the cursor shape when the mouse is poised over
  // the widget. Unfortunately, this results in a few extra picks
  // due to the cell picker. However given that its picking simple geometry
  // like the handles/arrows, this should be very quick
  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];
  int changed = 0;

  if (self->ManagesCursor && self->WidgetState != vtkImplicitCylinderWidget::Active)
  {
    int oldInteractionState = reinterpret_cast<vtkImplicitCylinderRepresentation*>(
      self->WidgetRep)->GetInteractionState();

    reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
      SetInteractionState(vtkImplicitCylinderRepresentation::Moving);
    int state = self->WidgetRep->ComputeInteractionState( X, Y );
    changed = self->UpdateCursorShape(state);
    reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
      SetInteractionState(oldInteractionState);
    changed = (changed || state != oldInteractionState) ? 1 : 0;
  }

  // See whether we're active
  if ( self->WidgetState == vtkImplicitCylinderWidget::Start )
  {
    if (changed && self->ManagesCursor)
    {
      self->Render();
    }
    return;
  }

  // Okay, adjust the representation
  double e[2];
  e[0] = static_cast<double>(X);
  e[1] = static_cast<double>(Y);
  self->WidgetRep->WidgetInteraction(e);

  // moving something
  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::InteractionEvent,nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::EndSelectAction(vtkAbstractWidget *w)
{
  vtkImplicitCylinderWidget *self = reinterpret_cast<vtkImplicitCylinderWidget*>(w);

  if ( self->WidgetState != vtkImplicitCylinderWidget::Active  ||
       self->WidgetRep->GetInteractionState() == vtkImplicitCylinderRepresentation::Outside )
  {
    return;
  }

  // Return state to not selected
  double e[2];
  self->WidgetRep->EndWidgetInteraction(e);
  self->WidgetState = vtkImplicitCylinderWidget::Start;
  self->ReleaseFocus();

  // Update cursor if managed
  self->UpdateCursorShape(reinterpret_cast<vtkImplicitCylinderRepresentation*>
    (self->WidgetRep)->GetRepresentationState());

  self->EventCallbackCommand->SetAbortFlag(1);
  self->EndInteraction();
  self->InvokeEvent(vtkCommand::EndInteractionEvent,nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::MoveCylinderAction(vtkAbstractWidget *w)
{
  vtkImplicitCylinderWidget *self = reinterpret_cast<vtkImplicitCylinderWidget*>(w);

  reinterpret_cast<vtkImplicitCylinderRepresentation*>(self->WidgetRep)->
    SetInteractionState(vtkImplicitCylinderRepresentation::Moving);

  int X = self->Interactor->GetEventPosition()[0];
  int Y = self->Interactor->GetEventPosition()[1];
  self->WidgetRep->ComputeInteractionState(X, Y);

  // The cursor must be over part of the widget for these key presses to work
  if ( self->WidgetRep->GetInteractionState() == vtkImplicitCylinderRepresentation::Outside )
  {
    return;
  }

  // Invoke all of the events associated with moving the cylinder
  self->InvokeEvent(vtkCommand::StartInteractionEvent,nullptr);

  // Move the cylinder
  double factor = ( self->Interactor->GetControlKey() ? 0.5 : 1.0);
  if (vtkStdString( self->Interactor->GetKeySym() ) == vtkStdString("Down") ||
      vtkStdString( self->Interactor->GetKeySym() ) == vtkStdString("Left"))
  {
    self->GetCylinderRepresentation()->BumpCylinder(-1,factor);
  }
  else
  {
    self->GetCylinderRepresentation()->BumpCylinder(1,factor);
  }
  self->InvokeEvent(vtkCommand::InteractionEvent,nullptr);

  self->EventCallbackCommand->SetAbortFlag(1);
  self->InvokeEvent(vtkCommand::EndInteractionEvent,nullptr);
  self->Render();
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::SetEnabled(int enabling)
{
  int enabled = this->Enabled;
  // We do this step first because it sets the CurrentRenderer
  this->Superclass::SetEnabled(enabling);

  // We defer enabling the handles until the selection process begins
  if (enabling && !enabled)
  {
    if (this->Parent)
    {
      this->Parent->AddObserver(
        vtkCommand::KeyPressEvent, this->KeyEventCallbackCommand, this->Priority);
      this->Parent->AddObserver(
        vtkCommand::KeyReleaseEvent, this->KeyEventCallbackCommand, this->Priority);
    }
    else
    {
      this->Interactor->AddObserver(
        vtkCommand::KeyPressEvent, this->KeyEventCallbackCommand, this->Priority);
      this->Interactor->AddObserver(
        vtkCommand::KeyReleaseEvent, this->KeyEventCallbackCommand, this->Priority);
    }
  }
  else if (!enabling && enabled)
  {
    if (this->Parent)
    {
      this->Parent->RemoveObserver(this->KeyEventCallbackCommand);
    }
    else
    {
      this->Interactor->RemoveObserver(this->KeyEventCallbackCommand);
    }
  }
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::CreateDefaultRepresentation()
{
  if ( ! this->WidgetRep )
  {
    this->WidgetRep = vtkImplicitCylinderRepresentation::New();
  }
}

//----------------------------------------------------------------------
void vtkImplicitCylinderWidget::
SetRepresentation(vtkImplicitCylinderRepresentation*rep)
{
  this->Superclass::SetWidgetRepresentation(
    reinterpret_cast<vtkWidgetRepresentation*>(rep));
}

//----------------------------------------------------------------------
int vtkImplicitCylinderWidget::UpdateCursorShape( int state )
{
  // So as to change the cursor shape when the mouse is poised over
  // the widget.
  if (this->ManagesCursor)
  {
    if (state == vtkImplicitCylinderRepresentation::Outside)
    {
      return this->RequestCursorShape(VTK_CURSOR_DEFAULT);
    }
    else if (state == vtkImplicitCylinderRepresentation::MovingOutline)
    {
      return this->RequestCursorShape(VTK_CURSOR_SIZEALL);
    }
    else
    {
      return this->RequestCursorShape(VTK_CURSOR_HAND);
    }
  }

  return 0;
}

//----------------------------------------------------------------------------
void vtkImplicitCylinderWidget::ProcessKeyEvents(
  vtkObject*, unsigned long event, void* clientdata, void*)
{
  vtkImplicitCylinderWidget* self = static_cast<vtkImplicitCylinderWidget*>(clientdata);
  vtkRenderWindowInteractor* iren = self->GetInteractor();
  vtkImplicitCylinderRepresentation* rep =
    vtkImplicitCylinderRepresentation::SafeDownCast(self->WidgetRep);
  switch (event)
  {
    case vtkCommand::KeyPressEvent:
      switch (iren->GetKeyCode())
      {
        case 'x':
        case 'X':
          rep->SetXTranslationAxisOn();
          break;
        case 'y':
        case 'Y':
          rep->SetYTranslationAxisOn();
          break;
        case 'z':
        case 'Z':
          rep->SetZTranslationAxisOn();
          break;
        default:
          break;
      }
      break;
    case vtkCommand::KeyReleaseEvent:
      switch (iren->GetKeyCode())
      {
        case 'x':
        case 'X':
        case 'y':
        case 'Y':
        case 'z':
        case 'Z':
          rep->SetTranslationAxisOff();
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

//----------------------------------------------------------------------------
void vtkImplicitCylinderWidget::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
