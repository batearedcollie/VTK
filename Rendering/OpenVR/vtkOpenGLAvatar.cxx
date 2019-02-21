/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLAvatar.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkOpenGLAvatar.h"

#include "vtkBoundingBox.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLActor.h"
#include "vtkOpenGLPolyDataMapper.h"
#include "vtkOpenGLRenderer.h"
#include "vtkOpenGLRenderWindow.h"
#include "vtkOpenGLState.h"
#include "vtkOpenVRCamera.h"
#include "vtkOpenVRRay.h"
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkOpenGLError.h"
#include "vtkRenderWindow.h"
#include "vtkShaderProgram.h"
#include "vtkTexture.h"
#include "vtkTransform.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkAvatarHead.h" // geometry for head
#include "vtkAvatarLeftHand.h" // geometry for hand
#include "vtkAvatarRightHand.h" // geometry for hand
#include "vtkAvatarTorso.h" // geometry for torso
#include "vtkAvatarLeftForeArm.h" // geometry for arm
#include "vtkAvatarRightForeArm.h" // geometry for arm
#include "vtkAvatarLeftUpperArm.h" // geometry for arm
#include "vtkAvatarRightUpperArm.h" // geometry for arm

#include <cmath>

namespace
{
void setOrientation(vtkTransform* trans, const double* orientation)
{
  trans->Identity();
  trans->RotateZ(orientation[2]);
  trans->RotateX(orientation[0]);
  trans->RotateY(orientation[1]);
}

void MultiplyComponents(double a[3], double scale[3])
{
  a[0] *= scale[0];
  a[1] *= scale[1];
  a[2] *= scale[2];
}

// calculate a rotation purely around Vup, using an approximate Vr (right)
// that isn't orthogonal.
// Reverse Vr, front if torso isn't facing the same way as the head.
void getTorsoTransform(vtkTransform* trans, double Vup[3], double inVr[3], double HeadOrientation[3]) {
  double Vr[3] = { inVr[0], inVr[1], inVr[2] };

  // make Vr orthogonal to Vup
  double Vtemp[3] = { Vup[0], Vup[1], Vup[2] };
  vtkMath::MultiplyScalar(Vtemp, vtkMath::Dot(Vup, Vr));
  vtkMath::Subtract(Vr, Vtemp, Vr);
  vtkMath::Normalize(Vr);
  // get third basis vector
  double Vfr[3];
  vtkMath::Cross(Vup, Vr, Vfr);
  // temporarily use trans to test Vfr versus head orientation
  setOrientation(trans, HeadOrientation);
  double Vhead[3] = { 1, 0, 0 };
  trans->TransformPoint(Vhead, Vhead);
  if (vtkMath::Dot(Vfr, Vhead) < 0) {
    // torso is facing behind the head. Swap.
    Vr[0] = -Vr[0];
    Vr[1] = -Vr[1];
    Vr[2] = -Vr[2];
    Vfr[0] = -Vfr[0];
    Vfr[1] = -Vfr[1];
    Vfr[2] = -Vfr[2];
  }
  // make new rotation matrix. Basis vectors form the rotation piece.
  trans->Identity();
  vtkNew<vtkMatrix4x4> mat;
  trans->GetMatrix(mat);
  for (int i = 0; i < 3; ++i) {
    mat->SetElement(i, 0, Vfr[i]);
    mat->SetElement(i, 1, Vup[i]);
    mat->SetElement(i, 2, Vr[i]);
  }
  trans->SetMatrix(mat);
}
}

vtkStandardNewMacro(vtkOpenGLAvatar);

