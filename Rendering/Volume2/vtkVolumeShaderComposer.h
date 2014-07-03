/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkOpenGLProjectedTetrahedraMapper.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef ___vtkVolumeShaderComposer_h
#define ___vtkVolumeShaderComposer_h

#include <vtkRenderer.h>
#include <vtkVolume.h>
#include <vtkVolumeMapper.h>
#include <vtkVolumeProperty.h>

#include <string>

namespace vtkvolume
{
  //--------------------------------------------------------------------------
  std::string replace(std::string source, const std::string &search,
                      const std::string replace, bool all)
    {
    std::string::size_type pos = 0;
    bool first = true;
    while ((pos = source.find(search, 0)) != std::string::npos)
      {
      source.replace(pos, search.length(), replace);
      pos += search.length();
      if (first)
        {
        first = false;
        if (!all)
          {
          return source;
          }
        }
      }
    return source;
    }

  //--------------------------------------------------------------------------
  std::string BaseUniformsVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                               vtkVolume* vol)
    { return std::string(
    "uniform mat4 m_modelview_matrix; \n\
    uniform mat4 m_projection_matrix; \n\
    uniform mat4 m_scene_matrix; \n\
    \n\
    uniform vec3 m_vol_extents_min; \n\
    uniform vec3 m_vol_extents_max; \n\
    \n\
    uniform vec3 m_texture_extents_max; \n\
    uniform vec3 m_texture_extents_min;"
    );
    }

  //--------------------------------------------------------------------------
  std::string BaseUniformsFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                               vtkVolume* vol)
    {
    return std::string(
      "/// Volume dataset \n\
      uniform sampler3D m_volume; \n\
      \n\
      /// Transfer functions \n\
      uniform sampler1D m_color_transfer_func; \n\
      uniform sampler1D m_opacity_transfer_func; \n\
      \n\
      uniform sampler2D m_noise_sampler; \n\
      uniform sampler2D m_depth_sampler; \n\
      \n\
      /// Camera position \n\
      uniform vec3 m_camera_pos; \n\
      uniform vec3 m_light_pos; \n\
      \n\
      /// view and model matrices \n\
      uniform mat4 m_scene_matrix; \n\
      uniform mat4 m_projection_matrix; \n\
      uniform mat4 m_modelview_matrix; \n\
      uniform mat4 m_texture_dataset_matrix; \n\
      \n\
      /// Ray step size \n\
      uniform vec3 m_step_size; \n\
      \n\
      /// Sample distance \n\
      uniform float m_sample_distance; \n\
      \n\
      /// Scales \n\
      uniform vec3 m_cell_m_scale; \n\
      uniform float m_scale; \n\
      uniform vec2 m_window_lower_left_corner; \n\
      uniform vec2 m_inv_original_window_size; \n\
      uniform vec2 m_inv_window_size; \n\
      uniform vec3 m_texture_extents_max; \n\
      uniform vec3 m_texture_extents_min; \n\
      \n\
      /// Material and lighting \n\
      uniform vec3 m_diffuse; \n\
      uniform vec3 m_ambient; \n\
      uniform vec3 m_specular; \n\
      uniform float m_shininess;"
      );
    }

  //--------------------------------------------------------------------------
  std::string BaseAttributesVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                 vtkVolume* vol)
    {
    return std::string(
      "layout(location = 0) in vec3 m_in_vertex_pos;"
    );
    }

  //--------------------------------------------------------------------------
  std::string BaseAttributesFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                 vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string TerminationUniformVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                     vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string TerminationAttributesVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                        vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string TerminationUniformsFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                      vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string TerminationAttributesFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                        vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string ShadingUniformsVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                  vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string ShadingAttributesVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                    vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string ShadingUniformsFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                  vtkVolume* vol)
    {
      return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string ShadingAttributesFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                    vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string ComputeClip(vtkRenderer* ren, vtkVolumeMapper* mapper,
                          vtkVolume* vol)
    {
    return std::string(
      "mat4 ogl_projection_matrix = transpose(m_projection_matrix); \n\
      mat4 ogl_modelview_matrix = transpose(m_modelview_matrix); \n\
      vec4 pos = ogl_projection_matrix * ogl_modelview_matrix * transpose(m_scene_matrix) * \n\
                 vec4(m_in_vertex_pos.xyz, 1); \n\
      gl_Position = pos;"
    );
    }

  //--------------------------------------------------------------------------
  std::string ComputeTextureCoords(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                   vtkVolume* vol)
    {
    return std::string(
      "/// Assuming point data only. Also, we offset the texture coordinate to account \n\
       /// for OpenGL treating voxel at the center of the cell. \n\
       vec3 uvx = (m_in_vertex_pos - m_vol_extents_min) / (m_vol_extents_max - m_vol_extents_min); \n\
       vec3 delta = m_texture_extents_max - m_texture_extents_min; \n\
       m_texture_coords = (uvx * (delta - vec3(1.0)) + vec3(0.5)) / delta;"
    );
    }

  //--------------------------------------------------------------------------
  std::string InitBase(vtkRenderer* ren, vtkVolumeMapper* mapper,
                       vtkVolume* vol)
    {
    return std::string(
      "vec3 l_light_pos_obj; \n\
      \n\
      /// inverse is available only on 120 or above \n\
      mat4 l_ogl_scene_matrix = inverse(transpose(m_scene_matrix)); \n\
      \n\
      /// Get the 3D texture coordinates for lookup into the m_volume dataset  \n\
      vec3 l_data_pos = m_texture_coords.xyz; \n\
      \n\
      /// Eye position in object space  \n\
      vec3 l_eye_pos_obj = (l_ogl_scene_matrix * vec4(m_camera_pos, 1.0)).xyz; \n\
      \n\
      /// Getting the ray marching direction (in object space); \n\
      vec3 geom_dir = normalize(m_vertex_pos.xyz - l_eye_pos_obj); \n\
      \n\
      /// Multiply the raymarching direction with the step size to get the \n\
      /// sub-step size we need to take at each raymarching step  \n\
      vec3 l_dir_step = geom_dir * m_step_size * m_sample_distance; \n\
      \n\
      l_data_pos += l_dir_step * texture(m_noise_sampler, l_data_pos.xy).x;\n\
      \n\
      /// Flag to deternmine if voxel should be considered for the rendering \n\
      bool l_skip = false;"
    );
    }

  //--------------------------------------------------------------------------
  std::string IncrementBase(vtkRenderer* ren, vtkVolumeMapper* mapper,
                            vtkVolume* vol)
    {
    return std::string("\n\
                       l_skip = false;");
    }

  //--------------------------------------------------------------------------
  std::string ExitBase(vtkRenderer* ren, vtkVolumeMapper* mapper, vtkVolume* vol)
    {
    return std::string("");
    }

  //--------------------------------------------------------------------------
  std::string InitShading(vtkRenderer* vtkNotUsed(ren), vtkVolumeMapper* mapper,
                          vtkVolume* vol)
    {
    if (mapper->GetBlendMode() == vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND)
      {
      return std::string(
        "/// We get data between 0.0 - 1.0 range \n\
        float maxValue = 0.0;"
      );
      }
    else if (vol->GetProperty()->GetShade())
      {
      return std::string(
        "/// Light position in object space \n\
        l_light_pos_obj = (l_ogl_scene_matrix *  vec4(m_light_pos, 1.0)).xyz;"
      );
      }
    else
      {
      return std::string("");
      }
    }

  //--------------------------------------------------------------------------
  std::string IncrementShading(vtkRenderer* ren, vtkVolumeMapper* mapper,
                               vtkVolume* vol)
    {
    std::string shaderStr = "if (!l_skip) \n\
                               {\n";

    if (mapper->GetBlendMode() == vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND)
      {
      shaderStr += std::string(
      "float scalar = texture(m_volume, l_data_pos).r; \n\
       maxValue = max(maxValue, scalar);"
      );
      }
    else
      {
      shaderStr += std::string(
      "/// Data fetching from the red channel of m_volume texture \n\
      float scalar = texture(m_volume, l_data_pos).r * m_scale; \n\
      vec4 l_src_color = vec4(texture(m_color_transfer_func, scalar).xyz, \n\
                           texture(m_opacity_transfer_func, scalar).w);"
      );

      if (vol->GetProperty()->GetShade())
        {
        shaderStr += std::string
            ("if (l_src_color.a > 0.01) \
                { \n\
                vec3 g1; \n\
                vec3 g2; \n\
                vec3 ldir = normalize(l_light_pos_obj - m_vertex_pos); \n\
                vec3 vdir = normalize(l_eye_pos_obj - m_vertex_pos); \n\
                vec3 h = normalize(ldir + vdir); \n\
                vec3 xvec = vec3(m_step_size[0], 0.0, 0.0); \n\
                vec3 yvec = vec3(0.0, m_step_size[1], 0.0); \n\
                vec3 zvec = vec3(0.0, 0.0, m_step_size[2]); \n\
                g1.x = texture(m_volume, vec3(l_data_pos + xvec)).x; \n\
                g1.y = texture(m_volume, vec3(l_data_pos + yvec)).x; \n\
                g1.z = texture(m_volume, vec3(l_data_pos + zvec)).x; \n\
                g2.x = texture(m_volume, vec3(l_data_pos - xvec)).x; \n\
                g2.y = texture(m_volume, vec3(l_data_pos - yvec)).x; \n\
                g2.z = texture(m_volume, vec3(l_data_pos - zvec)).x; \n\
                g2 = normalize(g1 - g2); \n\
                float normalLength = length(g2); \n\
                if (normalLength > 0.0) \
                  { \
                  g2 = normalize(g2); \n\
                  } \
                else \
                  { \
                  g2 = vec3(0.0, 0.0, 0.0); \n\
                  } \
                vec3 final_color = vec3(0.0); \n\
                float n_dot_l = dot(g2, ldir); \n\
                float n_dot_h = dot(g2, h); \n\
                if (n_dot_l < 0.0) \
                  { \
                  n_dot_l =- n_dot_l; \n\
                  } \
                if (n_dot_h < 0.0) \
                  { \
                  n_dot_h =- n_dot_h; \n\
                  } \
                final_color += m_ambient; \n\
                final_color += m_diffuse * n_dot_l; \n\
                float m_shine_factor = pow(n_dot_h, m_shininess); \n\
                final_color += m_specular * m_shine_factor; \n\
                final_color = clamp(final_color, l_clamp_min, l_clamp_max); \n\
                l_src_color.rgb += final_color.rgb; \n\
               }"
            );
        }

      shaderStr += std::string(
      "/// Opacity calculation using compositing: \n\
      /// here we use front to back compositing scheme whereby the current sample \n\
      /// value is multiplied to the currently accumulated alpha and then this product \n\
      /// is subtracted from the sample value to get the alpha from the previous steps. \n\
      /// Next, this alpha is multiplied with the current sample colour and accumulated \n\
      /// to the composited colour. The alpha value from the previous steps is then \n\
      /// accumulated to the composited colour alpha. \n\
      l_src_color.rgb *= l_src_color.a; \n\
      m_frag_color = (1.0f - m_frag_color.a) * l_src_color + m_frag_color;"
      );
      }

    shaderStr += std::string("\n}");
    return shaderStr;
    }

  //--------------------------------------------------------------------------
  std::string ExitShading(vtkRenderer* vtkNotUsed(ren), vtkVolumeMapper* mapper,
                          vtkVolume* vtkNotUsed(vol))
    {
    if (mapper->GetBlendMode() == vtkVolumeMapper::MAXIMUM_INTENSITY_BLEND)
      {
      return std::string(
      "maxValue *= m_scale; \n\
       vec4 l_src_color = texture(m_color_transfer_func, maxValue); \n\
       l_src_color.a = texture(m_opacity_transfer_func, maxValue).w; \n\
        m_frag_color.rgb = l_src_color.rgb * l_src_color.a; \n\
        m_frag_color.a = l_src_color.a;"
      );
      }
    else
      {
      return std::string("");
      }
  }

  //--------------------------------------------------------------------------
  std::string InitTermination(vtkRenderer* ren, vtkVolumeMapper* mapper,
                              vtkVolume* vol)
    {
    return std::string(
    "/// Minimum texture access coordinate \n\
    const vec3 l_tex_min = vec3(0); \n\
    \n\
    /// Maximum texture access coordinate \n\
    const vec3 l_tex_max = vec3(1); \n\
    \n\
    const vec3 l_clamp_min = vec3(0.0); \n\
    const vec3 l_clamp_max = vec3(1.0); \n\
    \n\
    /// Flag to indicate if the raymarch loop should terminate \n\
    bool stop = false; \n\
    \n\
    /// 2D Texture fragment coordinates [0,1] from fragment coordinates \n\
    /// the frame buffer texture has the size of the plain buffer but \n\
    /// we use a fraction of it. The texture coordinates is less than 1 if \n\
    /// the reduction factor is less than 1. \n\
    /// Device coordinates are between -1 and 1. We need texture coordinates \n\
    /// between 0 and 1 the m_depth_sampler buffer has the original size buffer. \n\
    vec2 m_frag_tex_coord = (gl_FragCoord.xy - m_window_lower_left_corner) * \n\
                             m_inv_window_size; \n\
    vec4 l_depth_value = texture2D(m_depth_sampler, m_frag_tex_coord); \n\
    float m_terminate_point_max = 0.0; \n\
    \n\
    /// Depth test \n\
    if(gl_FragCoord.z >= l_depth_value.x) \n\
     { \n\
     discard; \n\
     } \n\
    \n\
    /// color buffer or max scalar buffer have a reduced size. \n\
    m_frag_tex_coord = (gl_FragCoord.xy - m_window_lower_left_corner) * \n\
                         m_inv_original_window_size; \n\
    \n\
    /// Compute max number of iterations it will take before we hit \n\
    /// the termination point \n\
    \n\
    /// Abscissa of the point on the depth buffer along the ray. \n\
    /// point in texture coordinates \n\
    vec4 m_terminate_point; \n\
    m_terminate_point.x = (gl_FragCoord.x - m_window_lower_left_corner.x) * 2.0 * \n\
                          m_inv_window_size.x - 1.0; \n\
    m_terminate_point.y = (gl_FragCoord.y - m_window_lower_left_corner.y) * 2.0 * \n\
                          m_inv_window_size.y - 1.0; \n\
    m_terminate_point.z = (2.0 * l_depth_value.x - (gl_DepthRange.near + \n\
                          gl_DepthRange.far)) / gl_DepthRange.diff; \n\
    m_terminate_point.w = 1.0; \n\
    \n\
    /// From normalized device coordinates to eye coordinates. m_projection_matrix \n\
    /// is inversed because of way VT \n\
    /// From eye coordinates to texture coordinates \n\
    m_terminate_point = inverse(transpose(m_texture_dataset_matrix)) * \n\
                        l_ogl_scene_matrix * inverse(transpose(m_modelview_matrix)) * \n\
                        inverse(transpose(m_projection_matrix)) * \n\
                        m_terminate_point; \n\
    m_terminate_point /= m_terminate_point.w; \n\
    \n\
    m_terminate_point_max = length(m_terminate_point.xyz - l_data_pos.xyz) / \n\
                            length(l_dir_step); \n\
    float m_current_t = 0.0;"
    );
    }

  //--------------------------------------------------------------------------
  std::string IncrementTermination(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                   vtkVolume* vol)
    {
    return std::string(
      "/// The two constants l_tex_min and l_tex_max have a value of vec3(-1,-1,-1) \n\
      /// and vec3(1,1,1) respectively. To determine if the data value is \n\
      /// outside the m_volume data, we use the sign function. The sign function \n\
      /// return -1 if the value is less than 0, 0 if the value is equal to 0 \n\
      /// and 1 if value is greater than 0. Hence, the sign function for the \n\
      /// calculation (sign(l_data_pos-l_tex_min) and sign (l_tex_max-l_data_pos)) will \n\
      /// give us vec3(1,1,1) at the possible minimum and maximum position. \n\
      /// When we do a dot product between two vec3(1,1,1) we get the answer 3. \n\
      /// So to be within the dataset limits, the dot product will return a \n\
      /// value less than 3. If it is greater than 3, we are already out of \n\
      /// the m_volume dataset \n\
      stop = dot(sign(l_data_pos - l_tex_min), sign(l_tex_max - l_data_pos)) < 3.0; \n\
      \n\
      /// If the stopping condition is true we brek out of the ray marching loop \n\
      if (stop) \n\
       { \n\
       break; \n\
       } \n\
      /// Early ray termination \n\
      /// if the currently composited colour alpha is already fully saturated \n\
      /// we terminated the loop or if we have hit an obstacle in the direction of \n\
      /// they ray (using depth buffer) we terminate as well. \n\
      if((m_frag_color.a > (1 - 1/255.0)) ||  \n\
          m_current_t >= m_terminate_point_max) \n\
        { \n\
        break; \n\
        } \n\
      ++m_current_t;"
    );
    }

  //--------------------------------------------------------------------------
  std::string ExitTermination(vtkRenderer* ren, vtkVolumeMapper* mapper,
                              vtkVolume* vol)
   {
    return std::string("");
   }

  //--------------------------------------------------------------------------
  std::string CroppingUniformsVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                      vtkVolume* vol)
  {
    return std::string("");
  }

  //--------------------------------------------------------------------------
  std::string CroppingAttributesVert(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                     vtkVolume* vol)
  {
    return std::string("");
  }

  //--------------------------------------------------------------------------
  std::string CroppingUniformsFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                      vtkVolume* vol)
  {
    return std::string("\n\
      uniform float cropping_planes[6]; \n\
      uniform int cropping_flags;");
  }

  //--------------------------------------------------------------------------
  std::string CroppingAttributesFrag(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                     vtkVolume* vol)
  {
    return std::string("");
  }

  //--------------------------------------------------------------------------
  std::string GlobalsCropping(vtkRenderer* ren, vtkVolumeMapper* mapper,
                              vtkVolume* vol)
  {
    return std::string("\n\
      /// X: axis = 0, Y: axis = 1, Z: axis = 2 \n\
      /// cp Cropping plane bounds (minX, maxX, minY, maxY, minZ, maxZ) \n\
      int computeRegionCoord(float cp[6], vec3 pos, int axis) \n\
      { \n\
        int cpmin = axis * 2; \n\
        int cpmax = cpmin + 1; \n\
        \n\
        if (pos[axis] > m_texture_extents_min[axis] && \n\
            pos[axis] < cp[cpmin]) \n\
          { \n\
          return 1; \n\
          } \n\
        else if (pos[axis] > cp[cpmin] && \n\
                 pos[axis]  < cp[cpmax]) \n\
          { \n\
          return 2; \n\
          } \n\
        else if (pos[axis] > cp[cpmax] && \n\
                 pos[axis] < m_texture_extents_max[axis]) \n\
          { \n\
          return 3; \n\
          } \n\
        return 0; \n\
      } \n\
      \n\
      int computeRegion(float cp[6], vec3 pos) \n\
      { \n\
        return (computeRegionCoord(cp, pos, 0) * \n\
                computeRegionCoord(cp, pos, 1) * \n\
                computeRegionCoord(cp, pos, 2)); \n\
      }");
  }

  //--------------------------------------------------------------------------
  std::string InitCropping(vtkRenderer* ren, vtkVolumeMapper* mapper,
                           vtkVolume* vol)
  {
    return std::string("\n\
      /// Convert cropping region to texture space \n\
      float cropping_planes_ts[6];\n\
      cropping_planes_ts[0] = cropping_planes[0] * m_step_size[0];\n\
      cropping_planes_ts[1] = cropping_planes[1] * m_step_size[0];\n\
      \n\
      cropping_planes_ts[2] = cropping_planes[2] * m_step_size[1];\n\
      cropping_planes_ts[3] = cropping_planes[3] * m_step_size[1];\n\
      \n\
      cropping_planes_ts[4] = cropping_planes[4] * m_step_size[2];\n\
      cropping_planes_ts[5] = cropping_planes[5] * m_step_size[2];");
  }

  //--------------------------------------------------------------------------
  std::string IncrementCropping(vtkRenderer* ren, vtkVolumeMapper* mapper,
                                vtkVolume* vol)
  {
    return std::string("\n\
      /// Determine region \n\
      int regionNo = computeRegion(cropping_planes_ts, l_data_pos); \n\
      regionNo = max(0, regionNo - 1); \n\
      \n\
      /// Do & operation with cropping flags \n\
      /// Pass the flag that its Ok to sample or not to do sample \n\
      if (((1 << regionNo) & cropping_flags) == 0) \n\
       { \n\
       /// Skip this voxel \n\
       l_skip = true; \n\
       }");
  }

  //--------------------------------------------------------------------------
  std::string ExitCropping(vtkRenderer* ren, vtkVolumeMapper* mapper,
                           vtkVolume* vol)
  {
    return std::string("");
  }
}


#endif // ___vtkVolumeShaderComposer_h
