#version 330 core

//////////////////////////////////////////////////////////////////////////////
///
/// Inputs
///
//////////////////////////////////////////////////////////////////////////////

/// 3D texture coordinates form vertex shader
in vec3 m_texture_coords;
in vec3 m_vertex_pos;

//////////////////////////////////////////////////////////////////////////////
///
/// Outputs
///
//////////////////////////////////////////////////////////////////////////////

/// Fragment shader output
layout(location = 0) out vec4 m_frag_color;

//////////////////////////////////////////////////////////////////////////////
///
/// Attributes
///
//////////////////////////////////////////////////////////////////////////////

@BASE_ATTRIBUTES_FRAG@
@TERMINATION_ATTRIBUTES_FRAG@
@SHADING_ATTRIBUTES_FRAG@

//////////////////////////////////////////////////////////////////////////////
///
/// Uniforms
///
//////////////////////////////////////////////////////////////////////////////

@BASE_UNIFORMS_FRAG@
@TERMINATION_UNIFORMS_FRAG@
@SHADING_UNIFORMS_FRAG@

//////////////////////////////////////////////////////////////////////////////
///
/// Main
///
//////////////////////////////////////////////////////////////////////////////
void main()
{
  /// Initialize m_frag_color (output) to 0
  m_frag_color = vec4(0.0);

  @BASE_INIT@
  @TERMINATE_INIT@
  @SHADING_INIT@

  /// For all samples along the ray
  while (true)
    {
    @BASE_LOOP@
    @TERMINATE_LOOP@
    @SHADING_LOOP@

    /// Advance ray by m_dir_step
    l_data_pos += m_dir_step;
    };


  @BASE_EXIT@
  @TERMINATE_EXIT@
  @SHADING_EXIT@
}