vtkOpenGLAvatar::vtkOpenGLAvatar()
{
  vtkNew<vtkXMLPolyDataReader> reader;
  reader->SetReadFromInputString(true);
  reader->SetInputString(std::string(vtkAvatarHead, vtkAvatarHead + sizeof vtkAvatarHead));
  reader->Update();

  this->HeadMapper->SetInputData(reader->GetOutput());
  this->SetMapper(this->HeadMapper);
  this->HeadActor->SetMapper(this->HeadMapper);

  vtkNew<vtkXMLPolyDataReader> reader2;
  reader2->SetReadFromInputString(true);
  reader2->SetInputString(std::string(vtkAvatarLeftHand, vtkAvatarLeftHand + sizeof vtkAvatarLeftHand));
  reader2->Update();
  this->LeftHandMapper->SetInputData(reader2->GetOutput());
  this->LeftHandActor->SetMapper(this->LeftHandMapper);

  vtkNew<vtkXMLPolyDataReader> reader3;
  reader3->SetReadFromInputString(true);
  reader3->SetInputString(std::string(vtkAvatarRightHand, vtkAvatarRightHand + sizeof vtkAvatarRightHand));
  reader3->Update();
  this->RightHandMapper->SetInputData(reader3->GetOutput());
  this->RightHandActor->SetMapper(this->RightHandMapper);

  const unsigned char *models[NUM_BODY] = {
    vtkAvatarTorso,
    vtkAvatarLeftForeArm,
    vtkAvatarRightForeArm,
    vtkAvatarLeftUpperArm,
    vtkAvatarRightUpperArm,
  };
  size_t modelSize[NUM_BODY] = {
    sizeof vtkAvatarTorso,
    sizeof vtkAvatarLeftForeArm,
    sizeof vtkAvatarRightForeArm,
    sizeof vtkAvatarLeftUpperArm,
    sizeof vtkAvatarRightUpperArm,
  };

  this->GetProperty()->SetDiffuse(0.7);
  this->GetProperty()->SetAmbient(0.3);
  this->GetProperty()->SetSpecular(0.0);
  // link properties, share color.
  this->HeadActor->SetProperty(this->GetProperty());
  this->LeftHandActor->SetProperty(this->GetProperty());
  this->RightHandActor->SetProperty(this->GetProperty());

  for (int i = 0; i < NUM_BODY; ++i) {
    if (!models[i]) continue;
    vtkNew<vtkXMLPolyDataReader> reader4;
    reader4->SetReadFromInputString(true);
    reader4->SetInputString(std::string(models[i], models[i] + modelSize[i]));
    reader4->Update();
    this->BodyMapper[i]->SetInputData(reader4->GetOutput());
    this->BodyActor[i]->SetMapper(this->BodyMapper[i]);

    this->BodyActor[i]->SetProperty(this->GetProperty());
  }

  // this->LeftRay->SetShow(true);
  // this->RightRay->SetShow(true);

}

vtkOpenGLAvatar::~vtkOpenGLAvatar() = default;

// Actual Avatar render method.
void vtkOpenGLAvatar::Render(vtkRenderer *ren, vtkMapper *mapper)
{
  vtkOpenGLClearErrorMacro();

  this->CalcBody();

  this->HeadActor->SetScale(this->GetScale());
  this->HeadActor->SetPosition(this->HeadPosition);
  this->HeadActor->SetOrientation(this->HeadOrientation);
  this->LeftHandActor->SetScale(this->GetScale());
  this->LeftHandActor->SetPosition(this->LeftHandPosition);
  this->LeftHandActor->SetOrientation(this->LeftHandOrientation);
  this->RightHandActor->SetScale(this->GetScale());
  this->RightHandActor->SetPosition(this->RightHandPosition);
  this->RightHandActor->SetOrientation(this->RightHandOrientation);


  // send a render to the mapper; update pipeline
  if (this->HeadActor->GetVisibility())
  {
    mapper->Render(ren, this->HeadActor);
  }
  if (this->LeftHandActor->GetVisibility())
  {
    this->LeftHandMapper->Render(ren, this->LeftHandActor);
  }
  if (this->RightHandActor->GetVisibility())
  {
    this->RightHandMapper->Render(ren, this->RightHandActor);
  }
  for (int i = 0; i < NUM_BODY; ++i) {
    this->BodyActor[i]->SetScale(this->GetScale());
    this->BodyActor[i]->SetPosition(this->BodyPosition[i]);
    this->BodyActor[i]->SetOrientation(this->BodyOrientation[i]);
    if (this->BodyActor[i]->GetVisibility())
    {
      this->BodyMapper[i]->Render(ren, this->BodyActor[i]);
    }
  }
  if (this->LeftRay->GetShow() || this->RightRay->GetShow())
  {
    auto renWin = vtkOpenGLRenderWindow::SafeDownCast(ren->GetRenderWindow());
    vtkOpenVRCamera *cam =
      static_cast<vtkOpenVRCamera *>(ren->GetActiveCamera());
    if (renWin && cam)
    {

      vtkNew<vtkTransform> trans;
      vtkNew<vtkMatrix4x4> mat;
      vtkMatrix4x4 *wcdc; // we only need this one.
      vtkMatrix4x4 *wcvc;
      vtkMatrix3x3 *norms;
      vtkMatrix4x4 *vcdc;
      // VRRay needs the complete model -> device (screen) transform.
      // Get world -> device from the camera.
      cam->GetKeyMatrices(ren,wcvc,norms,vcdc,wcdc);
      vtkNew<vtkMatrix4x4> controller2device;

      if (this->LeftRay->GetShow())
      {
        trans->Translate(this->LeftHandPosition);
        //RotateZ, RotateX, and finally RotateY
        trans->RotateZ(this->LeftHandOrientation[2]);
        trans->RotateX(this->LeftHandOrientation[0]);
        trans->RotateY(this->LeftHandOrientation[1]);
        // VRRay and avatar are off by 90.
        trans->RotateY(-90);
        trans->GetMatrix(mat);
        // OpenGL expects transpose of VTK transforms.
        mat->Transpose();
        trans->SetMatrix(mat);
        vtkMatrix4x4::Multiply4x4(trans->GetMatrix(), wcdc, controller2device);
        this->LeftRay->Render(renWin, controller2device);
      }
      if (this->RightRay->GetShow())
      {
        trans->Translate(this->RightHandPosition);
        trans->RotateZ(this->RightHandOrientation[2]);
        trans->RotateX(this->RightHandOrientation[0]);
        trans->RotateY(this->RightHandOrientation[1]);
        trans->RotateY(-90);
        trans->GetMatrix(mat);
        mat->Transpose();
        trans->SetMatrix(mat);
        vtkMatrix4x4::Multiply4x4(trans->GetMatrix(), wcdc, controller2device);
        this->RightRay->Render(renWin, controller2device);
      }
    }
  }
  vtkOpenGLCheckErrorMacro("failed after Render");
}

