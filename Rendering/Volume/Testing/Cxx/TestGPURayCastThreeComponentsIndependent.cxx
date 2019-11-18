/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestGPURayCastThreeComponentsIndependent.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Description
// This test creates a vtkImageData with three components.
// The data is volume rendered considering the three components as independent.

#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSphere.h"
#include "vtkTestUtilities.h"
#include "vtkTesting.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"

static const char* TestGPURayCastThreeComponentsIndependentLog =
  "# StreamVersion 1\n"
  "EnterEvent 282 37 0 0 0 0 0\n"
  "MouseMoveEvent 282 37 0 0 0 0 0\n"
  "MouseMoveEvent 263 37 0 0 0 0 0\n"
  "MouseMoveEvent 244 42 0 0 0 0 0\n"
  "MouseMoveEvent 231 44 0 0 0 0 0\n"
  "MouseMoveEvent 222 47 0 0 0 0 0\n"
  "MouseMoveEvent 213 49 0 0 0 0 0\n"
  "MouseMoveEvent 212 50 0 0 0 0 0\n"
  "MouseMoveEvent 211 50 0 0 0 0 0\n"
  "MouseMoveEvent 210 51 0 0 0 0 0\n"
  "MouseMoveEvent 209 51 0 0 0 0 0\n"
  "MouseMoveEvent 207 52 0 0 0 0 0\n"
  "MouseMoveEvent 205 54 0 0 0 0 0\n"
  "MouseMoveEvent 203 56 0 0 0 0 0\n"
  "MouseMoveEvent 200 59 0 0 0 0 0\n"
  "MouseMoveEvent 198 61 0 0 0 0 0\n"
  "MouseMoveEvent 197 62 0 0 0 0 0\n"
  "MouseMoveEvent 196 63 0 0 0 0 0\n"
  "MouseMoveEvent 195 64 0 0 0 0 0\n"
  "MouseMoveEvent 192 67 0 0 0 0 0\n"
  "MouseMoveEvent 189 70 0 0 0 0 0\n"
  "MouseMoveEvent 186 72 0 0 0 0 0\n"
  "MouseMoveEvent 185 73 0 0 0 0 0\n"
  "MouseMoveEvent 183 74 0 0 0 0 0\n"
  "MouseMoveEvent 183 76 0 0 0 0 0\n"
  "MouseMoveEvent 183 79 0 0 0 0 0\n"
  "MouseMoveEvent 183 81 0 0 0 0 0\n"
  "MouseMoveEvent 183 82 0 0 0 0 0\n"
  "MouseMoveEvent 183 83 0 0 0 0 0\n"
  "MouseMoveEvent 183 84 0 0 0 0 0\n"
  "MouseMoveEvent 183 85 0 0 0 0 0\n"
  "MouseMoveEvent 183 86 0 0 0 0 0\n"
  "MouseMoveEvent 183 88 0 0 0 0 0\n"
  "MouseMoveEvent 183 91 0 0 0 0 0\n"
  "MouseMoveEvent 184 93 0 0 0 0 0\n"
  "MouseMoveEvent 184 94 0 0 0 0 0\n"
  "MouseMoveEvent 184 95 0 0 0 0 0\n"
  "MouseMoveEvent 185 96 0 0 0 0 0\n"
  "MouseMoveEvent 185 97 0 0 0 0 0\n"
  "MouseMoveEvent 186 99 0 0 0 0 0\n"
  "MouseMoveEvent 187 100 0 0 0 0 0\n"
  "MouseMoveEvent 188 101 0 0 0 0 0\n"
  "MouseMoveEvent 188 102 0 0 0 0 0\n"
  "MouseMoveEvent 189 104 0 0 0 0 0\n"
  "MouseMoveEvent 191 107 0 0 0 0 0\n"
  "MouseMoveEvent 192 109 0 0 0 0 0\n"
  "MouseMoveEvent 193 111 0 0 0 0 0\n"
  "MouseMoveEvent 193 112 0 0 0 0 0\n"
  "MouseMoveEvent 194 114 0 0 0 0 0\n"
  "MouseMoveEvent 195 115 0 0 0 0 0\n"
  "MouseMoveEvent 195 116 0 0 0 0 0\n"
  "MouseMoveEvent 195 117 0 0 0 0 0\n"
  "MouseWheelForwardEvent 195 117 0 0 0 0 0\n"
  "StartInteractionEvent 195 117 0 0 0 0 0\n"
  "RenderEvent 195 117 0 0 0 0 0\n"
  "EndInteractionEvent 195 117 0 0 0 0 0\n"
  "RenderEvent 195 117 0 0 0 0 0\n"
  "MouseWheelForwardEvent 195 117 0 0 0 0 0\n"
  "StartInteractionEvent 195 117 0 0 0 0 0\n"
  "RenderEvent 195 117 0 0 0 0 0\n"
  "EndInteractionEvent 195 117 0 0 0 0 0\n"
  "RenderEvent 195 117 0 0 0 0 0\n"
  "MouseMoveEvent 195 118 0 0 0 0 0\n"
  "MouseMoveEvent 195 119 0 0 0 0 0\n"
  "MouseMoveEvent 194 121 0 0 0 0 0\n"
  "MouseMoveEvent 193 122 0 0 0 0 0\n"
  "MouseMoveEvent 192 123 0 0 0 0 0\n"
  "MouseMoveEvent 190 125 0 0 0 0 0\n"
  "MouseMoveEvent 187 127 0 0 0 0 0\n"
  "MouseMoveEvent 184 130 0 0 0 0 0\n"
  "MouseMoveEvent 177 137 0 0 0 0 0\n"
  "MouseMoveEvent 169 145 0 0 0 0 0\n"
  "MouseMoveEvent 161 153 0 0 0 0 0\n"
  "MouseMoveEvent 155 157 0 0 0 0 0\n"
  "MouseMoveEvent 150 162 0 0 0 0 0\n"
  "MouseMoveEvent 144 167 0 0 0 0 0\n"
  "MouseMoveEvent 139 171 0 0 0 0 0\n"
  "MouseMoveEvent 137 175 0 0 0 0 0\n"
  "MouseMoveEvent 134 179 0 0 0 0 0\n"
  "MouseMoveEvent 133 182 0 0 0 0 0\n"
  "MouseMoveEvent 133 184 0 0 0 0 0\n"
  "MouseMoveEvent 132 185 0 0 0 0 0\n"
  "MouseMoveEvent 131 187 0 0 0 0 0\n"
  "MouseMoveEvent 130 189 0 0 0 0 0\n"
  "MouseMoveEvent 129 190 0 0 0 0 0\n"
  "LeftButtonPressEvent 129 190 0 0 0 0 0\n"
  "StartInteractionEvent 129 190 0 0 0 0 0\n"
  "MouseMoveEvent 130 190 0 0 0 0 0\n"
  "RenderEvent 130 190 0 0 0 0 0\n"
  "InteractionEvent 130 190 0 0 0 0 0\n"
  "MouseMoveEvent 135 185 0 0 0 0 0\n"
  "RenderEvent 135 185 0 0 0 0 0\n"
  "InteractionEvent 135 185 0 0 0 0 0\n"
  "MouseMoveEvent 138 179 0 0 0 0 0\n"
  "RenderEvent 138 179 0 0 0 0 0\n"
  "InteractionEvent 138 179 0 0 0 0 0\n"
  "MouseMoveEvent 147 169 0 0 0 0 0\n"
  "RenderEvent 147 169 0 0 0 0 0\n"
  "InteractionEvent 147 169 0 0 0 0 0\n"
  "MouseMoveEvent 154 161 0 0 0 0 0\n"
  "RenderEvent 154 161 0 0 0 0 0\n"
  "InteractionEvent 154 161 0 0 0 0 0\n"
  "MouseMoveEvent 158 154 0 0 0 0 0\n"
  "RenderEvent 158 154 0 0 0 0 0\n"
  "InteractionEvent 158 154 0 0 0 0 0\n"
  "MouseMoveEvent 163 147 0 0 0 0 0\n"
  "RenderEvent 163 147 0 0 0 0 0\n"
  "InteractionEvent 163 147 0 0 0 0 0\n"
  "MouseMoveEvent 168 136 0 0 0 0 0\n"
  "RenderEvent 168 136 0 0 0 0 0\n"
  "InteractionEvent 168 136 0 0 0 0 0\n"
  "MouseMoveEvent 177 121 0 0 0 0 0\n"
  "RenderEvent 177 121 0 0 0 0 0\n"
  "InteractionEvent 177 121 0 0 0 0 0\n"
  "MouseMoveEvent 184 109 0 0 0 0 0\n"
  "RenderEvent 184 109 0 0 0 0 0\n"
  "InteractionEvent 184 109 0 0 0 0 0\n"
  "MouseMoveEvent 189 97 0 0 0 0 0\n"
  "RenderEvent 189 97 0 0 0 0 0\n"
  "InteractionEvent 189 97 0 0 0 0 0\n"
  "MouseMoveEvent 197 84 0 0 0 0 0\n"
  "RenderEvent 197 84 0 0 0 0 0\n"
  "InteractionEvent 197 84 0 0 0 0 0\n"
  "MouseMoveEvent 201 74 0 0 0 0 0\n"
  "RenderEvent 201 74 0 0 0 0 0\n"
  "InteractionEvent 201 74 0 0 0 0 0\n"
  "MouseMoveEvent 206 66 0 0 0 0 0\n"
  "RenderEvent 206 66 0 0 0 0 0\n"
  "InteractionEvent 206 66 0 0 0 0 0\n"
  "MouseMoveEvent 211 54 0 0 0 0 0\n"
  "RenderEvent 211 54 0 0 0 0 0\n"
  "InteractionEvent 211 54 0 0 0 0 0\n"
  "MouseMoveEvent 219 41 0 0 0 0 0\n"
  "RenderEvent 219 41 0 0 0 0 0\n"
  "InteractionEvent 219 41 0 0 0 0 0\n"
  "MouseMoveEvent 223 34 0 0 0 0 0\n"
  "RenderEvent 223 34 0 0 0 0 0\n"
  "InteractionEvent 223 34 0 0 0 0 0\n"
  "MouseMoveEvent 230 24 0 0 0 0 0\n"
  "RenderEvent 230 24 0 0 0 0 0\n"
  "InteractionEvent 230 24 0 0 0 0 0\n"
  "MouseMoveEvent 234 17 0 0 0 0 0\n"
  "RenderEvent 234 17 0 0 0 0 0\n"
  "InteractionEvent 234 17 0 0 0 0 0\n"
  "MouseMoveEvent 236 15 0 0 0 0 0\n"
  "RenderEvent 236 15 0 0 0 0 0\n"
  "InteractionEvent 236 15 0 0 0 0 0\n"
  "MouseMoveEvent 239 10 0 0 0 0 0\n"
  "RenderEvent 239 10 0 0 0 0 0\n"
  "InteractionEvent 239 10 0 0 0 0 0\n"
  "LeftButtonReleaseEvent 239 10 0 0 0 0 0\n"
  "EndInteractionEvent 239 10 0 0 0 0 0\n"
  "RenderEvent 239 10 0 0 0 0 0\n"
  "MouseMoveEvent 239 11 0 0 0 0 0\n"
  "MouseMoveEvent 239 12 0 0 0 0 0\n"
  "MouseMoveEvent 239 14 0 0 0 0 0\n"
  "MouseMoveEvent 239 15 0 0 0 0 0\n"
  "MouseMoveEvent 237 19 0 0 0 0 0\n"
  "MouseMoveEvent 234 24 0 0 0 0 0\n"
  "MouseMoveEvent 228 35 0 0 0 0 0\n"
  "MouseMoveEvent 220 48 0 0 0 0 0\n"
  "MouseMoveEvent 209 62 0 0 0 0 0\n"
  "MouseMoveEvent 198 77 0 0 0 0 0\n"
  "MouseMoveEvent 187 92 0 0 0 0 0\n"
  "MouseMoveEvent 176 107 0 0 0 0 0\n"
  "MouseMoveEvent 165 120 0 0 0 0 0\n"
  "MouseMoveEvent 156 133 0 0 0 0 0\n"
  "MouseMoveEvent 147 146 0 0 0 0 0\n"
  "MouseMoveEvent 140 155 0 0 0 0 0\n"
  "MouseMoveEvent 133 164 0 0 0 0 0\n"
  "MouseMoveEvent 126 173 0 0 0 0 0\n"
  "MouseMoveEvent 119 182 0 0 0 0 0\n"
  "MouseMoveEvent 112 191 0 0 0 0 0\n"
  "MouseMoveEvent 107 198 0 0 0 0 0\n"
  "MouseMoveEvent 102 205 0 0 0 0 0\n"
  "MouseMoveEvent 98 212 0 0 0 0 0\n"
  "MouseMoveEvent 98 213 0 0 0 0 0\n"
  "MouseMoveEvent 96 214 0 0 0 0 0\n"
  "MouseMoveEvent 96 215 0 0 0 0 0\n"
  "MouseMoveEvent 96 217 0 0 0 0 0\n"
  "MouseMoveEvent 96 218 0 0 0 0 0\n"
  "MouseMoveEvent 96 219 0 0 0 0 0\n"
  "MouseMoveEvent 96 220 0 0 0 0 0\n"
  "MouseMoveEvent 97 221 0 0 0 0 0\n"
  "MouseMoveEvent 98 221 0 0 0 0 0\n"
  "MouseMoveEvent 99 221 0 0 0 0 0\n"
  "MouseMoveEvent 101 221 0 0 0 0 0\n"
  "MouseMoveEvent 103 220 0 0 0 0 0\n"
  "MouseMoveEvent 104 219 0 0 0 0 0\n"
  "LeftButtonPressEvent 104 219 0 0 0 0 0\n"
  "StartInteractionEvent 104 219 0 0 0 0 0\n"
  "MouseMoveEvent 105 218 0 0 0 0 0\n"
  "RenderEvent 105 218 0 0 0 0 0\n"
  "InteractionEvent 105 218 0 0 0 0 0\n"
  "MouseMoveEvent 113 208 0 0 0 0 0\n"
  "RenderEvent 113 208 0 0 0 0 0\n"
  "InteractionEvent 113 208 0 0 0 0 0\n"
  "MouseMoveEvent 124 187 0 0 0 0 0\n"
  "RenderEvent 124 187 0 0 0 0 0\n"
  "InteractionEvent 124 187 0 0 0 0 0\n"
  "MouseMoveEvent 132 171 0 0 0 0 0\n"
  "RenderEvent 132 171 0 0 0 0 0\n"
  "InteractionEvent 132 171 0 0 0 0 0\n"
  "MouseMoveEvent 136 164 0 0 0 0 0\n"
  "RenderEvent 136 164 0 0 0 0 0\n"
  "InteractionEvent 136 164 0 0 0 0 0\n"
  "MouseMoveEvent 140 158 0 0 0 0 0\n"
  "RenderEvent 140 158 0 0 0 0 0\n"
  "InteractionEvent 140 158 0 0 0 0 0\n"
  "MouseMoveEvent 146 149 0 0 0 0 0\n"
  "RenderEvent 146 149 0 0 0 0 0\n"
  "InteractionEvent 146 149 0 0 0 0 0\n"
  "MouseMoveEvent 150 139 0 0 0 0 0\n"
  "RenderEvent 150 139 0 0 0 0 0\n"
  "InteractionEvent 150 139 0 0 0 0 0\n"
  "MouseMoveEvent 153 134 0 0 0 0 0\n"
  "RenderEvent 153 134 0 0 0 0 0\n"
  "InteractionEvent 153 134 0 0 0 0 0\n"
  "MouseMoveEvent 157 127 0 0 0 0 0\n"
  "RenderEvent 157 127 0 0 0 0 0\n"
  "InteractionEvent 157 127 0 0 0 0 0\n"
  "MouseMoveEvent 161 121 0 0 0 0 0\n"
  "RenderEvent 161 121 0 0 0 0 0\n"
  "InteractionEvent 161 121 0 0 0 0 0\n"
  "MouseMoveEvent 164 116 0 0 0 0 0\n"
  "RenderEvent 164 116 0 0 0 0 0\n"
  "InteractionEvent 164 116 0 0 0 0 0\n"
  "MouseMoveEvent 167 111 0 0 0 0 0\n"
  "RenderEvent 167 111 0 0 0 0 0\n"
  "InteractionEvent 167 111 0 0 0 0 0\n"
  "MouseMoveEvent 172 107 0 0 0 0 0\n"
  "RenderEvent 172 107 0 0 0 0 0\n"
  "InteractionEvent 172 107 0 0 0 0 0\n"
  "MouseMoveEvent 177 105 0 0 0 0 0\n"
  "RenderEvent 177 105 0 0 0 0 0\n"
  "InteractionEvent 177 105 0 0 0 0 0\n"
  "MouseMoveEvent 185 101 0 0 0 0 0\n"
  "RenderEvent 185 101 0 0 0 0 0\n"
  "InteractionEvent 185 101 0 0 0 0 0\n"
  "MouseMoveEvent 192 99 0 0 0 0 0\n"
  "RenderEvent 192 99 0 0 0 0 0\n"
  "InteractionEvent 192 99 0 0 0 0 0\n"
  "MouseMoveEvent 195 99 0 0 0 0 0\n"
  "RenderEvent 195 99 0 0 0 0 0\n"
  "InteractionEvent 195 99 0 0 0 0 0\n"
  "MouseMoveEvent 196 99 0 0 0 0 0\n"
  "RenderEvent 196 99 0 0 0 0 0\n"
  "InteractionEvent 196 99 0 0 0 0 0\n"
  "MouseMoveEvent 201 107 0 0 0 0 0\n"
  "RenderEvent 201 107 0 0 0 0 0\n"
  "InteractionEvent 201 107 0 0 0 0 0\n"
  "MouseMoveEvent 207 115 0 0 0 0 0\n"
  "RenderEvent 207 115 0 0 0 0 0\n"
  "InteractionEvent 207 115 0 0 0 0 0\n"
  "MouseMoveEvent 213 127 0 0 0 0 0\n"
  "RenderEvent 213 127 0 0 0 0 0\n"
  "InteractionEvent 213 127 0 0 0 0 0\n"
  "MouseMoveEvent 220 143 0 0 0 0 0\n"
  "RenderEvent 220 143 0 0 0 0 0\n"
  "InteractionEvent 220 143 0 0 0 0 0\n"
  "MouseMoveEvent 223 154 0 0 0 0 0\n"
  "RenderEvent 223 154 0 0 0 0 0\n"
  "InteractionEvent 223 154 0 0 0 0 0\n"
  "MouseMoveEvent 225 165 0 0 0 0 0\n"
  "RenderEvent 225 165 0 0 0 0 0\n"
  "InteractionEvent 225 165 0 0 0 0 0\n"
  "MouseMoveEvent 226 180 0 0 0 0 0\n"
  "RenderEvent 226 180 0 0 0 0 0\n"
  "InteractionEvent 226 180 0 0 0 0 0\n"
  "MouseMoveEvent 226 187 0 0 0 0 0\n"
  "RenderEvent 226 187 0 0 0 0 0\n"
  "InteractionEvent 226 187 0 0 0 0 0\n"
  "MouseMoveEvent 226 201 0 0 0 0 0\n"
  "RenderEvent 226 201 0 0 0 0 0\n"
  "InteractionEvent 226 201 0 0 0 0 0\n"
  "MouseMoveEvent 225 205 0 0 0 0 0\n"
  "RenderEvent 225 205 0 0 0 0 0\n"
  "InteractionEvent 225 205 0 0 0 0 0\n"
  "MouseMoveEvent 224 206 0 0 0 0 0\n"
  "RenderEvent 224 206 0 0 0 0 0\n"
  "InteractionEvent 224 206 0 0 0 0 0\n"
  "MouseMoveEvent 223 206 0 0 0 0 0\n"
  "RenderEvent 223 206 0 0 0 0 0\n"
  "InteractionEvent 223 206 0 0 0 0 0\n"
  "MouseMoveEvent 219 204 0 0 0 0 0\n"
  "RenderEvent 219 204 0 0 0 0 0\n"
  "InteractionEvent 219 204 0 0 0 0 0\n"
  "MouseMoveEvent 212 202 0 0 0 0 0\n"
  "RenderEvent 212 202 0 0 0 0 0\n"
  "InteractionEvent 212 202 0 0 0 0 0\n"
  "MouseMoveEvent 204 201 0 0 0 0 0\n"
  "RenderEvent 204 201 0 0 0 0 0\n"
  "InteractionEvent 204 201 0 0 0 0 0\n"
  "MouseMoveEvent 185 201 0 0 0 0 0\n"
  "RenderEvent 185 201 0 0 0 0 0\n"
  "InteractionEvent 185 201 0 0 0 0 0\n"
  "MouseMoveEvent 179 201 0 0 0 0 0\n"
  "RenderEvent 179 201 0 0 0 0 0\n"
  "InteractionEvent 179 201 0 0 0 0 0\n"
  "MouseMoveEvent 171 201 0 0 0 0 0\n"
  "RenderEvent 171 201 0 0 0 0 0\n"
  "InteractionEvent 171 201 0 0 0 0 0\n"
  "MouseMoveEvent 165 203 0 0 0 0 0\n"
  "RenderEvent 165 203 0 0 0 0 0\n"
  "InteractionEvent 165 203 0 0 0 0 0\n"
  "MouseMoveEvent 157 207 0 0 0 0 0\n"
  "RenderEvent 157 207 0 0 0 0 0\n"
  "InteractionEvent 157 207 0 0 0 0 0\n"
  "MouseMoveEvent 149 212 0 0 0 0 0\n"
  "RenderEvent 149 212 0 0 0 0 0\n"
  "InteractionEvent 149 212 0 0 0 0 0\n"
  "MouseMoveEvent 141 219 0 0 0 0 0\n"
  "RenderEvent 141 219 0 0 0 0 0\n"
  "InteractionEvent 141 219 0 0 0 0 0\n"
  "MouseMoveEvent 139 221 0 0 0 0 0\n"
  "RenderEvent 139 221 0 0 0 0 0\n"
  "InteractionEvent 139 221 0 0 0 0 0\n"
  "MouseMoveEvent 138 220 0 0 0 0 0\n"
  "RenderEvent 138 220 0 0 0 0 0\n"
  "InteractionEvent 138 220 0 0 0 0 0\n"
  "MouseMoveEvent 137 211 0 0 0 0 0\n"
  "RenderEvent 137 211 0 0 0 0 0\n"
  "InteractionEvent 137 211 0 0 0 0 0\n"
  "MouseMoveEvent 137 196 0 0 0 0 0\n"
  "RenderEvent 137 196 0 0 0 0 0\n"
  "InteractionEvent 137 196 0 0 0 0 0\n"
  "MouseMoveEvent 137 174 0 0 0 0 0\n"
  "RenderEvent 137 174 0 0 0 0 0\n"
  "InteractionEvent 137 174 0 0 0 0 0\n"
  "MouseMoveEvent 137 161 0 0 0 0 0\n"
  "RenderEvent 137 161 0 0 0 0 0\n"
  "InteractionEvent 137 161 0 0 0 0 0\n"
  "MouseMoveEvent 138 151 0 0 0 0 0\n"
  "RenderEvent 138 151 0 0 0 0 0\n"
  "InteractionEvent 138 151 0 0 0 0 0\n"
  "MouseMoveEvent 142 138 0 0 0 0 0\n"
  "RenderEvent 142 138 0 0 0 0 0\n"
  "InteractionEvent 142 138 0 0 0 0 0\n"
  "MouseMoveEvent 146 128 0 0 0 0 0\n"
  "RenderEvent 146 128 0 0 0 0 0\n"
  "InteractionEvent 146 128 0 0 0 0 0\n"
  "MouseMoveEvent 152 116 0 0 0 0 0\n"
  "RenderEvent 152 116 0 0 0 0 0\n"
  "InteractionEvent 152 116 0 0 0 0 0\n"
  "MouseMoveEvent 162 95 0 0 0 0 0\n"
  "RenderEvent 162 95 0 0 0 0 0\n"
  "InteractionEvent 162 95 0 0 0 0 0\n"
  "MouseMoveEvent 167 81 0 0 0 0 0\n"
  "RenderEvent 167 81 0 0 0 0 0\n"
  "InteractionEvent 167 81 0 0 0 0 0\n"
  "MouseMoveEvent 170 71 0 0 0 0 0\n"
  "RenderEvent 170 71 0 0 0 0 0\n"
  "InteractionEvent 170 71 0 0 0 0 0\n"
  "MouseMoveEvent 171 63 0 0 0 0 0\n"
  "RenderEvent 171 63 0 0 0 0 0\n"
  "InteractionEvent 171 63 0 0 0 0 0\n"
  "MouseMoveEvent 171 57 0 0 0 0 0\n"
  "RenderEvent 171 57 0 0 0 0 0\n"
  "InteractionEvent 171 57 0 0 0 0 0\n"
  "MouseMoveEvent 171 50 0 0 0 0 0\n"
  "RenderEvent 171 50 0 0 0 0 0\n"
  "InteractionEvent 171 50 0 0 0 0 0\n"
  "MouseMoveEvent 171 43 0 0 0 0 0\n"
  "RenderEvent 171 43 0 0 0 0 0\n"
  "InteractionEvent 171 43 0 0 0 0 0\n"
  "MouseMoveEvent 169 32 0 0 0 0 0\n"
  "RenderEvent 169 32 0 0 0 0 0\n"
  "InteractionEvent 169 32 0 0 0 0 0\n"
  "MouseMoveEvent 168 15 0 0 0 0 0\n"
  "RenderEvent 168 15 0 0 0 0 0\n"
  "InteractionEvent 168 15 0 0 0 0 0\n"
  "MouseMoveEvent 168 7 0 0 0 0 0\n"
  "RenderEvent 168 7 0 0 0 0 0\n"
  "InteractionEvent 168 7 0 0 0 0 0\n"
  "MouseMoveEvent 168 6 0 0 0 0 0\n"
  "RenderEvent 168 6 0 0 0 0 0\n"
  "InteractionEvent 168 6 0 0 0 0 0\n"
  "LeftButtonReleaseEvent 168 6 0 0 0 0 0\n"
  "EndInteractionEvent 168 6 0 0 0 0 0\n"
  "RenderEvent 168 6 0 0 0 0 0\n"
  "MouseMoveEvent 167 7 0 0 0 0 0\n"
  "MouseMoveEvent 167 8 0 0 0 0 0\n"
  "MouseMoveEvent 166 11 0 0 0 0 0\n"
  "MouseMoveEvent 164 14 0 0 0 0 0\n"
  "MouseMoveEvent 162 19 0 0 0 0 0\n"
  "MouseMoveEvent 158 28 0 0 0 0 0\n"
  "MouseMoveEvent 153 42 0 0 0 0 0\n"
  "MouseMoveEvent 146 57 0 0 0 0 0\n"
  "MouseMoveEvent 141 76 0 0 0 0 0\n"
  "MouseMoveEvent 136 95 0 0 0 0 0\n"
  "MouseMoveEvent 131 114 0 0 0 0 0\n"
  "MouseMoveEvent 129 131 0 0 0 0 0\n"
  "MouseMoveEvent 129 142 0 0 0 0 0\n"
  "MouseMoveEvent 127 153 0 0 0 0 0\n"
  "MouseMoveEvent 127 160 0 0 0 0 0\n"
  "MouseMoveEvent 126 166 0 0 0 0 0\n"
  "MouseMoveEvent 126 170 0 0 0 0 0\n"
  "MouseMoveEvent 126 173 0 0 0 0 0\n"
  "MouseMoveEvent 126 174 0 0 0 0 0\n"
  "MouseMoveEvent 126 175 0 0 0 0 0\n"
  "MouseMoveEvent 125 176 0 0 0 0 0\n"
  "MouseMoveEvent 125 177 0 0 0 0 0\n"
  "MouseMoveEvent 125 178 0 0 0 0 0\n"
  "MouseMoveEvent 125 176 0 0 0 0 0\n"
  "MouseMoveEvent 125 175 0 0 0 0 0\n"
  "MouseMoveEvent 125 173 0 0 0 0 0\n"
  "MouseMoveEvent 125 172 0 0 0 0 0\n"
  "LeftButtonPressEvent 125 172 0 0 0 0 0\n"
  "StartInteractionEvent 125 172 0 0 0 0 0\n"
  "MouseMoveEvent 125 170 0 0 0 0 0\n"
  "RenderEvent 125 170 0 0 0 0 0\n"
  "InteractionEvent 125 170 0 0 0 0 0\n"
  "MouseMoveEvent 125 160 0 0 0 0 0\n"
  "RenderEvent 125 160 0 0 0 0 0\n"
  "InteractionEvent 125 160 0 0 0 0 0\n"
  "MouseMoveEvent 125 154 0 0 0 0 0\n"
  "RenderEvent 125 154 0 0 0 0 0\n"
  "InteractionEvent 125 154 0 0 0 0 0\n"
  "MouseMoveEvent 124 149 0 0 0 0 0\n"
  "RenderEvent 124 149 0 0 0 0 0\n"
  "InteractionEvent 124 149 0 0 0 0 0\n"
  "MouseMoveEvent 122 140 0 0 0 0 0\n"
  "RenderEvent 122 140 0 0 0 0 0\n"
  "InteractionEvent 122 140 0 0 0 0 0\n"
  "MouseMoveEvent 120 131 0 0 0 0 0\n"
  "RenderEvent 120 131 0 0 0 0 0\n"
  "InteractionEvent 120 131 0 0 0 0 0\n"
  "MouseMoveEvent 119 122 0 0 0 0 0\n"
  "RenderEvent 119 122 0 0 0 0 0\n"
  "InteractionEvent 119 122 0 0 0 0 0\n"
  "MouseMoveEvent 117 117 0 0 0 0 0\n"
  "RenderEvent 117 117 0 0 0 0 0\n"
  "InteractionEvent 117 117 0 0 0 0 0\n"
  "MouseMoveEvent 116 111 0 0 0 0 0\n"
  "RenderEvent 116 111 0 0 0 0 0\n"
  "InteractionEvent 116 111 0 0 0 0 0\n"
  "MouseMoveEvent 114 105 0 0 0 0 0\n"
  "RenderEvent 114 105 0 0 0 0 0\n"
  "InteractionEvent 114 105 0 0 0 0 0\n"
  "MouseMoveEvent 112 91 0 0 0 0 0\n"
  "RenderEvent 112 91 0 0 0 0 0\n"
  "InteractionEvent 112 91 0 0 0 0 0\n"
  "MouseMoveEvent 111 87 0 0 0 0 0\n"
  "RenderEvent 111 87 0 0 0 0 0\n"
  "InteractionEvent 111 87 0 0 0 0 0\n"
  "MouseMoveEvent 111 84 0 0 0 0 0\n"
  "RenderEvent 111 84 0 0 0 0 0\n"
  "InteractionEvent 111 84 0 0 0 0 0\n"
  "MouseMoveEvent 111 81 0 0 0 0 0\n"
  "RenderEvent 111 81 0 0 0 0 0\n"
  "InteractionEvent 111 81 0 0 0 0 0\n"
  "MouseMoveEvent 112 78 0 0 0 0 0\n"
  "RenderEvent 112 78 0 0 0 0 0\n"
  "InteractionEvent 112 78 0 0 0 0 0\n"
  "MouseMoveEvent 115 74 0 0 0 0 0\n"
  "RenderEvent 115 74 0 0 0 0 0\n"
  "InteractionEvent 115 74 0 0 0 0 0\n"
  "MouseMoveEvent 116 73 0 0 0 0 0\n"
  "RenderEvent 116 73 0 0 0 0 0\n"
  "InteractionEvent 116 73 0 0 0 0 0\n"
  "MouseMoveEvent 118 72 0 0 0 0 0\n"
  "RenderEvent 118 72 0 0 0 0 0\n"
  "InteractionEvent 118 72 0 0 0 0 0\n"
  "MouseMoveEvent 121 72 0 0 0 0 0\n"
  "RenderEvent 121 72 0 0 0 0 0\n"
  "InteractionEvent 121 72 0 0 0 0 0\n"
  "MouseMoveEvent 123 72 0 0 0 0 0\n"
  "RenderEvent 123 72 0 0 0 0 0\n"
  "InteractionEvent 123 72 0 0 0 0 0\n"
  "MouseMoveEvent 125 73 0 0 0 0 0\n"
  "RenderEvent 125 73 0 0 0 0 0\n"
  "InteractionEvent 125 73 0 0 0 0 0\n"
  "MouseMoveEvent 127 74 0 0 0 0 0\n"
  "RenderEvent 127 74 0 0 0 0 0\n"
  "InteractionEvent 127 74 0 0 0 0 0\n"
  "MouseMoveEvent 128 74 0 0 0 0 0\n"
  "RenderEvent 128 74 0 0 0 0 0\n"
  "InteractionEvent 128 74 0 0 0 0 0\n"
  "LeftButtonReleaseEvent 128 74 0 0 0 0 0\n"
  "EndInteractionEvent 128 74 0 0 0 0 0\n"
  "RenderEvent 128 74 0 0 0 0 0\n"
  "MouseMoveEvent 131 75 0 0 0 0 0\n"
  "MouseMoveEvent 134 76 0 0 0 0 0\n"
  "MouseMoveEvent 139 77 0 0 0 0 0\n"
  "MouseMoveEvent 144 77 0 0 0 0 0\n"
  "MouseMoveEvent 151 80 0 0 0 0 0\n"
  "MouseMoveEvent 159 81 0 0 0 0 0\n"
  "MouseMoveEvent 166 84 0 0 0 0 0\n"
  "MouseMoveEvent 174 85 0 0 0 0 0\n"
  "MouseMoveEvent 181 87 0 0 0 0 0\n"
  "MouseMoveEvent 189 90 0 0 0 0 0\n"
  "MouseMoveEvent 193 92 0 0 0 0 0\n"
  "MouseMoveEvent 197 94 0 0 0 0 0\n"
  "MouseMoveEvent 201 96 0 0 0 0 0\n"
  "MouseMoveEvent 203 98 0 0 0 0 0\n"
  "MouseMoveEvent 205 100 0 0 0 0 0\n"
  "MouseMoveEvent 206 102 0 0 0 0 0\n"
  "MouseMoveEvent 207 104 0 0 0 0 0\n"
  "MouseMoveEvent 207 106 0 0 0 0 0\n"
  "MouseMoveEvent 208 108 0 0 0 0 0\n"
  "MouseMoveEvent 208 109 0 0 0 0 0\n"
  "MouseMoveEvent 209 111 0 0 0 0 0\n"
  "MouseMoveEvent 209 112 0 0 0 0 0\n"
  "MouseMoveEvent 209 113 0 0 0 0 0\n"
  "MouseMoveEvent 209 114 0 0 0 0 0\n"
  "MouseMoveEvent 210 116 0 0 0 0 0\n"
  "MouseMoveEvent 211 117 0 0 0 0 0\n"
  "MouseMoveEvent 212 117 0 0 0 0 0\n"
  "MouseMoveEvent 213 118 0 0 0 0 0\n"
  "MouseMoveEvent 214 118 0 0 0 0 0\n"
  "MouseMoveEvent 215 118 0 0 0 0 0\n"
  "MouseMoveEvent 216 118 0 0 0 0 0\n"
  "MouseMoveEvent 218 119 0 0 0 0 0\n"
  "MouseMoveEvent 219 119 0 0 0 0 0\n"
  "MouseMoveEvent 220 119 0 0 0 0 0\n"
  "MouseMoveEvent 221 120 0 0 0 0 0\n"
  "MouseMoveEvent 222 120 0 0 0 0 0\n"
  "MouseMoveEvent 224 121 0 0 0 0 0\n"
  "MouseMoveEvent 225 121 0 0 0 0 0\n"
  "MouseMoveEvent 226 121 0 0 0 0 0\n"
  "MouseMoveEvent 227 122 0 0 0 0 0\n"
  "MouseMoveEvent 228 122 0 0 0 0 0\n"
  "MouseMoveEvent 229 122 0 0 0 0 0\n"
  "MouseMoveEvent 230 122 0 0 0 0 0\n"
  "MouseMoveEvent 232 123 0 0 0 0 0\n"
  "MouseMoveEvent 233 123 0 0 0 0 0\n"
  "MouseMoveEvent 234 124 0 0 0 0 0\n"
  "MouseMoveEvent 235 123 0 0 0 0 0\n"
  "MouseMoveEvent 235 121 0 0 0 0 0\n"
  "MouseMoveEvent 234 120 0 0 0 0 0\n"
  "MouseMoveEvent 233 118 0 0 0 0 0\n"
  "MouseMoveEvent 233 117 0 0 0 0 0\n"
  "MouseMoveEvent 232 116 0 0 0 0 0\n"
  "MouseMoveEvent 232 114 0 0 0 0 0\n"
  "MouseMoveEvent 231 113 0 0 0 0 0\n"
  "MouseMoveEvent 231 111 0 0 0 0 0\n"
  "MouseMoveEvent 231 109 0 0 0 0 0\n"
  "MouseMoveEvent 231 107 0 0 0 0 0\n"
  "MouseMoveEvent 231 105 0 0 0 0 0\n"
  "MouseMoveEvent 231 103 0 0 0 0 0\n"
  "MouseMoveEvent 231 101 0 0 0 0 0\n"
  "MouseMoveEvent 231 99 0 0 0 0 0\n"
  "MouseMoveEvent 231 97 0 0 0 0 0\n"
  "MouseMoveEvent 231 96 0 0 0 0 0\n"
  "MouseMoveEvent 231 95 0 0 0 0 0\n"
  "MouseMoveEvent 231 94 0 0 0 0 0\n"
  "MouseMoveEvent 232 93 0 0 0 0 0\n"
  "MouseMoveEvent 233 92 0 0 0 0 0\n"
  "MouseMoveEvent 234 90 0 0 0 0 0\n"
  "MouseMoveEvent 235 89 0 0 0 0 0\n"
  "MouseMoveEvent 236 88 0 0 0 0 0\n"
  "MouseMoveEvent 237 87 0 0 0 0 0\n"
  "MouseMoveEvent 238 86 0 0 0 0 0\n"
  "MouseMoveEvent 239 85 0 0 0 0 0\n"
  "MouseMoveEvent 240 84 0 0 0 0 0\n"
  "MouseMoveEvent 241 83 0 0 0 0 0\n"
  "MouseMoveEvent 242 82 0 0 0 0 0\n"
  "MouseMoveEvent 243 82 0 0 0 0 0\n"
  "MouseMoveEvent 244 81 0 0 0 0 0\n"
  "MouseMoveEvent 245 81 0 0 0 0 0\n"
  "MouseMoveEvent 246 80 0 0 0 0 0\n"
  "MouseMoveEvent 247 79 0 0 0 0 0\n"
  "MouseMoveEvent 248 78 0 0 0 0 0\n"
  "MouseMoveEvent 249 77 0 0 0 0 0\n"
  "MouseMoveEvent 249 76 0 0 0 0 0\n"
  "MouseMoveEvent 250 75 0 0 0 0 0\n"
  "MouseMoveEvent 250 74 0 0 0 0 0\n"
  "MouseMoveEvent 250 73 0 0 0 0 0\n"
  "MouseMoveEvent 250 72 0 0 0 0 0\n"
  "MouseMoveEvent 250 71 0 0 0 0 0\n"
  "MouseMoveEvent 250 69 0 0 0 0 0\n"
  "MouseMoveEvent 250 68 0 0 0 0 0\n"
  "MouseMoveEvent 250 67 0 0 0 0 0\n"
  "MouseMoveEvent 250 66 0 0 0 0 0\n"
  "MouseMoveEvent 250 65 0 0 0 0 0\n"
  "MouseMoveEvent 248 67 0 0 0 0 0\n"
  "MouseMoveEvent 247 68 0 0 0 0 0\n"
  "MouseMoveEvent 246 69 0 0 0 0 0\n"
  "MouseMoveEvent 245 70 0 0 0 0 0\n"
  "MouseMoveEvent 244 71 0 0 0 0 0\n"
  "MouseMoveEvent 243 72 0 0 0 0 0\n"
  "MouseMoveEvent 241 74 0 0 0 0 0\n"
  "MouseMoveEvent 240 75 0 0 0 0 0\n"
  "MouseMoveEvent 239 76 0 0 0 0 0\n"
  "MouseMoveEvent 236 78 0 0 0 0 0\n"
  "MouseMoveEvent 235 80 0 0 0 0 0\n"
  "MouseMoveEvent 234 81 0 0 0 0 0\n"
  "MouseMoveEvent 234 82 0 0 0 0 0\n"
  "MouseMoveEvent 234 83 0 0 0 0 0\n"
  "MouseMoveEvent 234 85 0 0 0 0 0\n"
  "MouseMoveEvent 234 87 0 0 0 0 0\n"
  "MouseMoveEvent 233 88 0 0 0 0 0\n"
  "MouseMoveEvent 233 90 0 0 0 0 0\n"
  "MouseMoveEvent 232 93 0 0 0 0 0\n"
  "MouseMoveEvent 232 94 0 0 0 0 0\n"
  "MouseMoveEvent 232 97 0 0 0 0 0\n"
  "MouseMoveEvent 232 99 0 0 0 0 0\n"
  "MouseMoveEvent 231 102 0 0 0 0 0\n"
  "MouseMoveEvent 231 103 0 0 0 0 0\n"
  "MouseMoveEvent 231 105 0 0 0 0 0\n"
  "MouseMoveEvent 231 106 0 0 0 0 0\n"
  "MouseMoveEvent 231 108 0 0 0 0 0\n"
  "MouseMoveEvent 231 109 0 0 0 0 0\n"
  "MouseMoveEvent 231 110 0 0 0 0 0\n"
  "MouseMoveEvent 231 112 0 0 0 0 0\n"
  "MouseMoveEvent 231 113 0 0 0 0 0\n"
  "MouseMoveEvent 231 114 0 0 0 0 0\n"
  "MouseMoveEvent 231 115 0 0 0 0 0\n"
  "MouseMoveEvent 231 116 0 0 0 0 0\n"
  "MouseMoveEvent 231 117 0 0 0 0 0\n"
  "MouseMoveEvent 232 120 0 0 0 0 0\n"
  "MouseMoveEvent 232 123 0 0 0 0 0\n"
  "MouseMoveEvent 233 126 0 0 0 0 0\n"
  "MouseMoveEvent 233 129 0 0 0 0 0\n"
  "MouseMoveEvent 233 133 0 0 0 0 0\n"
  "MouseMoveEvent 234 136 0 0 0 0 0\n"
  "MouseMoveEvent 234 138 0 0 0 0 0\n"
  "MouseMoveEvent 234 139 0 0 0 0 0\n"
  "MouseMoveEvent 234 140 0 0 0 0 0\n"
  "MouseMoveEvent 234 141 0 0 0 0 0\n"
  "MouseWheelBackwardEvent 234 141 0 0 0 0 0\n"
  "StartInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "EndInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "MouseWheelForwardEvent 234 141 0 0 0 0 0\n"
  "StartInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "EndInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "MouseWheelForwardEvent 234 141 0 0 0 1 0\n"
  "StartInteractionEvent 234 141 0 0 0 1 0\n"
  "RenderEvent 234 141 0 0 0 1 0\n"
  "EndInteractionEvent 234 141 0 0 0 1 0\n"
  "RenderEvent 234 141 0 0 0 1 0\n"
  "MouseWheelBackwardEvent 234 141 0 0 0 0 0\n"
  "StartInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "EndInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "MouseWheelForwardEvent 234 141 0 0 0 0 0\n"
  "StartInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "EndInteractionEvent 234 141 0 0 0 0 0\n"
  "RenderEvent 234 141 0 0 0 0 0\n"
  "MouseMoveEvent 234 144 0 0 0 0 0\n"
  "MouseMoveEvent 237 148 0 0 0 0 0\n"
  "MouseMoveEvent 241 156 0 0 0 0 0\n"
  "MouseMoveEvent 246 166 0 0 0 0 0\n"
  "MouseMoveEvent 248 177 0 0 0 0 0\n"
  "MouseMoveEvent 251 192 0 0 0 0 0\n"
  "MouseMoveEvent 254 204 0 0 0 0 0\n"
  "MouseMoveEvent 256 213 0 0 0 0 0\n"
  "MouseMoveEvent 259 222 0 0 0 0 0\n"
  "MouseMoveEvent 261 227 0 0 0 0 0\n"
  "MouseMoveEvent 261 228 0 0 0 0 0\n"
  "MouseMoveEvent 262 229 0 0 0 0 0\n"
  "MouseMoveEvent 262 230 0 0 0 0 0\n"
  "MouseMoveEvent 263 231 0 0 0 0 0\n"
  "MouseMoveEvent 263 232 0 0 0 0 0\n"
  "MouseMoveEvent 263 233 0 0 0 0 0\n"
  "KeyPressEvent 263 233 1 0 3 1 c\n"
  "CharEvent 263 233 1 0 3 1 c\n"
  "KeyReleaseEvent 263 233 1 0 3 1 c\n"
  "KeyReleaseEvent 263 233 1 0 0 1 Control_L\n"
  "MouseMoveEvent 264 234 0 0 0 0 Control_L\n"
  "MouseMoveEvent 270 234 0 0 0 0 Control_L\n"
  "MouseMoveEvent 291 232 0 0 0 0 Control_L\n"
  "LeaveEvent 312 229 0 0 0 0 Control_L\n";

