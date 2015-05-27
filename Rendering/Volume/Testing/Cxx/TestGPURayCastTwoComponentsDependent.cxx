/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestGPURayCastTwoComponentsDependent.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

// Description
// This test creates a vtkImageData with two components.
// The dataset is volume rendered with IndependentComponents off, i.e.
// the first component is passed through the color transfer function and the
// second is passed through the opacity transfer function.

#include "vtkCamera.h"
#include "vtkColorTransferFunction.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkImageData.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkNew.h"
#include "vtkPiecewiseFunction.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkTesting.h"
#include "vtkTestUtilities.h"
#include "vtkVolume.h"
#include "vtkVolumeProperty.h"

static const char * TestGPURayCastTwoComponentsDependentLog =
"# StreamVersion 1\n"
"EnterEvent 298 109 0 0 0 0 0\n"
"MouseMoveEvent 298 109 0 0 0 0 0\n"
"MouseMoveEvent 287 111 0 0 0 0 0\n"
"MouseMoveEvent 278 114 0 0 0 0 0\n"
"MouseMoveEvent 271 117 0 0 0 0 0\n"
"MouseMoveEvent 264 120 0 0 0 0 0\n"
"MouseMoveEvent 259 123 0 0 0 0 0\n"
"MouseMoveEvent 255 126 0 0 0 0 0\n"
"MouseMoveEvent 250 128 0 0 0 0 0\n"
"MouseMoveEvent 244 133 0 0 0 0 0\n"
"MouseMoveEvent 240 137 0 0 0 0 0\n"
"MouseMoveEvent 238 139 0 0 0 0 0\n"
"MouseMoveEvent 233 141 0 0 0 0 0\n"
"MouseMoveEvent 229 143 0 0 0 0 0\n"
"MouseMoveEvent 225 146 0 0 0 0 0\n"
"MouseMoveEvent 221 148 0 0 0 0 0\n"
"MouseMoveEvent 217 151 0 0 0 0 0\n"
"MouseMoveEvent 213 153 0 0 0 0 0\n"
"MouseMoveEvent 212 154 0 0 0 0 0\n"
"MouseMoveEvent 211 155 0 0 0 0 0\n"
"MouseMoveEvent 210 156 0 0 0 0 0\n"
"MouseMoveEvent 209 158 0 0 0 0 0\n"
"MouseMoveEvent 208 159 0 0 0 0 0\n"
"MouseMoveEvent 207 160 0 0 0 0 0\n"
"MouseMoveEvent 204 162 0 0 0 0 0\n"
"MouseMoveEvent 200 164 0 0 0 0 0\n"
"MouseMoveEvent 195 166 0 0 0 0 0\n"
"MouseMoveEvent 188 169 0 0 0 0 0\n"
"MouseMoveEvent 176 176 0 0 0 0 0\n"
"MouseMoveEvent 164 183 0 0 0 0 0\n"
"MouseMoveEvent 153 189 0 0 0 0 0\n"
"MouseMoveEvent 144 193 0 0 0 0 0\n"
"MouseMoveEvent 138 197 0 0 0 0 0\n"
"MouseMoveEvent 133 201 0 0 0 0 0\n"
"MouseMoveEvent 127 206 0 0 0 0 0\n"
"MouseMoveEvent 121 210 0 0 0 0 0\n"
"MouseMoveEvent 119 211 0 0 0 0 0\n"
"MouseMoveEvent 118 212 0 0 0 0 0\n"
"LeftButtonPressEvent 118 212 0 0 0 0 0\n"
"StartInteractionEvent 118 212 0 0 0 0 0\n"
"MouseMoveEvent 117 211 0 0 0 0 0\n"
"RenderEvent 117 211 0 0 0 0 0\n"
"InteractionEvent 117 211 0 0 0 0 0\n"
"MouseMoveEvent 118 209 0 0 0 0 0\n"
"RenderEvent 118 209 0 0 0 0 0\n"
"InteractionEvent 118 209 0 0 0 0 0\n"
"MouseMoveEvent 121 207 0 0 0 0 0\n"
"RenderEvent 121 207 0 0 0 0 0\n"
"InteractionEvent 121 207 0 0 0 0 0\n"
"MouseMoveEvent 125 199 0 0 0 0 0\n"
"RenderEvent 125 199 0 0 0 0 0\n"
"InteractionEvent 125 199 0 0 0 0 0\n"
"MouseMoveEvent 128 196 0 0 0 0 0\n"
"RenderEvent 128 196 0 0 0 0 0\n"
"InteractionEvent 128 196 0 0 0 0 0\n"
"MouseMoveEvent 132 190 0 0 0 0 0\n"
"RenderEvent 132 190 0 0 0 0 0\n"
"InteractionEvent 132 190 0 0 0 0 0\n"
"MouseMoveEvent 136 185 0 0 0 0 0\n"
"RenderEvent 136 185 0 0 0 0 0\n"
"InteractionEvent 136 185 0 0 0 0 0\n"
"MouseMoveEvent 139 181 0 0 0 0 0\n"
"RenderEvent 139 181 0 0 0 0 0\n"
"InteractionEvent 139 181 0 0 0 0 0\n"
"MouseMoveEvent 142 177 0 0 0 0 0\n"
"RenderEvent 142 177 0 0 0 0 0\n"
"InteractionEvent 142 177 0 0 0 0 0\n"
"MouseMoveEvent 144 174 0 0 0 0 0\n"
"RenderEvent 144 174 0 0 0 0 0\n"
"InteractionEvent 144 174 0 0 0 0 0\n"
"MouseMoveEvent 149 169 0 0 0 0 0\n"
"RenderEvent 149 169 0 0 0 0 0\n"
"InteractionEvent 149 169 0 0 0 0 0\n"
"MouseMoveEvent 153 165 0 0 0 0 0\n"
"RenderEvent 153 165 0 0 0 0 0\n"
"InteractionEvent 153 165 0 0 0 0 0\n"
"MouseMoveEvent 157 161 0 0 0 0 0\n"
"RenderEvent 157 161 0 0 0 0 0\n"
"InteractionEvent 157 161 0 0 0 0 0\n"
"MouseMoveEvent 159 158 0 0 0 0 0\n"
"RenderEvent 159 158 0 0 0 0 0\n"
"InteractionEvent 159 158 0 0 0 0 0\n"
"MouseMoveEvent 165 151 0 0 0 0 0\n"
"RenderEvent 165 151 0 0 0 0 0\n"
"InteractionEvent 165 151 0 0 0 0 0\n"
"MouseMoveEvent 168 147 0 0 0 0 0\n"
"RenderEvent 168 147 0 0 0 0 0\n"
"InteractionEvent 168 147 0 0 0 0 0\n"
"MouseMoveEvent 172 142 0 0 0 0 0\n"
"RenderEvent 172 142 0 0 0 0 0\n"
"InteractionEvent 172 142 0 0 0 0 0\n"
"MouseMoveEvent 175 139 0 0 0 0 0\n"
"RenderEvent 175 139 0 0 0 0 0\n"
"InteractionEvent 175 139 0 0 0 0 0\n"
"MouseMoveEvent 179 135 0 0 0 0 0\n"
"RenderEvent 179 135 0 0 0 0 0\n"
"InteractionEvent 179 135 0 0 0 0 0\n"
"MouseMoveEvent 183 131 0 0 0 0 0\n"
"RenderEvent 183 131 0 0 0 0 0\n"
"InteractionEvent 183 131 0 0 0 0 0\n"
"MouseMoveEvent 186 128 0 0 0 0 0\n"
"RenderEvent 186 128 0 0 0 0 0\n"
"InteractionEvent 186 128 0 0 0 0 0\n"
"MouseMoveEvent 188 126 0 0 0 0 0\n"
"RenderEvent 188 126 0 0 0 0 0\n"
"InteractionEvent 188 126 0 0 0 0 0\n"
"MouseMoveEvent 189 125 0 0 0 0 0\n"
"RenderEvent 189 125 0 0 0 0 0\n"
"InteractionEvent 189 125 0 0 0 0 0\n"
"MouseMoveEvent 192 123 0 0 0 0 0\n"
"RenderEvent 192 123 0 0 0 0 0\n"
"InteractionEvent 192 123 0 0 0 0 0\n"
"MouseMoveEvent 194 121 0 0 0 0 0\n"
"RenderEvent 194 121 0 0 0 0 0\n"
"InteractionEvent 194 121 0 0 0 0 0\n"
"MouseMoveEvent 196 120 0 0 0 0 0\n"
"RenderEvent 196 120 0 0 0 0 0\n"
"InteractionEvent 196 120 0 0 0 0 0\n"
"MouseMoveEvent 199 118 0 0 0 0 0\n"
"RenderEvent 199 118 0 0 0 0 0\n"
"InteractionEvent 199 118 0 0 0 0 0\n"
"MouseMoveEvent 200 118 0 0 0 0 0\n"
"RenderEvent 200 118 0 0 0 0 0\n"
"InteractionEvent 200 118 0 0 0 0 0\n"
"MouseMoveEvent 201 120 0 0 0 0 0\n"
"RenderEvent 201 120 0 0 0 0 0\n"
"InteractionEvent 201 120 0 0 0 0 0\n"
"MouseMoveEvent 201 123 0 0 0 0 0\n"
"RenderEvent 201 123 0 0 0 0 0\n"
"InteractionEvent 201 123 0 0 0 0 0\n"
"MouseMoveEvent 201 127 0 0 0 0 0\n"
"RenderEvent 201 127 0 0 0 0 0\n"
"InteractionEvent 201 127 0 0 0 0 0\n"
"MouseMoveEvent 197 138 0 0 0 0 0\n"
"RenderEvent 197 138 0 0 0 0 0\n"
"InteractionEvent 197 138 0 0 0 0 0\n"
"MouseMoveEvent 193 151 0 0 0 0 0\n"
"RenderEvent 193 151 0 0 0 0 0\n"
"InteractionEvent 193 151 0 0 0 0 0\n"
"MouseMoveEvent 187 167 0 0 0 0 0\n"
"RenderEvent 187 167 0 0 0 0 0\n"
"InteractionEvent 187 167 0 0 0 0 0\n"
"MouseMoveEvent 183 177 0 0 0 0 0\n"
"RenderEvent 183 177 0 0 0 0 0\n"
"InteractionEvent 183 177 0 0 0 0 0\n"
"MouseMoveEvent 182 185 0 0 0 0 0\n"
"RenderEvent 182 185 0 0 0 0 0\n"
"InteractionEvent 182 185 0 0 0 0 0\n"
"MouseMoveEvent 182 193 0 0 0 0 0\n"
"RenderEvent 182 193 0 0 0 0 0\n"
"InteractionEvent 182 193 0 0 0 0 0\n"
"MouseMoveEvent 180 201 0 0 0 0 0\n"
"RenderEvent 180 201 0 0 0 0 0\n"
"InteractionEvent 180 201 0 0 0 0 0\n"
"MouseMoveEvent 177 208 0 0 0 0 0\n"
"RenderEvent 177 208 0 0 0 0 0\n"
"InteractionEvent 177 208 0 0 0 0 0\n"
"MouseMoveEvent 176 215 0 0 0 0 0\n"
"RenderEvent 176 215 0 0 0 0 0\n"
"InteractionEvent 176 215 0 0 0 0 0\n"
"MouseMoveEvent 174 219 0 0 0 0 0\n"
"RenderEvent 174 219 0 0 0 0 0\n"
"InteractionEvent 174 219 0 0 0 0 0\n"
"MouseMoveEvent 173 222 0 0 0 0 0\n"
"RenderEvent 173 222 0 0 0 0 0\n"
"InteractionEvent 173 222 0 0 0 0 0\n"
"MouseMoveEvent 172 227 0 0 0 0 0\n"
"RenderEvent 172 227 0 0 0 0 0\n"
"InteractionEvent 172 227 0 0 0 0 0\n"
"MouseMoveEvent 169 232 0 0 0 0 0\n"
"RenderEvent 169 232 0 0 0 0 0\n"
"InteractionEvent 169 232 0 0 0 0 0\n"
"MouseMoveEvent 167 236 0 0 0 0 0\n"
"RenderEvent 167 236 0 0 0 0 0\n"
"InteractionEvent 167 236 0 0 0 0 0\n"
"MouseMoveEvent 164 240 0 0 0 0 0\n"
"RenderEvent 164 240 0 0 0 0 0\n"
"InteractionEvent 164 240 0 0 0 0 0\n"
"MouseMoveEvent 160 244 0 0 0 0 0\n"
"RenderEvent 160 244 0 0 0 0 0\n"
"InteractionEvent 160 244 0 0 0 0 0\n"
"MouseMoveEvent 158 246 0 0 0 0 0\n"
"RenderEvent 158 246 0 0 0 0 0\n"
"InteractionEvent 158 246 0 0 0 0 0\n"
"MouseMoveEvent 157 247 0 0 0 0 0\n"
"RenderEvent 157 247 0 0 0 0 0\n"
"InteractionEvent 157 247 0 0 0 0 0\n"
"MouseMoveEvent 156 249 0 0 0 0 0\n"
"RenderEvent 156 249 0 0 0 0 0\n"
"InteractionEvent 156 249 0 0 0 0 0\n"
"MouseMoveEvent 154 252 0 0 0 0 0\n"
"RenderEvent 154 252 0 0 0 0 0\n"
"InteractionEvent 154 252 0 0 0 0 0\n"
"MouseMoveEvent 153 253 0 0 0 0 0\n"
"RenderEvent 153 253 0 0 0 0 0\n"
"InteractionEvent 153 253 0 0 0 0 0\n"
"MouseMoveEvent 152 254 0 0 0 0 0\n"
"RenderEvent 152 254 0 0 0 0 0\n"
"InteractionEvent 152 254 0 0 0 0 0\n"
"MouseMoveEvent 151 255 0 0 0 0 0\n"
"RenderEvent 151 255 0 0 0 0 0\n"
"InteractionEvent 151 255 0 0 0 0 0\n"
"MouseMoveEvent 150 256 0 0 0 0 0\n"
"RenderEvent 150 256 0 0 0 0 0\n"
"InteractionEvent 150 256 0 0 0 0 0\n"
"MouseMoveEvent 149 257 0 0 0 0 0\n"
"RenderEvent 149 257 0 0 0 0 0\n"
"InteractionEvent 149 257 0 0 0 0 0\n"
"MouseMoveEvent 147 259 0 0 0 0 0\n"
"RenderEvent 147 259 0 0 0 0 0\n"
"InteractionEvent 147 259 0 0 0 0 0\n"
"MouseMoveEvent 146 261 0 0 0 0 0\n"
"RenderEvent 146 261 0 0 0 0 0\n"
"InteractionEvent 146 261 0 0 0 0 0\n"
"MouseMoveEvent 146 262 0 0 0 0 0\n"
"RenderEvent 146 262 0 0 0 0 0\n"
"InteractionEvent 146 262 0 0 0 0 0\n"
"MouseMoveEvent 145 263 0 0 0 0 0\n"
"RenderEvent 145 263 0 0 0 0 0\n"
"InteractionEvent 145 263 0 0 0 0 0\n"
"MouseMoveEvent 144 264 0 0 0 0 0\n"
"RenderEvent 144 264 0 0 0 0 0\n"
"InteractionEvent 144 264 0 0 0 0 0\n"
"MouseMoveEvent 143 264 0 0 0 0 0\n"
"RenderEvent 143 264 0 0 0 0 0\n"
"InteractionEvent 143 264 0 0 0 0 0\n"
"MouseMoveEvent 142 264 0 0 0 0 0\n"
"RenderEvent 142 264 0 0 0 0 0\n"
"InteractionEvent 142 264 0 0 0 0 0\n"
"MouseMoveEvent 141 264 0 0 0 0 0\n"
"RenderEvent 141 264 0 0 0 0 0\n"
"InteractionEvent 141 264 0 0 0 0 0\n"
"MouseMoveEvent 140 264 0 0 0 0 0\n"
"RenderEvent 140 264 0 0 0 0 0\n"
"InteractionEvent 140 264 0 0 0 0 0\n"
"MouseMoveEvent 138 263 0 0 0 0 0\n"
"RenderEvent 138 263 0 0 0 0 0\n"
"InteractionEvent 138 263 0 0 0 0 0\n"
"MouseMoveEvent 137 261 0 0 0 0 0\n"
"RenderEvent 137 261 0 0 0 0 0\n"
"InteractionEvent 137 261 0 0 0 0 0\n"
"MouseMoveEvent 136 260 0 0 0 0 0\n"
"RenderEvent 136 260 0 0 0 0 0\n"
"InteractionEvent 136 260 0 0 0 0 0\n"
"MouseMoveEvent 132 255 0 0 0 0 0\n"
"RenderEvent 132 255 0 0 0 0 0\n"
"InteractionEvent 132 255 0 0 0 0 0\n"
"MouseMoveEvent 128 249 0 0 0 0 0\n"
"RenderEvent 128 249 0 0 0 0 0\n"
"InteractionEvent 128 249 0 0 0 0 0\n"
"MouseMoveEvent 124 242 0 0 0 0 0\n"
"RenderEvent 124 242 0 0 0 0 0\n"
"InteractionEvent 124 242 0 0 0 0 0\n"
"MouseMoveEvent 122 237 0 0 0 0 0\n"
"RenderEvent 122 237 0 0 0 0 0\n"
"InteractionEvent 122 237 0 0 0 0 0\n"
"MouseMoveEvent 120 231 0 0 0 0 0\n"
"RenderEvent 120 231 0 0 0 0 0\n"
"InteractionEvent 120 231 0 0 0 0 0\n"
"MouseMoveEvent 118 225 0 0 0 0 0\n"
"RenderEvent 118 225 0 0 0 0 0\n"
"InteractionEvent 118 225 0 0 0 0 0\n"
"MouseMoveEvent 115 215 0 0 0 0 0\n"
"RenderEvent 115 215 0 0 0 0 0\n"
"InteractionEvent 115 215 0 0 0 0 0\n"
"MouseMoveEvent 114 203 0 0 0 0 0\n"
"RenderEvent 114 203 0 0 0 0 0\n"
"InteractionEvent 114 203 0 0 0 0 0\n"
"MouseMoveEvent 112 193 0 0 0 0 0\n"
"RenderEvent 112 193 0 0 0 0 0\n"
"InteractionEvent 112 193 0 0 0 0 0\n"
"MouseMoveEvent 112 184 0 0 0 0 0\n"
"RenderEvent 112 184 0 0 0 0 0\n"
"InteractionEvent 112 184 0 0 0 0 0\n"
"MouseMoveEvent 112 178 0 0 0 0 0\n"
"RenderEvent 112 178 0 0 0 0 0\n"
"InteractionEvent 112 178 0 0 0 0 0\n"
"MouseMoveEvent 112 173 0 0 0 0 0\n"
"RenderEvent 112 173 0 0 0 0 0\n"
"InteractionEvent 112 173 0 0 0 0 0\n"
"MouseMoveEvent 115 166 0 0 0 0 0\n"
"RenderEvent 115 166 0 0 0 0 0\n"
"InteractionEvent 115 166 0 0 0 0 0\n"
"MouseMoveEvent 116 163 0 0 0 0 0\n"
"RenderEvent 116 163 0 0 0 0 0\n"
"InteractionEvent 116 163 0 0 0 0 0\n"
"MouseMoveEvent 117 160 0 0 0 0 0\n"
"RenderEvent 117 160 0 0 0 0 0\n"
"InteractionEvent 117 160 0 0 0 0 0\n"
"MouseMoveEvent 118 157 0 0 0 0 0\n"
"RenderEvent 118 157 0 0 0 0 0\n"
"InteractionEvent 118 157 0 0 0 0 0\n"
"MouseMoveEvent 120 152 0 0 0 0 0\n"
"RenderEvent 120 152 0 0 0 0 0\n"
"InteractionEvent 120 152 0 0 0 0 0\n"
"MouseMoveEvent 121 148 0 0 0 0 0\n"
"RenderEvent 121 148 0 0 0 0 0\n"
"InteractionEvent 121 148 0 0 0 0 0\n"
"MouseMoveEvent 122 142 0 0 0 0 0\n"
"RenderEvent 122 142 0 0 0 0 0\n"
"InteractionEvent 122 142 0 0 0 0 0\n"
"MouseMoveEvent 123 136 0 0 0 0 0\n"
"RenderEvent 123 136 0 0 0 0 0\n"
"InteractionEvent 123 136 0 0 0 0 0\n"
"MouseMoveEvent 124 131 0 0 0 0 0\n"
"RenderEvent 124 131 0 0 0 0 0\n"
"InteractionEvent 124 131 0 0 0 0 0\n"
"MouseMoveEvent 126 124 0 0 0 0 0\n"
"RenderEvent 126 124 0 0 0 0 0\n"
"InteractionEvent 126 124 0 0 0 0 0\n"
"MouseMoveEvent 127 120 0 0 0 0 0\n"
"RenderEvent 127 120 0 0 0 0 0\n"
"InteractionEvent 127 120 0 0 0 0 0\n"
"MouseMoveEvent 128 119 0 0 0 0 0\n"
"RenderEvent 128 119 0 0 0 0 0\n"
"InteractionEvent 128 119 0 0 0 0 0\n"
"MouseMoveEvent 128 118 0 0 0 0 0\n"
"RenderEvent 128 118 0 0 0 0 0\n"
"InteractionEvent 128 118 0 0 0 0 0\n"
"MouseMoveEvent 130 116 0 0 0 0 0\n"
"RenderEvent 130 116 0 0 0 0 0\n"
"InteractionEvent 130 116 0 0 0 0 0\n"
"MouseMoveEvent 131 115 0 0 0 0 0\n"
"RenderEvent 131 115 0 0 0 0 0\n"
"InteractionEvent 131 115 0 0 0 0 0\n"
"MouseMoveEvent 133 113 0 0 0 0 0\n"
"RenderEvent 133 113 0 0 0 0 0\n"
"InteractionEvent 133 113 0 0 0 0 0\n"
"MouseMoveEvent 135 110 0 0 0 0 0\n"
"RenderEvent 135 110 0 0 0 0 0\n"
"InteractionEvent 135 110 0 0 0 0 0\n"
"MouseMoveEvent 137 108 0 0 0 0 0\n"
"RenderEvent 137 108 0 0 0 0 0\n"
"InteractionEvent 137 108 0 0 0 0 0\n"
"MouseMoveEvent 142 105 0 0 0 0 0\n"
"RenderEvent 142 105 0 0 0 0 0\n"
"InteractionEvent 142 105 0 0 0 0 0\n"
"MouseMoveEvent 147 100 0 0 0 0 0\n"
"RenderEvent 147 100 0 0 0 0 0\n"
"InteractionEvent 147 100 0 0 0 0 0\n"
"MouseMoveEvent 155 94 0 0 0 0 0\n"
"RenderEvent 155 94 0 0 0 0 0\n"
"InteractionEvent 155 94 0 0 0 0 0\n"
"MouseMoveEvent 165 88 0 0 0 0 0\n"
"RenderEvent 165 88 0 0 0 0 0\n"
"InteractionEvent 165 88 0 0 0 0 0\n"
"MouseMoveEvent 178 80 0 0 0 0 0\n"
"RenderEvent 178 80 0 0 0 0 0\n"
"InteractionEvent 178 80 0 0 0 0 0\n"
"MouseMoveEvent 190 73 0 0 0 0 0\n"
"RenderEvent 190 73 0 0 0 0 0\n"
"InteractionEvent 190 73 0 0 0 0 0\n"
"MouseMoveEvent 202 65 0 0 0 0 0\n"
"RenderEvent 202 65 0 0 0 0 0\n"
"InteractionEvent 202 65 0 0 0 0 0\n"
"MouseMoveEvent 213 56 0 0 0 0 0\n"
"RenderEvent 213 56 0 0 0 0 0\n"
"InteractionEvent 213 56 0 0 0 0 0\n"
"MouseMoveEvent 223 53 0 0 0 0 0\n"
"RenderEvent 223 53 0 0 0 0 0\n"
"InteractionEvent 223 53 0 0 0 0 0\n"
"MouseMoveEvent 228 51 0 0 0 0 0\n"
"RenderEvent 228 51 0 0 0 0 0\n"
"InteractionEvent 228 51 0 0 0 0 0\n"
"MouseMoveEvent 233 49 0 0 0 0 0\n"
"RenderEvent 233 49 0 0 0 0 0\n"
"InteractionEvent 233 49 0 0 0 0 0\n"
"MouseMoveEvent 239 46 0 0 0 0 0\n"
"RenderEvent 239 46 0 0 0 0 0\n"
"InteractionEvent 239 46 0 0 0 0 0\n"
"MouseMoveEvent 241 45 0 0 0 0 0\n"
"RenderEvent 241 45 0 0 0 0 0\n"
"InteractionEvent 241 45 0 0 0 0 0\n"
"MouseMoveEvent 242 44 0 0 0 0 0\n"
"RenderEvent 242 44 0 0 0 0 0\n"
"InteractionEvent 242 44 0 0 0 0 0\n"
"MouseMoveEvent 244 43 0 0 0 0 0\n"
"RenderEvent 244 43 0 0 0 0 0\n"
"InteractionEvent 244 43 0 0 0 0 0\n"
"MouseMoveEvent 251 39 0 0 0 0 0\n"
"RenderEvent 251 39 0 0 0 0 0\n"
"InteractionEvent 251 39 0 0 0 0 0\n"
"MouseMoveEvent 254 37 0 0 0 0 0\n"
"RenderEvent 254 37 0 0 0 0 0\n"
"InteractionEvent 254 37 0 0 0 0 0\n"
"MouseMoveEvent 257 34 0 0 0 0 0\n"
"RenderEvent 257 34 0 0 0 0 0\n"
"InteractionEvent 257 34 0 0 0 0 0\n"
"MouseMoveEvent 259 32 0 0 0 0 0\n"
"RenderEvent 259 32 0 0 0 0 0\n"
"InteractionEvent 259 32 0 0 0 0 0\n"
"MouseMoveEvent 260 31 0 0 0 0 0\n"
"RenderEvent 260 31 0 0 0 0 0\n"
"InteractionEvent 260 31 0 0 0 0 0\n"
"MouseMoveEvent 260 30 0 0 0 0 0\n"
"RenderEvent 260 30 0 0 0 0 0\n"
"InteractionEvent 260 30 0 0 0 0 0\n"
"MouseMoveEvent 260 29 0 0 0 0 0\n"
"RenderEvent 260 29 0 0 0 0 0\n"
"InteractionEvent 260 29 0 0 0 0 0\n"
"MouseMoveEvent 260 28 0 0 0 0 0\n"
"RenderEvent 260 28 0 0 0 0 0\n"
"InteractionEvent 260 28 0 0 0 0 0\n"
"MouseMoveEvent 260 26 0 0 0 0 0\n"
"RenderEvent 260 26 0 0 0 0 0\n"
"InteractionEvent 260 26 0 0 0 0 0\n"
"MouseMoveEvent 261 22 0 0 0 0 0\n"
"RenderEvent 261 22 0 0 0 0 0\n"
"InteractionEvent 261 22 0 0 0 0 0\n"
"MouseMoveEvent 261 19 0 0 0 0 0\n"
"RenderEvent 261 19 0 0 0 0 0\n"
"InteractionEvent 261 19 0 0 0 0 0\n"
"MouseMoveEvent 262 16 0 0 0 0 0\n"
"RenderEvent 262 16 0 0 0 0 0\n"
"InteractionEvent 262 16 0 0 0 0 0\n"
"MouseMoveEvent 263 14 0 0 0 0 0\n"
"RenderEvent 263 14 0 0 0 0 0\n"
"InteractionEvent 263 14 0 0 0 0 0\n"
"MouseMoveEvent 264 11 0 0 0 0 0\n"
"RenderEvent 264 11 0 0 0 0 0\n"
"InteractionEvent 264 11 0 0 0 0 0\n"
"MouseMoveEvent 264 10 0 0 0 0 0\n"
"RenderEvent 264 10 0 0 0 0 0\n"
"InteractionEvent 264 10 0 0 0 0 0\n"
"MouseMoveEvent 265 9 0 0 0 0 0\n"
"RenderEvent 265 9 0 0 0 0 0\n"
"InteractionEvent 265 9 0 0 0 0 0\n"
"LeftButtonReleaseEvent 265 9 0 0 0 0 0\n"
"EndInteractionEvent 265 9 0 0 0 0 0\n"
"RenderEvent 265 9 0 0 0 0 0\n"
"MouseMoveEvent 265 10 0 0 0 0 0\n"
"MouseMoveEvent 265 11 0 0 0 0 0\n"
"MouseMoveEvent 265 14 0 0 0 0 0\n"
"MouseMoveEvent 262 17 0 0 0 0 0\n"
"MouseMoveEvent 257 25 0 0 0 0 0\n"
"MouseMoveEvent 248 36 0 0 0 0 0\n"
"MouseMoveEvent 239 47 0 0 0 0 0\n"
"MouseMoveEvent 228 58 0 0 0 0 0\n"
"MouseMoveEvent 217 69 0 0 0 0 0\n"
"MouseMoveEvent 204 80 0 0 0 0 0\n"
"MouseMoveEvent 191 91 0 0 0 0 0\n"
"MouseMoveEvent 182 100 0 0 0 0 0\n"
"MouseMoveEvent 173 109 0 0 0 0 0\n"
"MouseMoveEvent 166 116 0 0 0 0 0\n"
"MouseMoveEvent 161 123 0 0 0 0 0\n"
"MouseMoveEvent 156 129 0 0 0 0 0\n"
"MouseMoveEvent 154 136 0 0 0 0 0\n"
"MouseMoveEvent 150 143 0 0 0 0 0\n"
"MouseMoveEvent 145 152 0 0 0 0 0\n"
"MouseMoveEvent 140 158 0 0 0 0 0\n"
"MouseMoveEvent 138 165 0 0 0 0 0\n"
"MouseMoveEvent 136 169 0 0 0 0 0\n"
"MouseMoveEvent 135 171 0 0 0 0 0\n"
"MouseMoveEvent 135 172 0 0 0 0 0\n"
"MouseWheelForwardEvent 135 172 0 0 0 0 0\n"
"StartInteractionEvent 135 172 0 0 0 0 0\n"
"RenderEvent 135 172 0 0 0 0 0\n"
"EndInteractionEvent 135 172 0 0 0 0 0\n"
"RenderEvent 135 172 0 0 0 0 0\n"
"MouseWheelForwardEvent 135 172 0 0 0 1 0\n"
"StartInteractionEvent 135 172 0 0 0 1 0\n"
"RenderEvent 135 172 0 0 0 1 0\n"
"EndInteractionEvent 135 172 0 0 0 1 0\n"
"RenderEvent 135 172 0 0 0 1 0\n"
"LeftButtonPressEvent 135 172 0 0 0 0 0\n"
"StartInteractionEvent 135 172 0 0 0 0 0\n"
"MouseMoveEvent 136 171 0 0 0 0 0\n"
"RenderEvent 136 171 0 0 0 0 0\n"
"InteractionEvent 136 171 0 0 0 0 0\n"
"MouseMoveEvent 137 171 0 0 0 0 0\n"
"RenderEvent 137 171 0 0 0 0 0\n"
"InteractionEvent 137 171 0 0 0 0 0\n"
"MouseMoveEvent 138 170 0 0 0 0 0\n"
"RenderEvent 138 170 0 0 0 0 0\n"
"InteractionEvent 138 170 0 0 0 0 0\n"
"MouseMoveEvent 139 169 0 0 0 0 0\n"
"RenderEvent 139 169 0 0 0 0 0\n"
"InteractionEvent 139 169 0 0 0 0 0\n"
"MouseMoveEvent 140 168 0 0 0 0 0\n"
"RenderEvent 140 168 0 0 0 0 0\n"
"InteractionEvent 140 168 0 0 0 0 0\n"
"MouseMoveEvent 143 166 0 0 0 0 0\n"
"RenderEvent 143 166 0 0 0 0 0\n"
"InteractionEvent 143 166 0 0 0 0 0\n"
"MouseMoveEvent 147 163 0 0 0 0 0\n"
"RenderEvent 147 163 0 0 0 0 0\n"
"InteractionEvent 147 163 0 0 0 0 0\n"
"MouseMoveEvent 149 161 0 0 0 0 0\n"
"RenderEvent 149 161 0 0 0 0 0\n"
"InteractionEvent 149 161 0 0 0 0 0\n"
"MouseMoveEvent 151 159 0 0 0 0 0\n"
"RenderEvent 151 159 0 0 0 0 0\n"
"InteractionEvent 151 159 0 0 0 0 0\n"
"MouseMoveEvent 152 158 0 0 0 0 0\n"
"RenderEvent 152 158 0 0 0 0 0\n"
"InteractionEvent 152 158 0 0 0 0 0\n"
"MouseMoveEvent 153 157 0 0 0 0 0\n"
"RenderEvent 153 157 0 0 0 0 0\n"
"InteractionEvent 153 157 0 0 0 0 0\n"
"MouseMoveEvent 154 155 0 0 0 0 0\n"
"RenderEvent 154 155 0 0 0 0 0\n"
"InteractionEvent 154 155 0 0 0 0 0\n"
"MouseMoveEvent 156 153 0 0 0 0 0\n"
"RenderEvent 156 153 0 0 0 0 0\n"
"InteractionEvent 156 153 0 0 0 0 0\n"
"MouseMoveEvent 157 152 0 0 0 0 0\n"
"RenderEvent 157 152 0 0 0 0 0\n"
"InteractionEvent 157 152 0 0 0 0 0\n"
"MouseMoveEvent 159 151 0 0 0 0 0\n"
"RenderEvent 159 151 0 0 0 0 0\n"
"InteractionEvent 159 151 0 0 0 0 0\n"
"MouseMoveEvent 163 148 0 0 0 0 0\n"
"RenderEvent 163 148 0 0 0 0 0\n"
"InteractionEvent 163 148 0 0 0 0 0\n"
"MouseMoveEvent 165 146 0 0 0 0 0\n"
"RenderEvent 165 146 0 0 0 0 0\n"
"InteractionEvent 165 146 0 0 0 0 0\n"
"MouseMoveEvent 167 144 0 0 0 0 0\n"
"RenderEvent 167 144 0 0 0 0 0\n"
"InteractionEvent 167 144 0 0 0 0 0\n"
"MouseMoveEvent 168 143 0 0 0 0 0\n"
"RenderEvent 168 143 0 0 0 0 0\n"
"InteractionEvent 168 143 0 0 0 0 0\n"
"MouseMoveEvent 169 142 0 0 0 0 0\n"
"RenderEvent 169 142 0 0 0 0 0\n"
"InteractionEvent 169 142 0 0 0 0 0\n"
"MouseMoveEvent 170 141 0 0 0 0 0\n"
"RenderEvent 170 141 0 0 0 0 0\n"
"InteractionEvent 170 141 0 0 0 0 0\n"
"MouseMoveEvent 169 140 0 0 0 0 0\n"
"RenderEvent 169 140 0 0 0 0 0\n"
"InteractionEvent 169 140 0 0 0 0 0\n"
"MouseMoveEvent 168 140 0 0 0 0 0\n"
"RenderEvent 168 140 0 0 0 0 0\n"
"InteractionEvent 168 140 0 0 0 0 0\n"
"MouseMoveEvent 167 140 0 0 0 0 0\n"
"RenderEvent 167 140 0 0 0 0 0\n"
"InteractionEvent 167 140 0 0 0 0 0\n"
"MouseMoveEvent 165 140 0 0 0 0 0\n"
"RenderEvent 165 140 0 0 0 0 0\n"
"InteractionEvent 165 140 0 0 0 0 0\n"
"MouseMoveEvent 164 140 0 0 0 0 0\n"
"RenderEvent 164 140 0 0 0 0 0\n"
"InteractionEvent 164 140 0 0 0 0 0\n"
"MouseMoveEvent 163 140 0 0 0 0 0\n"
"RenderEvent 163 140 0 0 0 0 0\n"
"InteractionEvent 163 140 0 0 0 0 0\n"
"MouseMoveEvent 161 142 0 0 0 0 0\n"
"RenderEvent 161 142 0 0 0 0 0\n"
"InteractionEvent 161 142 0 0 0 0 0\n"
"MouseMoveEvent 159 144 0 0 0 0 0\n"
"RenderEvent 159 144 0 0 0 0 0\n"
"InteractionEvent 159 144 0 0 0 0 0\n"
"MouseMoveEvent 157 146 0 0 0 0 0\n"
"RenderEvent 157 146 0 0 0 0 0\n"
"InteractionEvent 157 146 0 0 0 0 0\n"
"MouseMoveEvent 156 147 0 0 0 0 0\n"
"RenderEvent 156 147 0 0 0 0 0\n"
"InteractionEvent 156 147 0 0 0 0 0\n"
"MouseMoveEvent 156 148 0 0 0 0 0\n"
"RenderEvent 156 148 0 0 0 0 0\n"
"InteractionEvent 156 148 0 0 0 0 0\n"
"MouseMoveEvent 157 149 0 0 0 0 0\n"
"RenderEvent 157 149 0 0 0 0 0\n"
"InteractionEvent 157 149 0 0 0 0 0\n"
"LeftButtonReleaseEvent 157 149 0 0 0 0 0\n"
"EndInteractionEvent 157 149 0 0 0 0 0\n"
"RenderEvent 157 149 0 0 0 0 0\n"
"LeftButtonPressEvent 157 149 0 0 0 0 0\n"
"StartInteractionEvent 157 149 0 0 0 0 0\n"
"MouseMoveEvent 156 148 0 0 0 0 0\n"
"RenderEvent 156 148 0 0 0 0 0\n"
"InteractionEvent 156 148 0 0 0 0 0\n"
"MouseMoveEvent 155 147 0 0 0 0 0\n"
"RenderEvent 155 147 0 0 0 0 0\n"
"InteractionEvent 155 147 0 0 0 0 0\n"
"MouseMoveEvent 154 146 0 0 0 0 0\n"
"RenderEvent 154 146 0 0 0 0 0\n"
"InteractionEvent 154 146 0 0 0 0 0\n"
"MouseMoveEvent 153 145 0 0 0 0 0\n"
"RenderEvent 153 145 0 0 0 0 0\n"
"InteractionEvent 153 145 0 0 0 0 0\n"
"MouseMoveEvent 151 142 0 0 0 0 0\n"
"RenderEvent 151 142 0 0 0 0 0\n"
"InteractionEvent 151 142 0 0 0 0 0\n"
"MouseMoveEvent 150 140 0 0 0 0 0\n"
"RenderEvent 150 140 0 0 0 0 0\n"
"InteractionEvent 150 140 0 0 0 0 0\n"
"MouseMoveEvent 149 139 0 0 0 0 0\n"
"RenderEvent 149 139 0 0 0 0 0\n"
"InteractionEvent 149 139 0 0 0 0 0\n"
"MouseMoveEvent 148 138 0 0 0 0 0\n"
"RenderEvent 148 138 0 0 0 0 0\n"
"InteractionEvent 148 138 0 0 0 0 0\n"
"MouseMoveEvent 147 137 0 0 0 0 0\n"
"RenderEvent 147 137 0 0 0 0 0\n"
"InteractionEvent 147 137 0 0 0 0 0\n"
"LeftButtonReleaseEvent 147 137 0 0 0 0 0\n"
"EndInteractionEvent 147 137 0 0 0 0 0\n"
"RenderEvent 147 137 0 0 0 0 0\n"
"MouseMoveEvent 149 140 0 0 0 0 0\n"
"MouseMoveEvent 150 141 0 0 0 0 0\n"
"MouseMoveEvent 152 143 0 0 0 0 0\n"
"MouseMoveEvent 153 144 0 0 0 0 0\n"
"MouseMoveEvent 154 145 0 0 0 0 0\n"
"MouseMoveEvent 156 147 0 0 0 0 0\n"
"MouseMoveEvent 157 148 0 0 0 0 0\n"
"MouseMoveEvent 159 149 0 0 0 0 0\n"
"MouseMoveEvent 160 150 0 0 0 0 0\n"
"MouseMoveEvent 162 151 0 0 0 0 0\n"
"MouseMoveEvent 163 152 0 0 0 0 0\n"
"MouseMoveEvent 164 153 0 0 0 0 0\n"
"MouseMoveEvent 165 154 0 0 0 0 0\n"
"MouseMoveEvent 167 156 0 0 0 0 0\n"
"MouseMoveEvent 168 157 0 0 0 0 0\n"
"MouseMoveEvent 169 158 0 0 0 0 0\n"
"MouseMoveEvent 170 159 0 0 0 0 0\n"
"MouseMoveEvent 171 161 0 0 0 0 0\n"
"MouseMoveEvent 172 162 0 0 0 0 0\n"
"MouseMoveEvent 173 163 0 0 0 0 0\n"
"MouseMoveEvent 174 164 0 0 0 0 0\n"
"MouseMoveEvent 175 166 0 0 0 0 0\n"
"MouseMoveEvent 176 167 0 0 0 0 0\n"
"MouseMoveEvent 175 165 0 0 0 0 0\n"
"MouseMoveEvent 174 164 0 0 0 0 0\n"
"MouseMoveEvent 173 161 0 0 0 0 0\n"
"MouseMoveEvent 172 159 0 0 0 0 0\n"
"MouseMoveEvent 170 157 0 0 0 0 0\n"
"MouseMoveEvent 169 157 0 0 0 0 0\n"
"MouseMoveEvent 168 156 0 0 0 0 0\n"
"MouseMoveEvent 166 155 0 0 0 0 0\n"
"MouseMoveEvent 164 154 0 0 0 0 0\n"
"MouseMoveEvent 163 154 0 0 0 0 0\n"
"MouseMoveEvent 161 154 0 0 0 0 0\n"
"MouseMoveEvent 158 154 0 0 0 0 0\n"
"MouseMoveEvent 155 154 0 0 0 0 0\n"
"MouseMoveEvent 152 154 0 0 0 0 0\n"
"MouseMoveEvent 150 155 0 0 0 0 0\n"
"MouseMoveEvent 149 155 0 0 0 0 0\n"
"MouseMoveEvent 148 156 0 0 0 0 0\n"
"MouseMoveEvent 146 156 0 0 0 0 0\n"
"MouseMoveEvent 145 157 0 0 0 0 0\n"
"MouseMoveEvent 144 159 0 0 0 0 0\n"
"MouseMoveEvent 143 160 0 0 0 0 0\n"
"MouseMoveEvent 142 162 0 0 0 0 0\n"
"MouseMoveEvent 142 163 0 0 0 0 0\n"
"MouseMoveEvent 141 164 0 0 0 0 0\n"
"MouseMoveEvent 142 165 0 0 0 0 0\n"
"MouseMoveEvent 144 165 0 0 0 0 0\n"
"MouseMoveEvent 145 165 0 0 0 0 0\n"
"MouseMoveEvent 147 166 0 0 0 0 0\n"
"MouseMoveEvent 150 166 0 0 0 0 0\n"
"MouseMoveEvent 153 166 0 0 0 0 0\n"
"MouseMoveEvent 158 166 0 0 0 0 0\n"
"MouseMoveEvent 165 166 0 0 0 0 0\n"
"MouseMoveEvent 168 166 0 0 0 0 0\n"
"MouseMoveEvent 171 166 0 0 0 0 0\n"
"MouseMoveEvent 174 166 0 0 0 0 0\n"
"MouseMoveEvent 176 166 0 0 0 0 0\n"
"MouseMoveEvent 177 166 0 0 0 0 0\n"
"MouseMoveEvent 178 166 0 0 0 0 0\n"
"MouseMoveEvent 180 166 0 0 0 0 0\n"
"MouseMoveEvent 182 166 0 0 0 0 0\n"
"MouseMoveEvent 183 166 0 0 0 0 0\n"
"MouseMoveEvent 185 166 0 0 0 0 0\n"
"MouseMoveEvent 186 166 0 0 0 0 0\n"
"MouseMoveEvent 187 166 0 0 0 0 0\n"
"MouseMoveEvent 190 167 0 0 0 0 0\n"
"MouseMoveEvent 191 168 0 0 0 0 0\n"
"MouseMoveEvent 192 169 0 0 0 0 0\n"
"MouseMoveEvent 193 170 0 0 0 0 0\n"
"MouseMoveEvent 193 171 0 0 0 0 0\n"
"MouseMoveEvent 193 173 0 0 0 0 0\n"
"MouseMoveEvent 194 175 0 0 0 0 0\n"
"MouseMoveEvent 195 177 0 0 0 0 0\n"
"MouseMoveEvent 196 180 0 0 0 0 0\n"
"MouseMoveEvent 198 184 0 0 0 0 0\n"
"MouseMoveEvent 200 189 0 0 0 0 0\n"
"MouseMoveEvent 203 196 0 0 0 0 0\n"
"MouseMoveEvent 207 203 0 0 0 0 0\n"
"MouseMoveEvent 211 210 0 0 0 0 0\n"
"MouseMoveEvent 215 217 0 0 0 0 0\n"
"MouseMoveEvent 219 223 0 0 0 0 0\n"
"MouseMoveEvent 220 228 0 0 0 0 0\n"
"MouseMoveEvent 223 234 0 0 0 0 0\n"
"MouseMoveEvent 225 239 0 0 0 0 0\n"
"MouseMoveEvent 227 245 0 0 0 0 0\n"
"MouseMoveEvent 228 250 0 0 0 0 0\n"
"MouseMoveEvent 230 256 0 0 0 0 0\n"
"MouseMoveEvent 232 260 0 0 0 0 0\n"
"MouseMoveEvent 232 265 0 0 0 0 0\n"
"MouseMoveEvent 233 269 0 0 0 0 0\n"
"MouseMoveEvent 235 270 0 0 0 0 0\n"
"MouseMoveEvent 235 271 0 0 0 0 0\n"
"MouseMoveEvent 235 272 0 0 0 0 0\n"
"MouseMoveEvent 235 273 0 0 0 0 0\n"
"MouseMoveEvent 235 274 0 0 0 0 0\n"
"MouseMoveEvent 236 276 0 0 0 0 0\n"
"MouseMoveEvent 236 277 0 0 0 0 0\n"
"MouseMoveEvent 237 278 0 0 0 0 0\n"
"MouseMoveEvent 237 279 0 0 0 0 0\n"
"MouseMoveEvent 238 281 0 0 0 0 0\n"
"MouseMoveEvent 239 282 0 0 0 0 0\n"
"MouseMoveEvent 240 283 0 0 0 0 0\n"
"MouseMoveEvent 240 284 0 0 0 0 0\n"
"MouseMoveEvent 241 285 0 0 0 0 0\n"
"MouseMoveEvent 242 286 0 0 0 0 0\n"
"MouseMoveEvent 243 288 0 0 0 0 0\n"
"MouseMoveEvent 244 289 0 0 0 0 0\n"
"MouseMoveEvent 245 290 0 0 0 0 0\n"
"MouseMoveEvent 246 292 0 0 0 0 0\n"
"MouseMoveEvent 247 293 0 0 0 0 0\n"
"MouseMoveEvent 248 294 0 0 0 0 0\n"
"MouseMoveEvent 249 295 0 0 0 0 0\n"
"MouseMoveEvent 250 297 0 0 0 0 0\n"
"MouseMoveEvent 251 299 0 0 0 0 0\n"
"LeaveEvent 252 300 0 0 0 0 0\n"
"ExitEvent 252 300 0 0 0 0 0\n"
;