void vtkOpenGLAvatar::CalcBody()
{
  this->BodyPosition[TORSO][0] = this->HeadPosition[0];
  this->BodyPosition[TORSO][1] = this->HeadPosition[1];
  this->BodyPosition[TORSO][2] = this->HeadPosition[2];

  // keep the head orientation in the direction of the up vector.
  // use the vector between the hands as a guide for torso's rotation (Vright).
  double torsoRight[3] = {0, 0, 0};
  if (this->UseLeftHand && this->UseRightHand) {
    vtkMath::Subtract(this->RightHandPosition, this->LeftHandPosition, torsoRight);
  }
  else if (this->UseLeftHand)
  {
    vtkMath::Subtract(this->HeadPosition, this->LeftHandPosition, torsoRight);
  }
  else if (this->UseRightHand)
  {
    vtkMath::Subtract(this->RightHandPosition, this->HeadPosition, torsoRight);
  }
  // else no hands, and torsoRight remains zero.

  vtkNew<vtkTransform> trans;
  getTorsoTransform(trans, this->UpVector, torsoRight, this->HeadOrientation);

  trans->GetOrientation(this->BodyOrientation[TORSO]);

  // Initial try - keep forearm rigidly attached to hand.
  this->BodyPosition[LEFT_FORE][0] = this->LeftHandPosition[0];
  this->BodyPosition[LEFT_FORE][1] = this->LeftHandPosition[1];
  this->BodyPosition[LEFT_FORE][2] = this->LeftHandPosition[2];

  this->BodyOrientation[LEFT_FORE][0] = this->LeftHandOrientation[0];
  this->BodyOrientation[LEFT_FORE][1] = this->LeftHandOrientation[1];
  this->BodyOrientation[LEFT_FORE][2] = this->LeftHandOrientation[2];

  this->BodyPosition[RIGHT_FORE][0] = this->RightHandPosition[0];
  this->BodyPosition[RIGHT_FORE][1] = this->RightHandPosition[1];
  this->BodyPosition[RIGHT_FORE][2] = this->RightHandPosition[2];

  this->BodyOrientation[RIGHT_FORE][0] = this->RightHandOrientation[0];
  this->BodyOrientation[RIGHT_FORE][1] = this->RightHandOrientation[1];
  this->BodyOrientation[RIGHT_FORE][2] = this->RightHandOrientation[2];

  // Attach upper arm at shoulder, and rotate to hit the end of the forearm.
  // end of forearm, relative to the hand at 0, is elbow pos.
  double shoulderPos[3] = {-0.138, -0.53, -0.60};
  double scale[3];
  this->GetScale(scale);
  setOrientation(trans, this->BodyOrientation[TORSO]);
  // calculate relative left shoulder position (to torso)
  MultiplyComponents(shoulderPos, scale);
  trans->TransformPoint(shoulderPos, this->BodyPosition[LEFT_UPPER]);

  // move with torso
  this->BodyPosition[LEFT_UPPER][0] += this->BodyPosition[TORSO][0];
  this->BodyPosition[LEFT_UPPER][1] += this->BodyPosition[TORSO][1];
  this->BodyPosition[LEFT_UPPER][2] += this->BodyPosition[TORSO][2];

  shoulderPos[2] = +0.60 * scale[2];
  // calculate relative right shoulder position (to torso)
  trans->TransformPoint(shoulderPos, this->BodyPosition[RIGHT_UPPER]);

  // move with torso
  this->BodyPosition[RIGHT_UPPER][0] += this->BodyPosition[TORSO][0];
  this->BodyPosition[RIGHT_UPPER][1] += this->BodyPosition[TORSO][1];
  this->BodyPosition[RIGHT_UPPER][2] += this->BodyPosition[TORSO][2];

  // orient the upper left arm to aim at the elbow.
  double leftElbowPos[3] = {-0.85, 0.02, 0};
  setOrientation(trans, this->LeftHandOrientation);
  MultiplyComponents(leftElbowPos, scale);
  trans->TransformPoint(leftElbowPos, leftElbowPos);
  leftElbowPos[0] += this->LeftHandPosition[0];
  leftElbowPos[1] += this->LeftHandPosition[1];
  leftElbowPos[2] += this->LeftHandPosition[2];
  // upper-arm extends along +x at zero rotation. rotate (1,0,0) to
  // vector between shoulder and elbow.
  double leftUpperDir[3], cross[3], startDir[3] = { 1, 0, 0 };
  vtkMath::Subtract(leftElbowPos, this->BodyPosition[LEFT_UPPER], leftUpperDir);
  vtkMath::Cross(startDir, leftUpperDir, cross);
  vtkMath::Normalize(cross);
  double angle = vtkMath::AngleBetweenVectors(startDir, leftUpperDir) * 180 / vtkMath::Pi();
  trans->Identity();
  trans->RotateWXYZ(angle, cross);
  trans->GetOrientation(this->BodyOrientation[LEFT_UPPER]);

  // now the right upper arm
  double rightElbowPos[3] = {-0.85, 0.02, 0};
  setOrientation(trans, this->RightHandOrientation);
  MultiplyComponents(rightElbowPos, scale);
  trans->TransformPoint(rightElbowPos, rightElbowPos);
  rightElbowPos[0] += this->RightHandPosition[0];
  rightElbowPos[1] += this->RightHandPosition[1];
  rightElbowPos[2] += this->RightHandPosition[2];
  // upper-arm extends along +x at zero rotation. rotate (1,0,0) to
  // vector between shoulder and elbow.
  double rightUpperDir[3];
  vtkMath::Subtract(rightElbowPos, this->BodyPosition[RIGHT_UPPER], rightUpperDir);
  vtkMath::Cross(startDir, rightUpperDir, cross);
  vtkMath::Normalize(cross);
  angle = vtkMath::AngleBetweenVectors(startDir, rightUpperDir) * 180 / vtkMath::Pi();
  trans->Identity();
  trans->RotateWXYZ(angle, cross);
  trans->GetOrientation(this->BodyOrientation[RIGHT_UPPER]);

}

