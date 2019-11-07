
#include "vtkActor.h"
#include "vtkIntArray.h"
#include "vtkInteractorEventRecorder.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleTreeMapHover.h"
#include "vtkMath.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkPointData.h"
#include "vtkPolyDataMapper.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkSmartPointer.h"
#include "vtkSquarifyLayoutStrategy.h"
#include "vtkStdString.h"
#include "vtkTree.h"
#include "vtkTreeFieldAggregator.h"
#include "vtkTreeMapLayout.h"
#include "vtkTreeMapToPolyData.h"

#define VTK_CREATE(type,name) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

static char InteractorStyleTreeMapHoverEventLog[] =
"# StreamVersion 1\n"
"RenderEvent 0 0 0 0 0 0 0\n"
"EnterEvent 266 4 0 0 0 0 0\n"
"MouseMoveEvent 266 4 0 0 0 0 0\n"
"RenderEvent 266 4 0 0 0 0 0\n"
"MouseMoveEvent 252 21 0 0 0 0 0\n"
"RenderEvent 252 21 0 0 0 0 0\n"
"MouseMoveEvent 234 44 0 0 0 0 0\n"
"RenderEvent 234 44 0 0 0 0 0\n"
"MouseMoveEvent 216 66 0 0 0 0 0\n"
"RenderEvent 216 66 0 0 0 0 0\n"
"MouseMoveEvent 204 81 0 0 0 0 0\n"
"RenderEvent 204 81 0 0 0 0 0\n"
"MouseMoveEvent 193 93 0 0 0 0 0\n"
"RenderEvent 193 93 0 0 0 0 0\n"
"MouseMoveEvent 182 105 0 0 0 0 0\n"
"RenderEvent 182 105 0 0 0 0 0\n"
"MouseMoveEvent 169 120 0 0 0 0 0\n"
"RenderEvent 169 120 0 0 0 0 0\n"
"MouseMoveEvent 160 134 0 0 0 0 0\n"
"RenderEvent 160 134 0 0 0 0 0\n"
"MouseMoveEvent 153 144 0 0 0 0 0\n"
"RenderEvent 153 144 0 0 0 0 0\n"
"MouseMoveEvent 149 154 0 0 0 0 0\n"
"RenderEvent 149 154 0 0 0 0 0\n"
"MouseMoveEvent 145 163 0 0 0 0 0\n"
"RenderEvent 145 163 0 0 0 0 0\n"
"MouseMoveEvent 141 171 0 0 0 0 0\n"
"RenderEvent 141 171 0 0 0 0 0\n"
"MouseMoveEvent 138 175 0 0 0 0 0\n"
"RenderEvent 138 175 0 0 0 0 0\n"
"MouseMoveEvent 136 177 0 0 0 0 0\n"
"RenderEvent 136 177 0 0 0 0 0\n"
"MouseMoveEvent 135 178 0 0 0 0 0\n"
"RenderEvent 135 178 0 0 0 0 0\n"
"MouseMoveEvent 134 179 0 0 0 0 0\n"
"RenderEvent 134 179 0 0 0 0 0\n"
"RightButtonPressEvent 134 179 0 0 0 0 0\n"
"StartInteractionEvent 134 179 0 0 0 0 0\n"
"MouseMoveEvent 134 180 0 0 0 0 0\n"
"RenderEvent 134 180 0 0 0 0 0\n"
"RenderEvent 134 180 0 0 0 0 0\n"
"MouseMoveEvent 134 183 0 0 0 0 0\n"
"RenderEvent 134 183 0 0 0 0 0\n"
"RenderEvent 134 183 0 0 0 0 0\n"
"MouseMoveEvent 134 187 0 0 0 0 0\n"
"RenderEvent 134 187 0 0 0 0 0\n"
"RenderEvent 134 187 0 0 0 0 0\n"
"MouseMoveEvent 134 192 0 0 0 0 0\n"
"RenderEvent 134 192 0 0 0 0 0\n"
"RenderEvent 134 192 0 0 0 0 0\n"
"MouseMoveEvent 132 201 0 0 0 0 0\n"
"RenderEvent 132 201 0 0 0 0 0\n"
"RenderEvent 132 201 0 0 0 0 0\n"
"MouseMoveEvent 131 211 0 0 0 0 0\n"
"RenderEvent 131 211 0 0 0 0 0\n"
"RenderEvent 131 211 0 0 0 0 0\n"
"MouseMoveEvent 131 219 0 0 0 0 0\n"
"RenderEvent 131 219 0 0 0 0 0\n"
"RenderEvent 131 219 0 0 0 0 0\n"
"MouseMoveEvent 131 226 0 0 0 0 0\n"
"RenderEvent 131 226 0 0 0 0 0\n"
"RenderEvent 131 226 0 0 0 0 0\n"
"MouseMoveEvent 131 231 0 0 0 0 0\n"
"RenderEvent 131 231 0 0 0 0 0\n"
"RenderEvent 131 231 0 0 0 0 0\n"
"MouseMoveEvent 131 235 0 0 0 0 0\n"
"RenderEvent 131 235 0 0 0 0 0\n"
"RenderEvent 131 235 0 0 0 0 0\n"
"MouseMoveEvent 131 239 0 0 0 0 0\n"
"RenderEvent 131 239 0 0 0 0 0\n"
"RenderEvent 131 239 0 0 0 0 0\n"
"MouseMoveEvent 131 242 0 0 0 0 0\n"
"RenderEvent 131 242 0 0 0 0 0\n"
"RenderEvent 131 242 0 0 0 0 0\n"
"MouseMoveEvent 131 247 0 0 0 0 0\n"
"RenderEvent 131 247 0 0 0 0 0\n"
"RenderEvent 131 247 0 0 0 0 0\n"
"MouseMoveEvent 131 249 0 0 0 0 0\n"
"RenderEvent 131 249 0 0 0 0 0\n"
"RenderEvent 131 249 0 0 0 0 0\n"
"MouseMoveEvent 131 251 0 0 0 0 0\n"
"RenderEvent 131 251 0 0 0 0 0\n"
"RenderEvent 131 251 0 0 0 0 0\n"
"MouseMoveEvent 131 253 0 0 0 0 0\n"
"RenderEvent 131 253 0 0 0 0 0\n"
"RenderEvent 131 253 0 0 0 0 0\n"
"MouseMoveEvent 131 256 0 0 0 0 0\n"
"RenderEvent 131 256 0 0 0 0 0\n"
"RenderEvent 131 256 0 0 0 0 0\n"
"MouseMoveEvent 131 260 0 0 0 0 0\n"
"RenderEvent 131 260 0 0 0 0 0\n"
"RenderEvent 131 260 0 0 0 0 0\n"
"MouseMoveEvent 131 263 0 0 0 0 0\n"
"RenderEvent 131 263 0 0 0 0 0\n"
"RenderEvent 131 263 0 0 0 0 0\n"
"MouseMoveEvent 131 264 0 0 0 0 0\n"
"RenderEvent 131 264 0 0 0 0 0\n"
"RenderEvent 131 264 0 0 0 0 0\n"
"MouseMoveEvent 131 262 0 0 0 0 0\n"
"RenderEvent 131 262 0 0 0 0 0\n"
"RenderEvent 131 262 0 0 0 0 0\n"
"MouseMoveEvent 131 261 0 0 0 0 0\n"
"RenderEvent 131 261 0 0 0 0 0\n"
"RenderEvent 131 261 0 0 0 0 0\n"
"MouseMoveEvent 131 260 0 0 0 0 0\n"
"RenderEvent 131 260 0 0 0 0 0\n"
"RenderEvent 131 260 0 0 0 0 0\n"
"MouseMoveEvent 131 259 0 0 0 0 0\n"
"RenderEvent 131 259 0 0 0 0 0\n"
"RenderEvent 131 259 0 0 0 0 0\n"
"MouseMoveEvent 131 257 0 0 0 0 0\n"
"RenderEvent 131 257 0 0 0 0 0\n"
"RenderEvent 131 257 0 0 0 0 0\n"
"MouseMoveEvent 131 255 0 0 0 0 0\n"
"RenderEvent 131 255 0 0 0 0 0\n"
"RenderEvent 131 255 0 0 0 0 0\n"
"MouseMoveEvent 131 254 0 0 0 0 0\n"
"RenderEvent 131 254 0 0 0 0 0\n"
"RenderEvent 131 254 0 0 0 0 0\n"
"MouseMoveEvent 131 252 0 0 0 0 0\n"
"RenderEvent 131 252 0 0 0 0 0\n"
"RenderEvent 131 252 0 0 0 0 0\n"
"MouseMoveEvent 131 251 0 0 0 0 0\n"
"RenderEvent 131 251 0 0 0 0 0\n"
"RenderEvent 131 251 0 0 0 0 0\n"
"MouseMoveEvent 131 250 0 0 0 0 0\n"
"RenderEvent 131 250 0 0 0 0 0\n"
"RenderEvent 131 250 0 0 0 0 0\n"
"MouseMoveEvent 131 248 0 0 0 0 0\n"
"RenderEvent 131 248 0 0 0 0 0\n"
"RenderEvent 131 248 0 0 0 0 0\n"
"MouseMoveEvent 131 245 0 0 0 0 0\n"
"RenderEvent 131 245 0 0 0 0 0\n"
"RenderEvent 131 245 0 0 0 0 0\n"
"MouseMoveEvent 131 241 0 0 0 0 0\n"
"RenderEvent 131 241 0 0 0 0 0\n"
"RenderEvent 131 241 0 0 0 0 0\n"
"MouseMoveEvent 131 238 0 0 0 0 0\n"
"RenderEvent 131 238 0 0 0 0 0\n"
"RenderEvent 131 238 0 0 0 0 0\n"
"MouseMoveEvent 131 233 0 0 0 0 0\n"
"RenderEvent 131 233 0 0 0 0 0\n"
"RenderEvent 131 233 0 0 0 0 0\n"
"MouseMoveEvent 131 226 0 0 0 0 0\n"
"RenderEvent 131 226 0 0 0 0 0\n"
"RenderEvent 131 226 0 0 0 0 0\n"
"MouseMoveEvent 131 219 0 0 0 0 0\n"
"RenderEvent 131 219 0 0 0 0 0\n"
"RenderEvent 131 219 0 0 0 0 0\n"
"MouseMoveEvent 131 214 0 0 0 0 0\n"
"RenderEvent 131 214 0 0 0 0 0\n"
"RenderEvent 131 214 0 0 0 0 0\n"
"MouseMoveEvent 131 211 0 0 0 0 0\n"
"RenderEvent 131 211 0 0 0 0 0\n"
"RenderEvent 131 211 0 0 0 0 0\n"
"MouseMoveEvent 131 209 0 0 0 0 0\n"
"RenderEvent 131 209 0 0 0 0 0\n"
"RenderEvent 131 209 0 0 0 0 0\n"
"MouseMoveEvent 131 207 0 0 0 0 0\n"
"RenderEvent 131 207 0 0 0 0 0\n"
"RenderEvent 131 207 0 0 0 0 0\n"
"MouseMoveEvent 131 206 0 0 0 0 0\n"
"RenderEvent 131 206 0 0 0 0 0\n"
"RenderEvent 131 206 0 0 0 0 0\n"
"MouseMoveEvent 131 205 0 0 0 0 0\n"
"RenderEvent 131 205 0 0 0 0 0\n"
"RenderEvent 131 205 0 0 0 0 0\n"
"MouseMoveEvent 131 204 0 0 0 0 0\n"
"RenderEvent 131 204 0 0 0 0 0\n"
"RenderEvent 131 204 0 0 0 0 0\n"
"MouseMoveEvent 131 202 0 0 0 0 0\n"
"RenderEvent 131 202 0 0 0 0 0\n"
"RenderEvent 131 202 0 0 0 0 0\n"
"MouseMoveEvent 131 201 0 0 0 0 0\n"
"RenderEvent 131 201 0 0 0 0 0\n"
"RenderEvent 131 201 0 0 0 0 0\n"
"MouseMoveEvent 131 199 0 0 0 0 0\n"
"RenderEvent 131 199 0 0 0 0 0\n"
"RenderEvent 131 199 0 0 0 0 0\n"
"MouseMoveEvent 132 197 0 0 0 0 0\n"
"RenderEvent 132 197 0 0 0 0 0\n"
"RenderEvent 132 197 0 0 0 0 0\n"
"MouseMoveEvent 132 195 0 0 0 0 0\n"
"RenderEvent 132 195 0 0 0 0 0\n"
"RenderEvent 132 195 0 0 0 0 0\n"
"RightButtonReleaseEvent 132 195 0 0 0 0 0\n"
"EndInteractionEvent 132 195 0 0 0 0 0\n"
"RenderEvent 132 195 0 0 0 0 0\n"
"MouseMoveEvent 132 195 0 0 0 0 0\n"
"RenderEvent 132 195 0 0 0 0 0\n"
"KeyPressEvent 132 195 -128 0 0 1 Control_L\n"
"LeftButtonPressEvent 132 195 8 0 0 0 Control_L\n"
"StartInteractionEvent 132 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 133 195 8 0 0 0 Control_L\n"
"RenderEvent 133 195 8 0 0 0 Control_L\n"
"RenderEvent 133 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 134 195 8 0 0 0 Control_L\n"
"RenderEvent 134 195 8 0 0 0 Control_L\n"
"RenderEvent 134 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 135 195 8 0 0 0 Control_L\n"
"RenderEvent 135 195 8 0 0 0 Control_L\n"
"RenderEvent 135 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 136 196 8 0 0 0 Control_L\n"
"RenderEvent 136 196 8 0 0 0 Control_L\n"
"RenderEvent 136 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 137 196 8 0 0 0 Control_L\n"
"RenderEvent 137 196 8 0 0 0 Control_L\n"
"RenderEvent 137 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 139 196 8 0 0 0 Control_L\n"
"RenderEvent 139 196 8 0 0 0 Control_L\n"
"RenderEvent 139 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 140 196 8 0 0 0 Control_L\n"
"RenderEvent 140 196 8 0 0 0 Control_L\n"
"RenderEvent 140 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 141 196 8 0 0 0 Control_L\n"
"RenderEvent 141 196 8 0 0 0 Control_L\n"
"RenderEvent 141 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 142 196 8 0 0 0 Control_L\n"
"RenderEvent 142 196 8 0 0 0 Control_L\n"
"RenderEvent 142 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 143 196 8 0 0 0 Control_L\n"
"RenderEvent 143 196 8 0 0 0 Control_L\n"
"RenderEvent 143 196 8 0 0 0 Control_L\n"
"MouseMoveEvent 144 196 8 0 0 0 Control_L\n"
"RenderEvent 144 196 8 0 0 0 Control_L\n"
"RenderEvent 144 196 8 0 0 0 Control_L\n"
"KeyPressEvent 144 196 -128 0 0 1 Control_L\n"
"MouseMoveEvent 144 195 8 0 0 0 Control_L\n"
"RenderEvent 144 195 8 0 0 0 Control_L\n"
"RenderEvent 144 195 8 0 0 0 Control_L\n"
"KeyPressEvent 144 195 -128 0 0 1 Control_L\n"
"MouseMoveEvent 145 195 8 0 0 0 Control_L\n"
"RenderEvent 145 195 8 0 0 0 Control_L\n"
"RenderEvent 145 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 145 194 8 0 0 0 Control_L\n"
"RenderEvent 145 194 8 0 0 0 Control_L\n"
"RenderEvent 145 194 8 0 0 0 Control_L\n"
"KeyPressEvent 145 194 -128 0 0 1 Control_L\n"
"KeyPressEvent 145 194 -128 0 0 1 Control_L\n"
"KeyPressEvent 145 194 -128 0 0 1 Control_L\n"
"KeyPressEvent 145 194 -128 0 0 1 Control_L\n"
"MouseMoveEvent 146 193 8 0 0 0 Control_L\n"
"RenderEvent 146 193 8 0 0 0 Control_L\n"
"RenderEvent 146 193 8 0 0 0 Control_L\n"
"MouseMoveEvent 146 192 8 0 0 0 Control_L\n"
"RenderEvent 146 192 8 0 0 0 Control_L\n"
"RenderEvent 146 192 8 0 0 0 Control_L\n"
"KeyPressEvent 146 192 -128 0 0 1 Control_L\n"
"MouseMoveEvent 147 192 8 0 0 0 Control_L\n"
"RenderEvent 147 192 8 0 0 0 Control_L\n"
"RenderEvent 147 192 8 0 0 0 Control_L\n"
"KeyPressEvent 147 192 -128 0 0 1 Control_L\n"
"KeyPressEvent 147 192 -128 0 0 1 Control_L\n"
"KeyPressEvent 147 192 -128 0 0 1 Control_L\n"
"KeyPressEvent 147 192 -128 0 0 1 Control_L\n"
"KeyPressEvent 147 192 -128 0 0 1 Control_L\n"
"MouseMoveEvent 147 193 8 0 0 0 Control_L\n"
"RenderEvent 147 193 8 0 0 0 Control_L\n"
"RenderEvent 147 193 8 0 0 0 Control_L\n"
"KeyPressEvent 147 193 -128 0 0 1 Control_L\n"
"MouseMoveEvent 146 193 8 0 0 0 Control_L\n"
"RenderEvent 146 193 8 0 0 0 Control_L\n"
"RenderEvent 146 193 8 0 0 0 Control_L\n"
"MouseMoveEvent 145 193 8 0 0 0 Control_L\n"
"RenderEvent 145 193 8 0 0 0 Control_L\n"
"RenderEvent 145 193 8 0 0 0 Control_L\n"
"KeyPressEvent 145 193 -128 0 0 1 Control_L\n"
"MouseMoveEvent 145 194 8 0 0 0 Control_L\n"
"RenderEvent 145 194 8 0 0 0 Control_L\n"
"RenderEvent 145 194 8 0 0 0 Control_L\n"
"MouseMoveEvent 144 194 8 0 0 0 Control_L\n"
"RenderEvent 144 194 8 0 0 0 Control_L\n"
"RenderEvent 144 194 8 0 0 0 Control_L\n"
"KeyPressEvent 144 194 -128 0 0 1 Control_L\n"
"MouseMoveEvent 144 195 8 0 0 0 Control_L\n"
"RenderEvent 144 195 8 0 0 0 Control_L\n"
"RenderEvent 144 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 143 195 8 0 0 0 Control_L\n"
"RenderEvent 143 195 8 0 0 0 Control_L\n"
"RenderEvent 143 195 8 0 0 0 Control_L\n"
"MouseMoveEvent 142 195 8 0 0 0 Control_L\n"
"RenderEvent 142 195 8 0 0 0 Control_L\n"
"RenderEvent 142 195 8 0 0 0 Control_L\n"
"KeyPressEvent 142 195 -128 0 0 1 Control_L\n"
"MouseMoveEvent 141 196 8 0 0 0 Control_L\n"
"RenderEvent 141 196 8 0 0 0 Control_L\n"
"RenderEvent 141 196 8 0 0 0 Control_L\n"
"KeyPressEvent 141 196 -128 0 0 1 Control_L\n"
"MouseMoveEvent 140 196 8 0 0 0 Control_L\n"
"RenderEvent 140 196 8 0 0 0 Control_L\n"
"RenderEvent 140 196 8 0 0 0 Control_L\n"
"KeyPressEvent 140 196 -128 0 0 1 Control_L\n"
"MouseMoveEvent 138 196 8 0 0 0 Control_L\n"
"RenderEvent 138 196 8 0 0 0 Control_L\n"
"RenderEvent 138 196 8 0 0 0 Control_L\n"
"KeyPressEvent 138 196 -128 0 0 1 Control_L\n"
"MouseMoveEvent 137 197 8 0 0 0 Control_L\n"
"RenderEvent 137 197 8 0 0 0 Control_L\n"
"RenderEvent 137 197 8 0 0 0 Control_L\n"
"MouseMoveEvent 136 198 8 0 0 0 Control_L\n"
"RenderEvent 136 198 8 0 0 0 Control_L\n"
"RenderEvent 136 198 8 0 0 0 Control_L\n"
"KeyPressEvent 136 198 -128 0 0 1 Control_L\n"
"MouseMoveEvent 135 198 8 0 0 0 Control_L\n"
"RenderEvent 135 198 8 0 0 0 Control_L\n"
"RenderEvent 135 198 8 0 0 0 Control_L\n"
"MouseMoveEvent 134 198 8 0 0 0 Control_L\n"
"RenderEvent 134 198 8 0 0 0 Control_L\n"
"RenderEvent 134 198 8 0 0 0 Control_L\n"
"KeyPressEvent 134 198 -128 0 0 1 Control_L\n"
"MouseMoveEvent 133 199 8 0 0 0 Control_L\n"
"RenderEvent 133 199 8 0 0 0 Control_L\n"
"RenderEvent 133 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 131 199 8 0 0 0 Control_L\n"
"RenderEvent 131 199 8 0 0 0 Control_L\n"
"RenderEvent 131 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 130 199 8 0 0 0 Control_L\n"
"RenderEvent 130 199 8 0 0 0 Control_L\n"
"RenderEvent 130 199 8 0 0 0 Control_L\n"
"KeyPressEvent 130 199 -128 0 0 1 Control_L\n"
"MouseMoveEvent 127 199 8 0 0 0 Control_L\n"
"RenderEvent 127 199 8 0 0 0 Control_L\n"
"RenderEvent 127 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 126 199 8 0 0 0 Control_L\n"
"RenderEvent 126 199 8 0 0 0 Control_L\n"
"RenderEvent 126 199 8 0 0 0 Control_L\n"
"KeyPressEvent 126 199 -128 0 0 1 Control_L\n"
"MouseMoveEvent 125 199 8 0 0 0 Control_L\n"
"RenderEvent 125 199 8 0 0 0 Control_L\n"
"RenderEvent 125 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 124 199 8 0 0 0 Control_L\n"
"RenderEvent 124 199 8 0 0 0 Control_L\n"
"RenderEvent 124 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 123 199 8 0 0 0 Control_L\n"
"RenderEvent 123 199 8 0 0 0 Control_L\n"
"RenderEvent 123 199 8 0 0 0 Control_L\n"
"KeyPressEvent 123 199 -128 0 0 1 Control_L\n"
"MouseMoveEvent 122 199 8 0 0 0 Control_L\n"
"RenderEvent 122 199 8 0 0 0 Control_L\n"
"RenderEvent 122 199 8 0 0 0 Control_L\n"
"KeyPressEvent 122 199 -128 0 0 1 Control_L\n"
"MouseMoveEvent 121 199 8 0 0 0 Control_L\n"
"RenderEvent 121 199 8 0 0 0 Control_L\n"
"RenderEvent 121 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 120 199 8 0 0 0 Control_L\n"
"RenderEvent 120 199 8 0 0 0 Control_L\n"
"RenderEvent 120 199 8 0 0 0 Control_L\n"
"KeyPressEvent 120 199 -128 0 0 1 Control_L\n"
"MouseMoveEvent 118 199 8 0 0 0 Control_L\n"
"RenderEvent 118 199 8 0 0 0 Control_L\n"
"RenderEvent 118 199 8 0 0 0 Control_L\n"
"MouseMoveEvent 117 199 8 0 0 0 Control_L\n"
"RenderEvent 117 199 8 0 0 0 Control_L\n"
"RenderEvent 117 199 8 0 0 0 Control_L\n"
"KeyPressEvent 117 199 -128 0 0 1 Control_L\n"
"MouseMoveEvent 116 200 8 0 0 0 Control_L\n"
"RenderEvent 116 200 8 0 0 0 Control_L\n"
"RenderEvent 116 200 8 0 0 0 Control_L\n"
"KeyPressEvent 116 200 -128 0 0 1 Control_L\n"
"MouseMoveEvent 116 201 8 0 0 0 Control_L\n"
"RenderEvent 116 201 8 0 0 0 Control_L\n"
"RenderEvent 116 201 8 0 0 0 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"LeftButtonReleaseEvent 116 201 8 0 0 0 Control_L\n"
"RenderEvent 116 201 8 0 0 0 Control_L\n"
"EndInteractionEvent 116 201 8 0 0 0 Control_L\n"
"RenderEvent 116 201 8 0 0 0 Control_L\n"
"MouseMoveEvent 116 201 8 0 0 0 Control_L\n"
"RenderEvent 116 201 8 0 0 0 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyPressEvent 116 201 -128 0 0 1 Control_L\n"
"KeyReleaseEvent 116 201 0 0 0 1 Control_L\n"
"MouseMoveEvent 116 202 0 0 0 0 Control_L\n"
"RenderEvent 116 202 0 0 0 0 Control_L\n"
"MouseMoveEvent 116 203 0 0 0 0 Control_L\n"
"RenderEvent 116 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 118 203 0 0 0 0 Control_L\n"
"RenderEvent 118 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 122 203 0 0 0 0 Control_L\n"
"RenderEvent 122 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 126 203 0 0 0 0 Control_L\n"
"RenderEvent 126 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 132 203 0 0 0 0 Control_L\n"
"RenderEvent 132 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 138 203 0 0 0 0 Control_L\n"
"RenderEvent 138 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 145 203 0 0 0 0 Control_L\n"
"RenderEvent 145 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 153 203 0 0 0 0 Control_L\n"
"RenderEvent 153 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 157 203 0 0 0 0 Control_L\n"
"RenderEvent 157 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 160 204 0 0 0 0 Control_L\n"
"RenderEvent 160 204 0 0 0 0 Control_L\n"
"MouseMoveEvent 163 206 0 0 0 0 Control_L\n"
"RenderEvent 163 206 0 0 0 0 Control_L\n"
"MouseMoveEvent 166 208 0 0 0 0 Control_L\n"
"RenderEvent 166 208 0 0 0 0 Control_L\n"
"MouseMoveEvent 168 209 0 0 0 0 Control_L\n"
"RenderEvent 168 209 0 0 0 0 Control_L\n"
"MouseMoveEvent 170 209 0 0 0 0 Control_L\n"
"RenderEvent 170 209 0 0 0 0 Control_L\n"
"MouseMoveEvent 172 209 0 0 0 0 Control_L\n"
"RenderEvent 172 209 0 0 0 0 Control_L\n"
"MouseMoveEvent 175 209 0 0 0 0 Control_L\n"
"RenderEvent 175 209 0 0 0 0 Control_L\n"
"MouseMoveEvent 177 209 0 0 0 0 Control_L\n"
"RenderEvent 177 209 0 0 0 0 Control_L\n"
"MouseMoveEvent 179 208 0 0 0 0 Control_L\n"
"RenderEvent 179 208 0 0 0 0 Control_L\n"
"MouseMoveEvent 180 207 0 0 0 0 Control_L\n"
"RenderEvent 180 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 182 207 0 0 0 0 Control_L\n"
"RenderEvent 182 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 183 207 0 0 0 0 Control_L\n"
"RenderEvent 183 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 185 207 0 0 0 0 Control_L\n"
"RenderEvent 185 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 187 207 0 0 0 0 Control_L\n"
"RenderEvent 187 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 188 207 0 0 0 0 Control_L\n"
"RenderEvent 188 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 189 207 0 0 0 0 Control_L\n"
"RenderEvent 189 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 190 207 0 0 0 0 Control_L\n"
"RenderEvent 190 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 191 207 0 0 0 0 Control_L\n"
"RenderEvent 191 207 0 0 0 0 Control_L\n"
"MouseMoveEvent 191 206 0 0 0 0 Control_L\n"
"RenderEvent 191 206 0 0 0 0 Control_L\n"
"MouseMoveEvent 192 202 0 0 0 0 Control_L\n"
"RenderEvent 192 202 0 0 0 0 Control_L\n"
"MouseMoveEvent 193 198 0 0 0 0 Control_L\n"
"RenderEvent 193 198 0 0 0 0 Control_L\n"
"MouseMoveEvent 194 193 0 0 0 0 Control_L\n"
"RenderEvent 194 193 0 0 0 0 Control_L\n"
"MouseMoveEvent 196 186 0 0 0 0 Control_L\n"
"RenderEvent 196 186 0 0 0 0 Control_L\n"
"MouseMoveEvent 198 181 0 0 0 0 Control_L\n"
"RenderEvent 198 181 0 0 0 0 Control_L\n"
"MouseMoveEvent 199 176 0 0 0 0 Control_L\n"
"RenderEvent 199 176 0 0 0 0 Control_L\n"
"MouseMoveEvent 201 174 0 0 0 0 Control_L\n"
"RenderEvent 201 174 0 0 0 0 Control_L\n"
"MouseMoveEvent 202 172 0 0 0 0 Control_L\n"
"RenderEvent 202 172 0 0 0 0 Control_L\n"
"MouseMoveEvent 202 171 0 0 0 0 Control_L\n"
"RenderEvent 202 171 0 0 0 0 Control_L\n"
"MouseMoveEvent 202 171 0 0 0 0 Control_L\n"
"RenderEvent 202 171 0 0 0 0 Control_L\n"
"MouseMoveEvent 202 171 0 0 0 0 Control_L\n"
"RenderEvent 202 171 0 0 0 0 Control_L\n"
"LeftButtonPressEvent 202 171 0 0 0 0 Control_L\n"
"StartInteractionEvent 202 171 0 0 0 0 Control_L\n"
"LeftButtonReleaseEvent 202 171 0 0 0 0 Control_L\n"
"RenderEvent 202 171 0 0 0 0 Control_L\n"
"EndInteractionEvent 202 171 0 0 0 0 Control_L\n"
"RenderEvent 202 171 0 0 0 0 Control_L\n"
"MouseMoveEvent 202 171 0 0 0 0 Control_L\n"
"RenderEvent 202 171 0 0 0 0 Control_L\n"
"MouseMoveEvent 200 176 0 0 0 0 Control_L\n"
"RenderEvent 200 176 0 0 0 0 Control_L\n"
"MouseMoveEvent 200 179 0 0 0 0 Control_L\n"
"RenderEvent 200 179 0 0 0 0 Control_L\n"
"MouseMoveEvent 199 181 0 0 0 0 Control_L\n"
"RenderEvent 199 181 0 0 0 0 Control_L\n"
"MouseMoveEvent 198 183 0 0 0 0 Control_L\n"
"RenderEvent 198 183 0 0 0 0 Control_L\n"
"MouseMoveEvent 196 184 0 0 0 0 Control_L\n"
"RenderEvent 196 184 0 0 0 0 Control_L\n"
"MouseMoveEvent 194 185 0 0 0 0 Control_L\n"
"RenderEvent 194 185 0 0 0 0 Control_L\n"
"MouseMoveEvent 192 185 0 0 0 0 Control_L\n"
"RenderEvent 192 185 0 0 0 0 Control_L\n"
"MouseMoveEvent 191 186 0 0 0 0 Control_L\n"
"RenderEvent 191 186 0 0 0 0 Control_L\n"
"MouseMoveEvent 189 187 0 0 0 0 Control_L\n"
"RenderEvent 189 187 0 0 0 0 Control_L\n"
"MouseMoveEvent 187 188 0 0 0 0 Control_L\n"
"RenderEvent 187 188 0 0 0 0 Control_L\n"
"MouseMoveEvent 185 190 0 0 0 0 Control_L\n"
"RenderEvent 185 190 0 0 0 0 Control_L\n"
"MouseMoveEvent 184 192 0 0 0 0 Control_L\n"
"RenderEvent 184 192 0 0 0 0 Control_L\n"
"MouseMoveEvent 181 194 0 0 0 0 Control_L\n"
"RenderEvent 181 194 0 0 0 0 Control_L\n"
"MouseMoveEvent 181 197 0 0 0 0 Control_L\n"
"RenderEvent 181 197 0 0 0 0 Control_L\n"
"MouseMoveEvent 179 199 0 0 0 0 Control_L\n"
"RenderEvent 179 199 0 0 0 0 Control_L\n"
"MouseMoveEvent 178 200 0 0 0 0 Control_L\n"
"RenderEvent 178 200 0 0 0 0 Control_L\n"
"MouseMoveEvent 178 201 0 0 0 0 Control_L\n"
"RenderEvent 178 201 0 0 0 0 Control_L\n"
"MouseMoveEvent 177 201 0 0 0 0 Control_L\n"
"RenderEvent 177 201 0 0 0 0 Control_L\n"
"MouseMoveEvent 176 203 0 0 0 0 Control_L\n"
"RenderEvent 176 203 0 0 0 0 Control_L\n"
"MouseMoveEvent 175 204 0 0 0 0 Control_L\n"
"RenderEvent 175 204 0 0 0 0 Control_L\n"
"MouseMoveEvent 174 206 0 0 0 0 Control_L\n"
"RenderEvent 174 206 0 0 0 0 Control_L\n"
"MouseMoveEvent 173 208 0 0 0 0 Control_L\n"
"RenderEvent 173 208 0 0 0 0 Control_L\n"
"MouseMoveEvent 173 210 0 0 0 0 Control_L\n"
"RenderEvent 173 210 0 0 0 0 Control_L\n"
"MouseMoveEvent 172 211 0 0 0 0 Control_L\n"
"RenderEvent 172 211 0 0 0 0 Control_L\n"
"MouseMoveEvent 171 211 0 0 0 0 Control_L\n"
"RenderEvent 171 211 0 0 0 0 Control_L\n"
"MouseMoveEvent 171 212 0 0 0 0 Control_L\n"
"RenderEvent 171 212 0 0 0 0 Control_L\n"
"MouseMoveEvent 170 212 0 0 0 0 Control_L\n"
"RenderEvent 170 212 0 0 0 0 Control_L\n"
"MouseMoveEvent 169 212 0 0 0 0 Control_L\n"
"RenderEvent 169 212 0 0 0 0 Control_L\n"
"MouseMoveEvent 169 213 0 0 0 0 Control_L\n"
"RenderEvent 169 213 0 0 0 0 Control_L\n"
"LeftButtonPressEvent 169 213 0 0 0 0 Control_L\n"
"StartInteractionEvent 169 213 0 0 0 0 Control_L\n"
"LeftButtonReleaseEvent 169 213 0 0 0 0 Control_L\n"
"RenderEvent 169 213 0 0 0 0 Control_L\n"
"EndInteractionEvent 169 213 0 0 0 0 Control_L\n"
"RenderEvent 169 213 0 0 0 0 Control_L\n"
"MouseMoveEvent 169 213 0 0 0 0 Control_L\n"
;