int TestGPURayCastTwoComponentsDependent(int argc, char *argv[])
{
  cout << "CTEST_FULL_OUTPUT (Avoid ctest truncation of output)" << endl;

  int dims[3] = {25, 25, 25};

  // Create a vtkImageData with two components
  vtkNew<vtkImageData> image;
  image->SetDimensions(dims[0], dims[1], dims[2]);
  image->AllocateScalars(VTK_FLOAT, 2);

  // Fill the first half rectangular parallelopiped along X with the
  // first component values and the second half with second component values
  float * ptr = static_cast<float *> (image->GetScalarPointer(0, 0, 0));


  for (int z = 0; z < dims[2]; ++z)
    {
    for (int y = 0; y < dims[1]; ++y)
      {
      for (int x = 0; x < dims[0]; ++x)
        {
        double itr = floor(x / 5.0);
        *ptr++ = itr;
        *ptr++ = itr / 5.0;
        }
      }
    }

  vtkNew<vtkRenderWindow> renWin;
  renWin->SetSize(301, 300); // Intentional NPOT size
  renWin->SetMultiSamples(0);

  vtkNew<vtkRenderer> ren;
  renWin->AddRenderer(ren.GetPointer());

  vtkNew<vtkRenderWindowInteractor> iren;
  vtkNew<vtkInteractorStyleTrackballCamera> style;
  iren->SetInteractorStyle(style.GetPointer());
  iren->SetRenderWindow(renWin.GetPointer());

  renWin->Render();

  // Volume render the dataset
  vtkNew<vtkGPUVolumeRayCastMapper> mapper;
  mapper->AutoAdjustSampleDistancesOff();
  mapper->SetSampleDistance(0.9);
  mapper->SetInputData(image.GetPointer());

  // Color transfer function
  vtkNew<vtkColorTransferFunction> ctf;
  ctf->AddRGBPoint(0.0, 1.0, 0.0, 0.0);
  ctf->AddRGBPoint(1.0, 1.0, 1.0, 0.0);
  ctf->AddRGBPoint(2.0, 1.0, 1.0, 1.0);
  ctf->AddRGBPoint(3.0, 0.0, 1.0, 1.0);
  ctf->AddRGBPoint(4.0, 0.0, 0.0, 1.0);

  // Opacity functions
  vtkNew<vtkPiecewiseFunction> pf;
  pf->AddPoint(0.0, 0.1);
  pf->AddPoint(0.2, 1.0);
  pf->AddPoint(0.4, 0.1);
  pf->AddPoint(0.6, 1.0);
  pf->AddPoint(0.8, 0.1);

  // Volume property with independent components ON
  vtkNew<vtkVolumeProperty> property;
  property->IndependentComponentsOff();
  property->SetInterpolationTypeToLinear();

  // Set color and opacity functions
  property->SetColor(ctf.GetPointer());
  property->SetScalarOpacity(pf.GetPointer());

  vtkNew<vtkVolume> volume;
  volume->SetMapper(mapper.GetPointer());
  volume->SetProperty(property.GetPointer());
  ren->AddVolume(volume.GetPointer());

  ren->ResetCamera();

  iren->Initialize();
  renWin->Render();

  return vtkTesting::InteractorEventLoop(argc, argv,
                                         iren.GetPointer(),
                                         TestGPURayCastTwoComponentsDependentLog);
}
