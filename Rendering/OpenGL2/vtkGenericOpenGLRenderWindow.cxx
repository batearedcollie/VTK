/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGenericRenderWindowInteractor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGenericOpenGLRenderWindow.h"

#include "vtkCommand.h"
#include "vtkLogger.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLError.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLRenderer.h"
#include "vtkRendererCollection.h"

vtkStandardNewMacro(vtkGenericOpenGLRenderWindow);

vtkGenericOpenGLRenderWindow::vtkGenericOpenGLRenderWindow()
{
  this->ReadyForRendering = true;
  this->DirectStatus = 0;
  this->CurrentStatus = false;
  this->SupportsOpenGLStatus = 0;
  this->ForceMaximumHardwareLineWidth = 0;
  this->ScreenSize[0] = 0;
  this->ScreenSize[1] = 0;
}

vtkGenericOpenGLRenderWindow::~vtkGenericOpenGLRenderWindow()
{
  this->Finalize();

  vtkRenderer* ren;
  vtkCollectionSimpleIterator rit;
  this->Renderers->InitTraversal(rit);
  while ((ren = this->Renderers->GetNextRenderer(rit)))
  {
    ren->SetRenderWindow(nullptr);
  }
}

void vtkGenericOpenGLRenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

float vtkGenericOpenGLRenderWindow::GetMaximumHardwareLineWidth()
{
  return this->ForceMaximumHardwareLineWidth > 0
    ? this->ForceMaximumHardwareLineWidth
    : this->Superclass::GetMaximumHardwareLineWidth();
}

void vtkGenericOpenGLRenderWindow::SetFrontBuffer(unsigned int b)
{
  this->FrontBuffer = b;
}

void vtkGenericOpenGLRenderWindow::SetFrontLeftBuffer(unsigned int b)
{
  this->FrontLeftBuffer = b;
}

void vtkGenericOpenGLRenderWindow::SetFrontRightBuffer(unsigned int b)
{
  this->FrontRightBuffer = b;
}

void vtkGenericOpenGLRenderWindow::SetBackBuffer(unsigned int b)
{
  this->BackBuffer = b;
}

void vtkGenericOpenGLRenderWindow::SetBackLeftBuffer(unsigned int b)
{
  this->BackLeftBuffer = b;
}

void vtkGenericOpenGLRenderWindow::SetBackRightBuffer(unsigned int b)
{
  this->BackRightBuffer = b;
}

void vtkGenericOpenGLRenderWindow::SetDefaultFrameBufferId(unsigned int id)
{
  this->DefaultFrameBufferId = id;
}

void vtkGenericOpenGLRenderWindow::SetOwnContext(int val)
{
  this->OwnContext = val;
}

void vtkGenericOpenGLRenderWindow::Finalize()
{
  // tell each of the renderers that this render window/graphics context
  // is being removed (the RendererCollection is removed by vtkRenderWindow's
  // destructor)
  this->ReleaseGraphicsResources(this);
}

void vtkGenericOpenGLRenderWindow::Frame()
{
  this->InvokeEvent(vtkCommand::WindowFrameEvent, nullptr);
}

void vtkGenericOpenGLRenderWindow::MakeCurrent()
{
  this->InvokeEvent(vtkCommand::WindowMakeCurrentEvent, nullptr);
}

bool vtkGenericOpenGLRenderWindow::IsCurrent()
{
  this->InvokeEvent(vtkCommand::WindowIsCurrentEvent, &this->CurrentStatus);
  return this->CurrentStatus;
}

int vtkGenericOpenGLRenderWindow::SupportsOpenGL()
{
  this->InvokeEvent(vtkCommand::WindowSupportsOpenGLEvent,
                    &this->SupportsOpenGLStatus);
  return this->SupportsOpenGLStatus;
}

int vtkGenericOpenGLRenderWindow::IsDirect()
{
  this->InvokeEvent(vtkCommand::WindowIsDirectEvent, &this->DirectStatus);
  return this->DirectStatus;
}