int TestGPURayCastThreeComponentsIndependent(int argc, char* argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  int dims[3] = { 100, 100, 100 };

  // Create a vtkImageData with two components
  vtkNew<vtkImageData> image;
  image->SetDimensions(dims[0], dims[1], dims[2]);
  image->AllocateScalars(VTK_DOUBLE, 3);

  // Fill the first half rectangular parallelopiped along X with the
  // first component values and the second half with second component values
  double* ptr = static_cast<double*>(image->GetScalarPointer(0, 0, 0));

  double center1[3], center2[3], center3[3];
  center1[0] = dims[0] / 3;
  center2[0] = center1[0] * 2;
  center3[0] = dims[0] / 2;
  center1[1] = center2[1] = dims[1] / 2;
  center3[1] = dims[1] / 3;
  center1[2] = center2[2] = center3[2] = dims[2] / 2;

  double radius;
  radius = center1[0];

  vtkNew<vtkSphere> sphere1;
  sphere1->SetCenter(center1);
  sphere1->SetRadius(radius);
  vtkNew<vtkSphere> sphere2;
  sphere2->SetCenter(center2);
  sphere2->SetRadius(radius);
  vtkNew<vtkSphere> sphere3;
  sphere3->SetCenter(center3);
  sphere3->SetRadius(radius);

  for (int z = 0; z < dims[2]; ++z)
  {
    for (int y = 0; y < dims[1]; ++y)
    {
      for (int x = 0; x < dims[0]; ++x)
      {
        // Set first component
        if (sphere1->EvaluateFunction(x, y, z) > 0)
        {
          // point outside sphere 1
          *ptr++ = 0.0;
        }
        else
        {
          *ptr++ = 0.33;
        }
        // Set second component
        if (sphere2->EvaluateFunction(x, y, z) > 0)
        {
          // point outside sphere 2
          *ptr++ = 0.0;
        }
        else
        {
          *ptr++ = 0.33;
        }
        // Set third component
        if (sphere3->EvaluateFunction(x, y, z) > 0)
        {
          // point outside sphere 2
          *ptr++ = 0.0;
        }
        else
        {
          *ptr++ = 0.33;
        }
      }
    }
  }

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(301, 300); // Intentional NPOT size
  renWin->SetMultiSamples(0);

  vtkNew<vtkRenderer> ren;
  renWin->AddRenderer(ren);

  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style);
  iren->SetRenderWindow(renWin);

  renWin->Render();

  // Volume render the dataset
  vtkNew<vtkGPUVolumeRayCastMapper> mapper;
  mapper->AutoAdjustSampleDistancesOff();
  mapper->SetSampleDistance(0.9);
  mapper->SetInputData(image);

  // Color transfer function
  vtkNew<vtkColorTransferFunction> ctf1;
  ctf1->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
  ctf1->AddRGBPoint(1.0, 1.0, 0.0, 0.0);

  vtkNew<vtkColorTransferFunction> ctf2;
  ctf2->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
  ctf2->AddRGBPoint(1.0, 0.0, 1.0, 0.0);

  vtkNew<vtkColorTransferFunction> ctf3;
  ctf3->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
  ctf3->AddRGBPoint(1.0, 0.0, 0.0, 1.0);

  // Opacity functions
  vtkNew<vtkPiecewiseFunction> pf1;
  pf1->AddPoint(0.0, 0.0);
  pf1->AddPoint(1.0, 0.2);

  vtkNew<vtkPiecewiseFunction> pf2;
  pf2->AddPoint(0.0, 0.0);
  pf2->AddPoint(1.0, 0.2);

  vtkNew<vtkPiecewiseFunction> pf3;
  pf3->AddPoint(0.0, 0.0);
  pf3->AddPoint(1.0, 0.2);

  // Volume property with independent components ON
  vtkNew<vtkVolumeProperty> property;
  property->IndependentComponentsOn();

  // Set color and opacity functions
  property->SetColor(0, ctf1);
  property->SetColor(1, ctf2);
  property->SetColor(2, ctf3);
  property->SetScalarOpacity(0, pf1);
  property->SetScalarOpacity(1, pf2);
  property->SetScalarOpacity(2, pf3);

  vtkNew<vtkVolume> volume;
  volume->SetMapper(mapper);
  volume->SetProperty(property);
  ren->AddVolume(volume);

  ren->ResetCamera();

  iren->Initialize();
  renWin->Render();

  return vtkTesting::InteractorEventLoop(
    argc, argv, iren, TestGPURayCastThreeComponentsIndependentLog);
}
