/*

VTK_FREETYPE_CHANGE this file is new for VTK.

This header file mangles all symbols exported from the freetype library.
It is included in all files while building the freetype library.  Due to
namespace pollution, no freetype headers should be included in .h files in
VTK.

To generate this list: wrap everything below in #if 0, build VTK, then
use the following command to obtain the symbol list:

nm libvtkfreetype.a |grep " [TRD] " | awk '{ print "#define "$3" vtk_freetype_"$3 }'


In fact, you must do this on *all* supported platforms and merge the results,
since some symbols are present only on some platforms.

Then alphabetise the list so that future diffs/merges work better.

Note that _fini and _init should be excluded because they are not functions
implemented by the library but are rather created by the linker and
used when the shared library is loaded/unloaded from an executable.

*/

#ifndef vtk_freetype_mangle_h
#define vtk_freetype_mangle_h

#define afm_parser_funcs vtk_freetype_afm_parser_funcs
#define bdf_cmap_class vtk_freetype_bdf_cmap_class
#define bdf_driver_class vtk_freetype_bdf_driver_class
#define cff_cmap_encoding_class_rec vtk_freetype_cff_cmap_encoding_class_rec
#define cff_cmap_unicode_class_rec vtk_freetype_cff_cmap_unicode_class_rec
#define cff_driver_class vtk_freetype_cff_driver_class
#define FT_Activate_Size vtk_freetype_FT_Activate_Size
#define FT_Add_Default_Modules vtk_freetype_FT_Add_Default_Modules
#define FT_Add_Module vtk_freetype_FT_Add_Module
#define FT_Angle_Diff vtk_freetype_FT_Angle_Diff
#define FT_Atan2 vtk_freetype_FT_Atan2
#define FT_Attach_File vtk_freetype_FT_Attach_File
#define FT_Attach_Stream vtk_freetype_FT_Attach_Stream
#define FT_Bitmap_Convert vtk_freetype_FT_Bitmap_Convert
#define FT_Bitmap_Copy vtk_freetype_FT_Bitmap_Copy
#define FT_Bitmap_Done vtk_freetype_FT_Bitmap_Done
#define FT_Bitmap_Embolden vtk_freetype_FT_Bitmap_Embolden
#define ft_bitmap_glyph_class vtk_freetype_ft_bitmap_glyph_class
#define FT_Bitmap_New vtk_freetype_FT_Bitmap_New
#define FT_CeilFix vtk_freetype_FT_CeilFix
#define FT_CMap_Done vtk_freetype_FT_CMap_Done
#define FT_CMap_New vtk_freetype_FT_CMap_New
#define ft_corner_is_flat vtk_freetype_ft_corner_is_flat
#define ft_corner_orientation vtk_freetype_ft_corner_orientation
#define FT_Cos vtk_freetype_FT_Cos
#define ft_debug_init vtk_freetype_ft_debug_init
#define FT_DivFix vtk_freetype_FT_DivFix
#define FT_Done_Face vtk_freetype_FT_Done_Face
#define FT_Done_FreeType vtk_freetype_FT_Done_FreeType
#define FT_Done_Glyph vtk_freetype_FT_Done_Glyph
#define FT_Done_GlyphSlot vtk_freetype_FT_Done_GlyphSlot
#define FT_Done_Library vtk_freetype_FT_Done_Library
#define FT_Done_Memory vtk_freetype_FT_Done_Memory
#define FT_Done_Size vtk_freetype_FT_Done_Size
#define FT_Face_GetCharsOfVariant vtk_freetype_FT_Face_GetCharsOfVariant
#define FT_Face_GetCharVariantIndex vtk_freetype_FT_Face_GetCharVariantIndex
#define FT_Face_GetCharVariantIsDefault vtk_freetype_FT_Face_GetCharVariantIsDefault
#define FT_Face_GetVariantSelectors vtk_freetype_FT_Face_GetVariantSelectors
#define FT_Face_GetVariantsOfChar vtk_freetype_FT_Face_GetVariantsOfChar
#define FT_FloorFix vtk_freetype_FT_FloorFix
#define FT_Get_Advance vtk_freetype_FT_Get_Advance
#define FT_Get_Advances vtk_freetype_FT_Get_Advances
#define FT_Get_Char_Index vtk_freetype_FT_Get_Char_Index
#define FT_Get_Charmap_Index vtk_freetype_FT_Get_Charmap_Index
#define FT_Get_CMap_Format vtk_freetype_FT_Get_CMap_Format
#define FT_Get_CMap_Language_ID vtk_freetype_FT_Get_CMap_Language_ID
#define FT_Get_First_Char vtk_freetype_FT_Get_First_Char
#define FT_Get_Glyph vtk_freetype_FT_Get_Glyph
#define FT_Get_Glyph_Name vtk_freetype_FT_Get_Glyph_Name
#define FT_Get_Kerning vtk_freetype_FT_Get_Kerning
#define FT_Get_MM_Var vtk_freetype_FT_Get_MM_Var
#define FT_Get_Module vtk_freetype_FT_Get_Module
#define FT_Get_Module_Interface vtk_freetype_FT_Get_Module_Interface
#define FT_Get_Multi_Master vtk_freetype_FT_Get_Multi_Master
#define FT_Get_Name_Index vtk_freetype_FT_Get_Name_Index
#define FT_Get_Next_Char vtk_freetype_FT_Get_Next_Char
#define FT_Get_Postscript_Name vtk_freetype_FT_Get_Postscript_Name
#define FT_Get_Renderer vtk_freetype_FT_Get_Renderer
#define FT_Get_Sfnt_Name vtk_freetype_FT_Get_Sfnt_Name
#define FT_Get_Sfnt_Name_Count vtk_freetype_FT_Get_Sfnt_Name_Count
#define FT_Get_Sfnt_Table vtk_freetype_FT_Get_Sfnt_Table
#define FT_Get_SubGlyph_Info vtk_freetype_FT_Get_SubGlyph_Info
#define FT_Get_Track_Kerning vtk_freetype_FT_Get_Track_Kerning
#define FT_Get_TrueType_Engine_Type vtk_freetype_FT_Get_TrueType_Engine_Type
#define FT_GetFile_From_Mac_ATS_Name vtk_freetype_FT_GetFile_From_Mac_ATS_Name
#define FT_GetFile_From_Mac_Name vtk_freetype_FT_GetFile_From_Mac_Name
#define FT_GetFilePath_From_Mac_ATS_Name vtk_freetype_FT_GetFilePath_From_Mac_ATS_Name
#define FT_Glyph_Copy vtk_freetype_FT_Glyph_Copy
#define FT_Glyph_Get_CBox vtk_freetype_FT_Glyph_Get_CBox
#define FT_Glyph_To_Bitmap vtk_freetype_FT_Glyph_To_Bitmap
#define FT_Glyph_Transform vtk_freetype_FT_Glyph_Transform
#define FT_GlyphLoader_Add vtk_freetype_FT_GlyphLoader_Add
#define FT_GlyphLoader_CheckPoints vtk_freetype_FT_GlyphLoader_CheckPoints
#define FT_GlyphLoader_CheckSubGlyphs vtk_freetype_FT_GlyphLoader_CheckSubGlyphs
#define FT_GlyphLoader_CopyPoints vtk_freetype_FT_GlyphLoader_CopyPoints
#define FT_GlyphLoader_CreateExtra vtk_freetype_FT_GlyphLoader_CreateExtra
#define FT_GlyphLoader_Done vtk_freetype_FT_GlyphLoader_Done
#define FT_GlyphLoader_New vtk_freetype_FT_GlyphLoader_New
#define FT_GlyphLoader_Prepare vtk_freetype_FT_GlyphLoader_Prepare
#define FT_GlyphLoader_Reset vtk_freetype_FT_GlyphLoader_Reset
#define FT_GlyphLoader_Rewind vtk_freetype_FT_GlyphLoader_Rewind
#define ft_glyphslot_alloc_bitmap vtk_freetype_ft_glyphslot_alloc_bitmap
#define ft_glyphslot_free_bitmap vtk_freetype_ft_glyphslot_free_bitmap
#define FT_GlyphSlot_Own_Bitmap vtk_freetype_FT_GlyphSlot_Own_Bitmap
#define ft_glyphslot_set_bitmap vtk_freetype_ft_glyphslot_set_bitmap
#define ft_grays_raster vtk_freetype_ft_grays_raster
#define ft_highpow2 vtk_freetype_ft_highpow2
#define FT_Init_FreeType vtk_freetype_FT_Init_FreeType
#define FT_Library_Version vtk_freetype_FT_Library_Version
#define FT_List_Add vtk_freetype_FT_List_Add
#define FT_List_Finalize vtk_freetype_FT_List_Finalize
#define FT_List_Find vtk_freetype_FT_List_Find
#define FT_List_Insert vtk_freetype_FT_List_Insert
#define FT_List_Iterate vtk_freetype_FT_List_Iterate
#define FT_List_Remove vtk_freetype_FT_List_Remove
#define FT_List_Up vtk_freetype_FT_List_Up
#define FT_Load_Char vtk_freetype_FT_Load_Char
#define FT_Load_Glyph vtk_freetype_FT_Load_Glyph
#define FT_Load_Sfnt_Table vtk_freetype_FT_Load_Sfnt_Table
#define FT_Lookup_Renderer vtk_freetype_FT_Lookup_Renderer
#define FT_Match_Size vtk_freetype_FT_Match_Size
#define FT_Matrix_Invert vtk_freetype_FT_Matrix_Invert
#define FT_Matrix_Multiply vtk_freetype_FT_Matrix_Multiply
#define FT_Matrix_Multiply_Scaled vtk_freetype_FT_Matrix_Multiply_Scaled
#define ft_mem_alloc vtk_freetype_ft_mem_alloc
#define ft_mem_dup vtk_freetype_ft_mem_dup
#define ft_mem_free vtk_freetype_ft_mem_free
#define ft_mem_qalloc vtk_freetype_ft_mem_qalloc
#define ft_mem_qrealloc vtk_freetype_ft_mem_qrealloc
#define ft_mem_realloc vtk_freetype_ft_mem_realloc
#define ft_mem_strcpyn vtk_freetype_ft_mem_strcpyn
#define ft_mem_strdup vtk_freetype_ft_mem_strdup
#define ft_module_get_service vtk_freetype_ft_module_get_service
#define FT_MulDiv vtk_freetype_FT_MulDiv
#define FT_MulDiv_No_Round vtk_freetype_FT_MulDiv_No_Round
#define FT_MulFix vtk_freetype_FT_MulFix
#define FT_New_Face vtk_freetype_FT_New_Face
#define FT_New_Face_From_FOND vtk_freetype_FT_New_Face_From_FOND
#define FT_New_Face_From_FSRef vtk_freetype_FT_New_Face_From_FSRef
#define FT_New_Face_From_FSSpec vtk_freetype_FT_New_Face_From_FSSpec
#define FT_New_GlyphSlot vtk_freetype_FT_New_GlyphSlot
#define FT_New_Library vtk_freetype_FT_New_Library
#define FT_New_Memory vtk_freetype_FT_New_Memory
#define FT_New_Memory_Face vtk_freetype_FT_New_Memory_Face
#define FT_New_Size vtk_freetype_FT_New_Size
#define FT_Open_Face vtk_freetype_FT_Open_Face
#define FT_Outline_Check vtk_freetype_FT_Outline_Check
#define FT_Outline_Copy vtk_freetype_FT_Outline_Copy
#define FT_Outline_Decompose vtk_freetype_FT_Outline_Decompose
#define FT_Outline_Done vtk_freetype_FT_Outline_Done
#define FT_Outline_Done_Internal vtk_freetype_FT_Outline_Done_Internal
#define FT_Outline_Embolden vtk_freetype_FT_Outline_Embolden
#define FT_Outline_Get_BBox vtk_freetype_FT_Outline_Get_BBox
#define FT_Outline_Get_Bitmap vtk_freetype_FT_Outline_Get_Bitmap
#define FT_Outline_Get_CBox vtk_freetype_FT_Outline_Get_CBox
#define FT_Outline_Get_Orientation vtk_freetype_FT_Outline_Get_Orientation
#define ft_outline_glyph_class vtk_freetype_ft_outline_glyph_class
#define FT_Outline_New vtk_freetype_FT_Outline_New
#define FT_Outline_New_Internal vtk_freetype_FT_Outline_New_Internal
#define FT_Outline_Render vtk_freetype_FT_Outline_Render
#define FT_Outline_Reverse vtk_freetype_FT_Outline_Reverse
#define FT_Outline_Transform vtk_freetype_FT_Outline_Transform
#define FT_Outline_Translate vtk_freetype_FT_Outline_Translate
#define FT_Raccess_Get_DataOffsets vtk_freetype_FT_Raccess_Get_DataOffsets
#define FT_Raccess_Get_HeaderInfo vtk_freetype_FT_Raccess_Get_HeaderInfo
#define FT_Raccess_Guess vtk_freetype_FT_Raccess_Guess
#define ft_raster1_renderer_class vtk_freetype_ft_raster1_renderer_class
#define ft_raster5_renderer_class vtk_freetype_ft_raster5_renderer_class
#define FT_Reference_Face vtk_freetype_FT_Reference_Face
#define FT_Reference_Library vtk_freetype_FT_Reference_Library
#define FT_Remove_Module vtk_freetype_FT_Remove_Module
#define FT_Render_Glyph vtk_freetype_FT_Render_Glyph
#define FT_Render_Glyph_Internal vtk_freetype_FT_Render_Glyph_Internal
#define FT_Request_Metrics vtk_freetype_FT_Request_Metrics
#define FT_Request_Size vtk_freetype_FT_Request_Size
#define FT_RoundFix vtk_freetype_FT_RoundFix
#define FT_Select_Charmap vtk_freetype_FT_Select_Charmap
#define FT_Select_Metrics vtk_freetype_FT_Select_Metrics
#define FT_Select_Size vtk_freetype_FT_Select_Size
#define ft_service_list_lookup vtk_freetype_ft_service_list_lookup
#define FT_Set_Char_Size vtk_freetype_FT_Set_Char_Size
#define FT_Set_Charmap vtk_freetype_FT_Set_Charmap
#define FT_Set_Debug_Hook vtk_freetype_FT_Set_Debug_Hook
#define FT_Set_MM_Blend_Coordinates vtk_freetype_FT_Set_MM_Blend_Coordinates
#define FT_Set_MM_Design_Coordinates vtk_freetype_FT_Set_MM_Design_Coordinates
#define FT_Set_Pixel_Sizes vtk_freetype_FT_Set_Pixel_Sizes
#define FT_Set_Renderer vtk_freetype_FT_Set_Renderer
#define FT_Set_Transform vtk_freetype_FT_Set_Transform
#define FT_Set_Var_Blend_Coordinates vtk_freetype_FT_Set_Var_Blend_Coordinates
#define FT_Set_Var_Design_Coordinates vtk_freetype_FT_Set_Var_Design_Coordinates
#define FT_Sfnt_Table_Info vtk_freetype_FT_Sfnt_Table_Info
#define FT_Sin vtk_freetype_FT_Sin
#define ft_smooth_lcd_renderer_class vtk_freetype_ft_smooth_lcd_renderer_class
#define ft_smooth_lcdv_renderer_class vtk_freetype_ft_smooth_lcdv_renderer_class
#define ft_smooth_renderer_class vtk_freetype_ft_smooth_renderer_class
#define FT_SqrtFixed vtk_freetype_FT_SqrtFixed
#define ft_standard_raster vtk_freetype_ft_standard_raster
#define FT_Stream_Close vtk_freetype_FT_Stream_Close
#define FT_Stream_EnterFrame vtk_freetype_FT_Stream_EnterFrame
#define FT_Stream_ExitFrame vtk_freetype_FT_Stream_ExitFrame
#define FT_Stream_ExtractFrame vtk_freetype_FT_Stream_ExtractFrame
#define FT_Stream_Free vtk_freetype_FT_Stream_Free
#define FT_Stream_GetChar vtk_freetype_FT_Stream_GetChar
#define FT_Stream_GetULong vtk_freetype_FT_Stream_GetULong
#define FT_Stream_GetULongLE vtk_freetype_FT_Stream_GetULongLE
#define FT_Stream_GetUOffset vtk_freetype_FT_Stream_GetUOffset
#define FT_Stream_GetUShort vtk_freetype_FT_Stream_GetUShort
#define FT_Stream_GetUShortLE vtk_freetype_FT_Stream_GetUShortLE
#define FT_Stream_New vtk_freetype_FT_Stream_New
#define FT_Stream_Open vtk_freetype_FT_Stream_Open
#define FT_Stream_OpenLZW vtk_freetype_FT_Stream_OpenLZW
#define FT_Stream_OpenMemory vtk_freetype_FT_Stream_OpenMemory
#define FT_Stream_Pos vtk_freetype_FT_Stream_Pos
#define FT_Stream_Read vtk_freetype_FT_Stream_Read
#define FT_Stream_ReadAt vtk_freetype_FT_Stream_ReadAt
#define FT_Stream_ReadChar vtk_freetype_FT_Stream_ReadChar
#define FT_Stream_ReadFields vtk_freetype_FT_Stream_ReadFields
#define FT_Stream_ReadULong vtk_freetype_FT_Stream_ReadULong
#define FT_Stream_ReadULongLE vtk_freetype_FT_Stream_ReadULongLE
#define FT_Stream_ReadUOffset vtk_freetype_FT_Stream_ReadUOffset
#define FT_Stream_ReadUShort vtk_freetype_FT_Stream_ReadUShort
#define FT_Stream_ReadUShortLE vtk_freetype_FT_Stream_ReadUShortLE
#define FT_Stream_ReleaseFrame vtk_freetype_FT_Stream_ReleaseFrame
#define FT_Stream_Seek vtk_freetype_FT_Stream_Seek
#define FT_Stream_Skip vtk_freetype_FT_Stream_Skip
#define FT_Stream_TryRead vtk_freetype_FT_Stream_TryRead
#define ft_synthesize_vertical_metrics vtk_freetype_ft_synthesize_vertical_metrics
#define FT_Tan vtk_freetype_FT_Tan
#define FT_Trace_Get_Count vtk_freetype_FT_Trace_Get_Count
#define FT_Trace_Get_Name vtk_freetype_FT_Trace_Get_Name
#define ft_validator_error vtk_freetype_ft_validator_error
#define ft_validator_init vtk_freetype_ft_validator_init
#define ft_validator_run vtk_freetype_ft_validator_run
#define FT_Vector_From_Polar vtk_freetype_FT_Vector_From_Polar
#define FT_Vector_Length vtk_freetype_FT_Vector_Length
#define FT_Vector_Polarize vtk_freetype_FT_Vector_Polarize
#define FT_Vector_Rotate vtk_freetype_FT_Vector_Rotate
#define FT_Vector_Transform vtk_freetype_FT_Vector_Transform
#define FT_Vector_Transform_Scaled vtk_freetype_FT_Vector_Transform_Scaled
#define FT_Vector_Unit vtk_freetype_FT_Vector_Unit
#define ftc_basic_image_cache_class vtk_freetype_ftc_basic_image_cache_class
#define ftc_basic_image_family_class vtk_freetype_ftc_basic_image_family_class
#define ftc_basic_sbit_cache_class vtk_freetype_ftc_basic_sbit_cache_class
#define ftc_basic_sbit_family_class vtk_freetype_ftc_basic_sbit_family_class
#define ftc_cmap_cache_class vtk_freetype_ftc_cmap_cache_class
#define FTC_CMapCache_Lookup vtk_freetype_FTC_CMapCache_Lookup
#define FTC_CMapCache_New vtk_freetype_FTC_CMapCache_New
#define ftc_face_list_class vtk_freetype_ftc_face_list_class
#define FTC_ImageCache_Lookup vtk_freetype_FTC_ImageCache_Lookup
#define FTC_ImageCache_LookupScaler vtk_freetype_FTC_ImageCache_LookupScaler
#define FTC_ImageCache_New vtk_freetype_FTC_ImageCache_New
#define FTC_Manager_Done vtk_freetype_FTC_Manager_Done
#define FTC_Manager_LookupFace vtk_freetype_FTC_Manager_LookupFace
#define FTC_Manager_LookupSize vtk_freetype_FTC_Manager_LookupSize
#define FTC_Manager_New vtk_freetype_FTC_Manager_New
#define FTC_Manager_RemoveFaceID vtk_freetype_FTC_Manager_RemoveFaceID
#define FTC_Manager_Reset vtk_freetype_FTC_Manager_Reset
#define FTC_Node_Unref vtk_freetype_FTC_Node_Unref
#define FTC_SBitCache_Lookup vtk_freetype_FTC_SBitCache_Lookup
#define FTC_SBitCache_LookupScaler vtk_freetype_FTC_SBitCache_LookupScaler
#define FTC_SBitCache_New vtk_freetype_FTC_SBitCache_New
#define ftc_size_list_class vtk_freetype_ftc_size_list_class
#define pcf_cmap_class vtk_freetype_pcf_cmap_class
#define pcf_driver_class vtk_freetype_pcf_driver_class
#define pfr_cmap_class_rec vtk_freetype_pfr_cmap_class_rec
#define pfr_driver_class vtk_freetype_pfr_driver_class
#define pfr_metrics_service_rec vtk_freetype_pfr_metrics_service_rec
#define ps_hints_apply vtk_freetype_ps_hints_apply
#define ps_parser_funcs vtk_freetype_ps_parser_funcs
#define ps_table_funcs vtk_freetype_ps_table_funcs
#define psaux_module_class vtk_freetype_psaux_module_class
#define pshinter_module_class vtk_freetype_pshinter_module_class
#define psnames_module_class vtk_freetype_psnames_module_class
#define sfnt_module_class vtk_freetype_sfnt_module_class
#define t1_builder_funcs vtk_freetype_t1_builder_funcs
#define t1_cmap_classes vtk_freetype_t1_cmap_classes
#define t1_cmap_custom_class_rec vtk_freetype_t1_cmap_custom_class_rec
#define t1_cmap_expert_class_rec vtk_freetype_t1_cmap_expert_class_rec
#define t1_cmap_standard_class_rec vtk_freetype_t1_cmap_standard_class_rec
#define t1_cmap_unicode_class_rec vtk_freetype_t1_cmap_unicode_class_rec
#define t1_decoder_funcs vtk_freetype_t1_decoder_funcs
#define t1_driver_class vtk_freetype_t1_driver_class
#define t1cid_driver_class vtk_freetype_t1cid_driver_class
#define t42_driver_class vtk_freetype_t42_driver_class
#define tt_cmap0_class_rec vtk_freetype_tt_cmap0_class_rec
#define tt_cmap10_class_rec vtk_freetype_tt_cmap10_class_rec
#define tt_cmap12_class_rec vtk_freetype_tt_cmap12_class_rec
#define tt_cmap13_class_rec vtk_freetype_tt_cmap13_class_rec
#define tt_cmap14_class_rec vtk_freetype_tt_cmap14_class_rec
#define tt_cmap2_class_rec vtk_freetype_tt_cmap2_class_rec
#define tt_cmap4_class_rec vtk_freetype_tt_cmap4_class_rec
#define tt_cmap6_class_rec vtk_freetype_tt_cmap6_class_rec
#define tt_cmap8_class_rec vtk_freetype_tt_cmap8_class_rec
#define tt_default_graphics_state vtk_freetype_tt_default_graphics_state
#define tt_driver_class vtk_freetype_tt_driver_class
#define TT_New_Context vtk_freetype_TT_New_Context
#define TT_RunIns vtk_freetype_TT_RunIns
#define winfnt_driver_class vtk_freetype_winfnt_driver_class
#define FT_Stream_OpenGzip vtk_freetype_FT_Stream_OpenGzip

#endif