// Multiple sub-actors require a custom bounding box calc.
double *vtkOpenGLAvatar::GetBounds()
{
  vtkDebugMacro( << "Getting Bounds" );
  vtkBoundingBox bbox;

  bbox.AddBounds(this->HeadActor->GetBounds());
  bbox.AddBounds(this->RightHandActor->GetBounds());
  bbox.AddBounds(this->LeftHandActor->GetBounds());
  for (int i = 0; i < NUM_BODY; ++i) {
    bbox.AddBounds(this->BodyActor[i]->GetBounds());
  }

  bbox.GetBounds(this->Bounds);
  return this->Bounds;
}

//----------------------------------------------------------------------------
void vtkOpenGLAvatar::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}

void vtkOpenGLAvatar::SetUseLeftHand(bool val)
{
  this->Superclass::SetUseLeftHand(val);
  this->LeftHandActor->SetVisibility(val);
  this->BodyActor[LEFT_FORE]->SetVisibility(val);
  bool upperViz = val && !this->ShowHandsOnly;
  this->BodyActor[LEFT_UPPER]->SetVisibility(upperViz);
}
void vtkOpenGLAvatar::SetUseRightHand(bool val)
{
  this->Superclass::SetUseRightHand(val);
  this->RightHandActor->SetVisibility(val);
  this->BodyActor[RIGHT_FORE]->SetVisibility(val);
  bool upperViz = val && !this->ShowHandsOnly;
  this->BodyActor[RIGHT_UPPER]->SetVisibility(upperViz);
}

void vtkOpenGLAvatar::SetShowHandsOnly(bool val)
{
  std::cout << "Hiding parts" << std::endl;
  this->Superclass::SetShowHandsOnly(val);
  this->HeadActor->SetVisibility(!val);
  this->BodyActor[TORSO]->SetVisibility(!val);
  bool upperViz = !val && this->BodyActor[LEFT_UPPER]->GetVisibility();
  this->BodyActor[LEFT_UPPER]->SetVisibility(upperViz);
  upperViz = !val && this->BodyActor[RIGHT_UPPER]->GetVisibility();
  this->BodyActor[RIGHT_UPPER]->SetVisibility(upperViz);
}

void vtkOpenGLAvatar::SetLeftShowRay(bool val)
{
  this->LeftRay->SetShow(val);
}

void vtkOpenGLAvatar::SetRightShowRay(bool val)
{
  this->RightRay->SetShow(val);
}