void vtkGenericOpenGLRenderWindow::SetWindowId(void*)
{
}

void* vtkGenericOpenGLRenderWindow::GetGenericWindowId()
{
  return nullptr;
}

void vtkGenericOpenGLRenderWindow::SetDisplayId(void*)
{
}

void vtkGenericOpenGLRenderWindow::SetParentId(void*)
{
}

void* vtkGenericOpenGLRenderWindow::GetGenericDisplayId()
{
  return nullptr;
}

void* vtkGenericOpenGLRenderWindow::GetGenericParentId()
{
  return nullptr;
}

void* vtkGenericOpenGLRenderWindow::GetGenericContext()
{
  return nullptr;
}

void* vtkGenericOpenGLRenderWindow::GetGenericDrawable()
{
  return nullptr;
}

void vtkGenericOpenGLRenderWindow::SetWindowInfo(const char*)
{
}

void vtkGenericOpenGLRenderWindow::SetParentInfo(const char*)
{
}

int* vtkGenericOpenGLRenderWindow::GetScreenSize()
{
  return this->ScreenSize;
}

void vtkGenericOpenGLRenderWindow::HideCursor()
{
}

void vtkGenericOpenGLRenderWindow::ShowCursor()
{
}

void vtkGenericOpenGLRenderWindow::SetFullScreen(vtkTypeBool)
{
}

void vtkGenericOpenGLRenderWindow::WindowRemap()
{
}

int vtkGenericOpenGLRenderWindow::GetEventPending()
{
  return 0;
}

void vtkGenericOpenGLRenderWindow::SetNextWindowId(void*)
{
}

void vtkGenericOpenGLRenderWindow::SetNextWindowInfo(const char*)
{
}

void vtkGenericOpenGLRenderWindow::CreateAWindow()
{
}

void vtkGenericOpenGLRenderWindow::DestroyWindow()
{
}

void vtkGenericOpenGLRenderWindow::SetIsDirect(int newValue)
{
  this->DirectStatus = newValue;
}

void vtkGenericOpenGLRenderWindow::SetSupportsOpenGL(int newValue)
{
  this->SupportsOpenGLStatus = newValue;
}

void vtkGenericOpenGLRenderWindow::SetIsCurrent(bool newValue)
{
  this->CurrentStatus = newValue;
}

void vtkGenericOpenGLRenderWindow::Render()
{
  if (this->ReadyForRendering)
  {
    this->MakeCurrent();
    if (!this->IsCurrent())
    {
      vtkLogF(TRACE, "rendering skipped since `MakeCurrent` was not successful.");
    }
    else
    {
      // Query current GL state and store them
      this->SaveGLState();

      this->Superclass::Render();

      // Restore state to previous known value
      this->RestoreGLState();
    }
  }
}

void vtkGenericOpenGLRenderWindow::SetIsPicking(vtkTypeBool isPicking)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this
                << "): setting IsPicking to " << isPicking);
  if (this->IsPicking != isPicking)
  {
    this->IsPicking = isPicking;
    this->Modified();
    this->InvokeEvent(this->IsPicking ? vtkCommand::StartPickEvent
                                      : vtkCommand::EndPickEvent,
                      nullptr);
  }
}

void vtkGenericOpenGLRenderWindow::SetCurrentCursor(int cShape)
{
  vtkDebugMacro(<< this->GetClassName() << " (" << this
                << "): setting current Cursor to " << cShape);
  if (this->GetCurrentCursor() != cShape)
  {
    this->CurrentCursor = cShape;
    this->Modified();
    this->InvokeEvent(vtkCommand::CursorChangedEvent, &cShape);
  }
}

int vtkGenericOpenGLRenderWindow::ReadPixels(
  const vtkRecti& rect, int front, int glFormat, int glType, void* data, int right)
{
  if (this->ReadyForRendering)
  {
    return this->Superclass::ReadPixels(rect, front, glFormat, glType, data, right);
  }

  vtkWarningMacro("`ReadPixels` called before window is ready for rendering; ignoring.");
  return VTK_ERROR;
}
