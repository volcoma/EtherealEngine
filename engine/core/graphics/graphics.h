#pragma once

#include <bgfx/bgfx.h>
#include <bgfx/embedded_shader.h>
#include <bgfx/platform.h>
#include <bx/string.h>
#include <bx/allocator.h>
#include "format.h"
#include "vertex_decl.h"
#include <cstdint>
#include <functional>
#include <string>

namespace gfx
{
static const uint16_t invalid_handle = bgfx::kInvalidHandle;
using init_type = bgfx::Init;
using view_id = bgfx::ViewId;
using renderer_type = bgfx::RendererType::Enum;
using backbuffer_ratio = bgfx::BackbufferRatio::Enum;
using memory_view = bgfx::Memory;
using texture_info = bgfx::TextureInfo;
using uniform_type = bgfx::UniformType::Enum;
using uniform_info = bgfx::UniformInfo;
using attachment = bgfx::Attachment;
using platform_data = bgfx::PlatformData;
using caps = bgfx::Caps;
using stats = bgfx::Stats;
using access = bgfx::Access::Enum;
using view_mode = bgfx::ViewMode::Enum;
using occlusion_query_result = bgfx::OcclusionQueryResult::Enum;
using topology_conversion = bgfx::TopologyConvert::Enum;
using topology_sort = bgfx::TopologySort::Enum;
using topology = bgfx::Topology;
using release_fn = bgfx::ReleaseFn;
using encoder = bgfx::Encoder;

using transform = bgfx::Transform;
using dynamic_index_buffer_handle = bgfx::DynamicIndexBufferHandle;
using dynamic_vertex_buffer_handle = bgfx::DynamicVertexBufferHandle;
using frame_buffer_handle = bgfx::FrameBufferHandle;
using index_buffer_handle = bgfx::IndexBufferHandle;
using indirect_buffer_handle = bgfx::IndirectBufferHandle;
using occlusion_query_handle = bgfx::OcclusionQueryHandle;
using program_handle = bgfx::ProgramHandle;
using shader_handle = bgfx::ShaderHandle;
using texture_handle = bgfx::TextureHandle;
using uniform_handle = bgfx::UniformHandle;
using vertex_buffer_handle = bgfx::VertexBufferHandle;
using vertex_layout_handle = bgfx::VertexLayoutHandle;
using embedded_shader = bgfx::EmbeddedShader;
using transient_vertex_buffer = bgfx::TransientVertexBuffer;
using transient_index_buffer = bgfx::TransientIndexBuffer;
using instance_data_buffer = bgfx::InstanceDataBuffer;

void set_platform_data(const platform_data& _data);

/**/
bool init(init_type init_data);

/**/
void shutdown();

/**/
void reset(uint32_t _width, uint32_t _height, uint32_t _flags = BGFX_RESET_NONE);

/**/
void vertex_pack(const float _input[4], bool _inputNormalized, attribute _attr, const vertex_layout& _decl,
				 void* _data, uint32_t _index = 0);

/**/
void vertex_unpack(float _output[4], attribute _attr, const vertex_layout& _decl, const void* _data,
				   uint32_t _index = 0);

/**/
void vertex_convert(const vertex_layout& _destDecl, void* _destData, const vertex_layout& _srcDecl,
					const void* _srcData, uint32_t _num = 1);

/**/
uint16_t weld_vertices(uint16_t* _output, const vertex_layout& _decl, const void* _data, uint16_t _num,
					   float _epsilon = 0.001f);

/**/
uint32_t topology_convert(topology_conversion _conversion, void* _dst, uint32_t _dstSize,
						  const void* _indices, uint32_t _numIndices, bool _index32);

/**/
void topology_sort_tri_list(topology_sort _sort, void* _dst, uint32_t _dstSize, const float _dir[3],
							const float _pos[3], const void* _vertices, uint32_t _stride,
							const void* _indices, uint32_t _numIndices, bool _index32);

shader_handle create_embedded_shader(const embedded_shader* _es, renderer_type _type, const char* _name);

/**/
uint8_t get_supported_renderers(uint8_t _max = 0, renderer_type* _enum = nullptr);

/**/
const char* get_renderer_name(renderer_type _type);

/**/
encoder* begin();

/**/
void end(encoder* _encoder);

/**/
uint32_t frame(bool _capture = true);

/**/
renderer_type get_renderer_type();

/**/
const caps* get_caps();

/**/
const stats* get_stats();

/**/
const memory_view* alloc(uint32_t _size);

/**/
const memory_view* copy(const void* _data, uint32_t _size);

/**/
const memory_view* make_ref(const void* _data, uint32_t _size);

/**/
const memory_view* make_ref(const void* _data, uint32_t _size, release_fn _releaseFn = nullptr,
							void* _userData = nullptr);

/**/
void set_debug(uint32_t _debug);

/**/
void dbg_text_clear(uint8_t _attr = 0, bool _small = false);

/**/
void dbg_text_printf(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, ...);

/**/
void dbg_text_vprintf(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, va_list _argList);

/**/
void dbg_text_image(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const void* _data,
					uint16_t _pitch);

/**/
index_buffer_handle create_index_buffer(const memory_view* _mem, uint16_t _flags = BGFX_BUFFER_NONE);

/**/
void destroy(index_buffer_handle _handle);

/**/
vertex_buffer_handle create_vertex_buffer(const memory_view* _mem, const vertex_layout& _decl,
										  uint16_t _flags = BGFX_BUFFER_NONE);

/**/
void destroy(vertex_buffer_handle _handle);

/**/
dynamic_index_buffer_handle create_dynamic_index_buffer(uint32_t _num, uint16_t _flags = BGFX_BUFFER_NONE);

/**/
dynamic_index_buffer_handle create_dynamic_index_buffer(const memory_view* _mem,
														uint16_t _flags = BGFX_BUFFER_NONE);

/**/
void update(dynamic_index_buffer_handle _handle, uint32_t _startIndex,
								 const memory_view* _mem);

/**/
void destroy(dynamic_index_buffer_handle _handle);

/**/
dynamic_vertex_buffer_handle create_dynamic_vertex_buffer(uint32_t _num, const vertex_layout& _decl,
														  uint16_t _flags = BGFX_BUFFER_NONE);

/**/
dynamic_vertex_buffer_handle create_dynamic_vertex_buffer(const memory_view* _mem, const vertex_layout& _decl,
														  uint16_t _flags = BGFX_BUFFER_NONE);

/**/
void update(dynamic_vertex_buffer_handle _handle, uint32_t _startVertex,
								  const memory_view* _mem);

/**/
void destroy(dynamic_vertex_buffer_handle _handle);

/**/
uint32_t get_avail_transient_index_buffer(uint32_t _num);

/**/
uint32_t get_avail_transient_vertex_buffer(uint32_t _num, const vertex_layout& _decl);

/**/
uint32_t get_avail_instance_data_buffer(uint32_t _num, uint16_t _stride);

/**/
void alloc_transient_index_buffer(transient_index_buffer* _tib, uint32_t _num);

/**/
void alloc_transient_vertex_buffer(transient_vertex_buffer* _tvb, uint32_t _num, const vertex_layout& _decl);

/**/
bool alloc_transient_buffers(transient_vertex_buffer* _tvb, const vertex_layout& _decl, uint32_t _numVertices,
							 transient_index_buffer* _tib, uint32_t _numIndices);

/**/
void alloc_instance_data_buffer(instance_data_buffer* _idb, uint32_t _num, uint16_t _stride);

/**/
indirect_buffer_handle create_indirect_buffer(uint32_t _num);

/**/
void destroy(indirect_buffer_handle _handle);

/**/
shader_handle create_shader(const memory_view* _mem);

/**/
uint16_t get_shader_uniforms(shader_handle _handle, uniform_handle* _uniforms = nullptr, uint16_t _max = 0);

/**/
void get_uniform_info(uniform_handle _handle, uniform_info& _info);

/**/
void set_name(shader_handle _handle, const char* _name);

/**/
void destroy(shader_handle _handle);

/**/
program_handle create_program(shader_handle _vsh, shader_handle _fsh, bool _destroyShaders = false);

/**/
program_handle create_program(shader_handle _csh, bool _destroyShaders = false);

/**/
void destroy(program_handle _handle);

/**/
bool is_texture_valid(uint16_t _depth, bool _cubeMap, uint16_t _numLayers, texture_format _format,
					  uint64_t _flags);

/**/
void calc_texture_size(texture_info& _info, uint16_t _width, uint16_t _height, uint16_t _depth, bool _cubeMap,
					   bool _hasMips, uint16_t _numLayers, texture_format _format);

/**/
texture_handle create_texture(const memory_view* _mem, uint64_t _flags = BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE, uint8_t _skip = 0,
							  texture_info* _info = nullptr);

/**/
texture_handle create_texture_2d(uint16_t _width, uint16_t _height, bool _hasMips, uint16_t _numLayers,
								 texture_format _format, uint64_t _flags = BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE,
								 const memory_view* _mem = nullptr);

/**/
texture_handle create_texture_2d(backbuffer_ratio _ratio, bool _hasMips, uint16_t _numLayers,
								 texture_format _format, uint64_t _flags = BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE);

/**/
texture_handle create_texture_3d(uint16_t _width, uint16_t _height, uint16_t _depth, bool _hasMips,
								 texture_format _format, uint64_t _flags = BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE,
								 const memory_view* _mem = nullptr);

/**/
texture_handle create_texture_cube(uint16_t _size, bool _hasMips, uint16_t _numLayers, texture_format _format,
								   uint64_t _flags = BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE, const memory_view* _mem = nullptr);

/**/
void update_texture_2d(texture_handle _handle, uint16_t _layer, uint8_t _mip, uint16_t _x, uint16_t _y,
					   uint16_t _width, uint16_t _height, const memory_view* _mem,
					   uint16_t _pitch = UINT16_MAX);

/**/
void update_texture_3d(texture_handle _handle, uint8_t _mip, uint16_t _x, uint16_t _y, uint16_t _z,
					   uint16_t _width, uint16_t _height, uint16_t _depth, const memory_view* _mem);

/**/
void update_texture_cube(texture_handle _handle, uint16_t _layer, uint8_t _side, uint8_t _mip, uint16_t _x,
						 uint16_t _y, uint16_t _width, uint16_t _height, const memory_view* _mem,
						 uint16_t _pitch = UINT16_MAX);

/**/
uint32_t read_texture(texture_handle _handle, void* _data, uint8_t _mip = 0);

/**/
void set_name(texture_handle _handle, const char* _name);

/**/
void destroy(texture_handle _handle);

/**/
frame_buffer_handle create_frame_buffer(uint16_t _width, uint16_t _height, texture_format _format,
										uint64_t _textureFlags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

/**/
frame_buffer_handle create_frame_buffer(backbuffer_ratio _ratio, texture_format _format,
										uint64_t _textureFlags = BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP);

/**/
frame_buffer_handle create_frame_buffer(uint8_t _num, const texture_handle* _handles,
										bool _destroyTextures = false);

/**/
frame_buffer_handle create_frame_buffer(uint8_t _num, const attachment* _attachment,
										bool _destroyTextures = false);

/**/
frame_buffer_handle create_frame_buffer(void* _nwh, uint16_t _width, uint16_t _height,
                                        texture_format _format = texture_format::Count,
										texture_format _depthFormat = texture_format::Count);

/**/
texture_handle get_texture(frame_buffer_handle _handle, uint8_t _attachment = 0);

/**/
void destroy(frame_buffer_handle _handle);

/**/
uniform_handle create_uniform(const char* _name, uniform_type _type, uint16_t _num = 1);

/**/
void destroy(uniform_handle _handle);

/**/
occlusion_query_handle create_occlusion_query();

/**/
occlusion_query_result get_result(occlusion_query_handle _handle, int32_t* _result = nullptr);

/**/
void destroy(occlusion_query_handle _handle);

/**/
void set_palette_color(uint8_t _index, uint32_t _rgba);
void set_palette_color(uint8_t _index, const float _rgba[4]);
void set_palette_color(uint8_t _index, float _r, float _g, float _b, float _a);

/**/
void set_view_name(view_id _id, const char* _name);

/**/
void set_view_rect(view_id _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);

/**/
void set_view_rect(view_id _id, uint16_t _x, uint16_t _y, backbuffer_ratio _ratio);

/**/
void set_view_scissor(view_id _id, uint16_t _x = 0, uint16_t _y = 0, uint16_t _width = 0,
					  uint16_t _height = 0);

/**/
void set_view_clear(view_id _id, uint16_t _flags, uint32_t _rgba = 0x000000ff, float _depth = 1.0f,
					uint8_t _stencil = 0);

/**/
void set_view_clear(view_id _id, uint16_t _flags, float _depth, uint8_t _stencil, uint8_t _0 = UINT8_MAX,
					uint8_t _1 = UINT8_MAX, uint8_t _2 = UINT8_MAX, uint8_t _3 = UINT8_MAX,
					uint8_t _4 = UINT8_MAX, uint8_t _5 = UINT8_MAX, uint8_t _6 = UINT8_MAX,
					uint8_t _7 = UINT8_MAX);

/**/
void set_view_mode(view_id _id, view_mode _mode = view_mode::Default);

/**/
void set_view_frame_buffer(view_id _id, frame_buffer_handle _handle);

/**/
void set_view_transform(view_id _id, const void* _view, const void* _proj);

/**/
void set_view_order(view_id _id = 0, uint16_t _num = UINT16_MAX, const view_id* _order = nullptr);

/**/
void reset_view(view_id _id);

/**/
void set_marker(const char* _marker);

/**/
void set_state(uint64_t _state, uint32_t _rgba = 0);

/**/
void set_condition(occlusion_query_handle _handle, bool _visible);

/**/
void set_stencil(uint32_t _fstencil, uint32_t _bstencil = BGFX_STENCIL_NONE);

/**/
uint16_t set_scissor(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);

/**/
void set_scissord(uint16_t _cache = UINT16_MAX);

/**/
uint32_t set_transform(const void* _mtx, uint16_t _num = 1);

/**/
uint32_t alloc_transform(transform* _transform, uint16_t _num);

/**/
void set_transform(uint32_t _cache, uint16_t _num = 1);

/**/
void set_uniform(uniform_handle _handle, const void* _value, uint16_t _num = 1);

/**/
void set_index_buffer(index_buffer_handle _handle, uint32_t _firstIndex, uint32_t _numIndices);

/**/
void set_index_buffer(dynamic_index_buffer_handle _handle, uint32_t _firstIndex, uint32_t _numIndices);

/**/
void set_index_buffer(const transient_index_buffer* _tib, uint32_t _firstIndex, uint32_t _numIndices);

/**/
void set_vertex_buffer(uint8_t _stream, vertex_buffer_handle _handle, uint32_t _startVertex,
					   uint32_t _numVertices);

/**/
void set_vertex_buffer(uint8_t _stream, dynamic_vertex_buffer_handle _handle, uint32_t _startVertex,
					   uint32_t _numVertices);

/**/
void set_vertex_buffer(uint8_t _stream, const transient_vertex_buffer* _tvb, uint32_t _startVertex,
					   uint32_t _numVertices);

/**/
void set_instance_data_buffer(const instance_data_buffer* _idb, uint32_t _start, uint32_t _num);

/**/
void set_instance_data_buffer(vertex_buffer_handle _handle, uint32_t _startVertex, uint32_t _num);

/**/
void set_instance_data_buffer(dynamic_vertex_buffer_handle _handle, uint32_t _startVertex, uint32_t _num);

/**/
void set_texture(uint8_t _stage, uniform_handle _sampler, texture_handle _handle,
				 uint32_t _flags = UINT32_MAX);

/**/
void touch(view_id _id);

/**/
void submit(view_id _id, program_handle _handle, int32_t _depth = 0, bool _preserveState = false);

/**/
void submit(view_id _id, program_handle _program, occlusion_query_handle _occlusionQuery, int32_t _depth = 0,
			bool _preserveState = false);

/**/
void submit(view_id _id, program_handle _handle, indirect_buffer_handle _indirectHandle, uint16_t _start = 0,
			uint16_t _num = 1, int32_t _depth = 0, bool _preserveState = false);

/**/
void set_image(uint8_t _stage, texture_handle _handle, uint8_t _mip, access _access,
			   texture_format _format = texture_format::Count);

/**/
void set_buffer(uint8_t _stage, index_buffer_handle _handle, access _access);

/**/
void set_buffer(uint8_t _stage, vertex_buffer_handle _handle, access _access);

/**/
void set_buffer(uint8_t _stage, dynamic_index_buffer_handle _handle, access _access);

/**/
void set_buffer(uint8_t _stage, dynamic_vertex_buffer_handle _handle, access _access);

/**/
void set_buffer(uint8_t _stage, indirect_buffer_handle _handle, access _access);

/**/
void dispatch(view_id _id, program_handle _handle, uint32_t _numX = 1, uint32_t _numY = 1, uint32_t _numZ = 1);

/**/
void dispatch_indirect(view_id _id, program_handle _handle, indirect_buffer_handle _indirectHandle,
					   uint16_t _start = 0, uint16_t _num = 1);

/**/
void discard();

/**/
void blit(view_id _id, texture_handle _dst, uint16_t _dstX, uint16_t _dstY, texture_handle _src,
		  uint16_t _srcX = 0, uint16_t _srcY = 0, uint16_t _width = UINT16_MAX,
		  uint16_t _height = UINT16_MAX);
void blit(view_id _id, texture_handle _dst, uint8_t _dstMip, uint16_t _dstX, uint16_t _dstY, uint16_t _dstZ,
		  texture_handle _src, uint8_t _srcMip = 0, uint16_t _srcX = 0, uint16_t _srcY = 0,
		  uint16_t _srcZ = 0, uint16_t _width = UINT16_MAX, uint16_t _height = UINT16_MAX,
		  uint16_t _depth = UINT16_MAX);

/**/
void request_screen_shot(frame_buffer_handle _handle, const char* _filePath);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void set_trace_logger(const std::function<void(const std::string&)>& logger);
void set_info_logger(const std::function<void(const std::string&)>& logger);
void set_warning_logger(const std::function<void(const std::string&)>& logger);
void set_error_logger(const std::function<void(const std::string&)>& logger);

void flush();

bool is_origin_bottom_left();
bool is_homogeneous_depth();
float get_half_texel();
std::uint32_t get_max_blend_transforms();

std::uint64_t screen_quad(float dest_width, float dest_height, float depth = 0.0f, float width = 1.0f,
						  float height = 1.0f);
std::uint64_t clip_quad(float depth = 0.0f, float width = 1.0f, float height = 1.0f);

void get_size_from_ratio(backbuffer_ratio _ratio, std::uint16_t& _width, std::uint16_t& _height);

const std::string& get_renderer_filename_extension();
bool is_supported(uint64_t flag);
}