//#define RECORD

int TestInteractorStyleTreeMapHover(int argc, char* argv[])
{
  // Create input
  VTK_CREATE(vtkMutableDirectedGraph, builder);
  VTK_CREATE(vtkIntArray, sizeArr);
  sizeArr->SetName("size");
  builder->GetVertexData()->AddArray(sizeArr);
  builder->AddVertex();
  sizeArr->InsertNextValue(0);
  builder->AddChild(0);
  sizeArr->InsertNextValue(15);
  builder->AddChild(0);
  sizeArr->InsertNextValue(50);
  builder->AddChild(0);
  sizeArr->InsertNextValue(0);
  builder->AddChild(3);
  sizeArr->InsertNextValue(2);
  builder->AddChild(3);
  sizeArr->InsertNextValue(12);
  builder->AddChild(3);
  sizeArr->InsertNextValue(10);
  builder->AddChild(3);
  sizeArr->InsertNextValue(8);
  builder->AddChild(3);
  sizeArr->InsertNextValue(6);
  builder->AddChild(3);
  sizeArr->InsertNextValue(4);

  VTK_CREATE(vtkTree, tree);
  if (!tree->CheckedShallowCopy(builder))
  {
    cerr << "Invalid tree structure." << endl;
  }

  VTK_CREATE(vtkTreeFieldAggregator, agg);
  agg->SetInputData(tree);
  agg->SetField("size");
  agg->SetLeafVertexUnitSize(false);

  VTK_CREATE(vtkTreeMapLayout, layout);
  VTK_CREATE(vtkSquarifyLayoutStrategy, box);
  box->SetShrinkPercentage(0.1);
  layout->SetInputConnection(agg->GetOutputPort());
  layout->SetLayoutStrategy(box);

  VTK_CREATE(vtkTreeMapToPolyData, poly);
  poly->SetInputConnection(layout->GetOutputPort());

  VTK_CREATE(vtkPolyDataMapper, mapper);
  mapper->SetInputConnection(poly->GetOutputPort());
  mapper->SetScalarModeToUseCellFieldData();
  mapper->SelectColorArray("size");
  mapper->SetScalarRange(0, 100);

  VTK_CREATE(vtkActor, actor);
  actor->SetMapper(mapper);

  VTK_CREATE(vtkRenderWindow, win);
  VTK_CREATE(vtkRenderer, ren);
  ren->AddActor(actor);
  win->AddRenderer(ren);
  VTK_CREATE(vtkRenderWindowInteractor, iren);
  VTK_CREATE(vtkInteractorStyleTreeMapHover, hover);
  hover->SetLabelField("size");
  hover->SetTreeMapToPolyData(poly);
  hover->SetLayout(layout);

  win->SetInteractor(iren);
  iren->SetInteractorStyle(hover);

  //TestStyle(hover);

  // record events
  VTK_CREATE(vtkInteractorEventRecorder, recorder);
  recorder->SetInteractor(iren);
#ifdef RECORD
  recorder->SetFileName("record.log");
  recorder->SetEnabled(true);
  recorder->Record();
#else
  recorder->ReadFromInputStringOn();
  recorder->SetInputString(InteractorStyleTreeMapHoverEventLog);
#endif

  // interact with data
  // render the image
  //
  iren->Initialize();
  win->Render();

#ifdef RECORD
#else
  recorder->Play();

  // Remove the observers so we can go interactive. Without this the "-I"
  // testing option fails.
  recorder->Off();
#endif

  win->Render();

  int retVal = vtkRegressionTestImage(win);
  if (retVal == vtkRegressionTester::DO_INTERACTOR)
  {
    win->Render();
    iren->Start();
    retVal = vtkRegressionTester::PASSED;
  }

  return !retVal;
}
