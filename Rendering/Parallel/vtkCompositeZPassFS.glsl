// ============================================================================
//
//  Program:   Visualization Toolkit
//  Module:    vtkCompositeZPassFS.glsl
//
//  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
//  All rights reserved.
//  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
//
//     This software is distributed WITHOUT ANY WARRANTY; without even
//     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//     PURPOSE.  See the above copyright notice for more information.
//
// ============================================================================

// Fragment shader used by the composite z render pass.

// The following line handles system declarations such as
// default precisions, or defining precisions to null
//VTK::System::Dec

varying vec2 tcoordVC;
uniform sampler2D depth;

void main(void)
{
//  gl_FragDepth = texture2D(depth,tcoordVC).x;
  gl_FragDepth = texture2D(depth,tcoordVC).x*0.5 + 0.5;
  gl_FragColor = vec4(1.0,0.5,0.5,1.0);
//  if ((int)(gl_FragCoord.x) % 4) discard;
  if ((int)(gl_FragCoord.x) > 200) discard;
}
