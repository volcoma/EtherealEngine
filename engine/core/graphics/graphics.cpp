#include "graphics.h"
#include <algorithm>
#include <map>
namespace gfx
{
static std::map<std::string, std::function<void(const std::string& log_msg)>> s_loggers;
static bool s_initted = false;

void set_info_logger(std::function<void(const std::string& log_msg)> logger)
{
	s_loggers["info"] = logger;
}
static std::function<void(const std::string& log_msg)> warning_logger;
void set_warning_logger(std::function<void(const std::string& log_msg)> logger)
{
	s_loggers["warning"] = logger;
}
static std::function<void(const std::string& log_msg)> error_logger;
void set_error_logger(std::function<void(const std::string& log_msg)> logger)
{
	s_loggers["error"] = logger;
}
void log(const std::string& category, const std::string& log_msg)
{
	if(s_loggers[category])
		s_loggers[category](log_msg);
}

struct gfx_callback : public bgfx::CallbackI
{
	virtual ~gfx_callback()
	{
	}

	virtual void traceVargs(const char* /*_filePath*/, std::uint16_t /*_line*/, const char* /*_format*/,
							va_list /*_argList*/) final
	{
	}

	virtual void profilerBegin(const char* /*_name*/, std::uint32_t /*_abgr*/, const char* /*_filePath*/,
							   std::uint16_t /*_line*/) final
	{
	}

	virtual void profilerBeginLiteral(const char* /*_name*/, std::uint32_t /*_abgr*/,
									  const char* /*_filePath*/, std::uint16_t /*_line*/) final
	{
	}

	virtual void profilerEnd() final
	{
	}
	virtual void fatal(bgfx::Fatal::Enum /*_code*/, const char* _str) final
	{
		log("error", _str);
	}

	virtual std::uint32_t cacheReadSize(std::uint64_t /*_id*/) final
	{
		return 0;
	}

	virtual bool cacheRead(std::uint64_t /*_id*/, void* /*_data*/, std::uint32_t /*_size*/) final
	{
		return false;
	}

	virtual void cacheWrite(std::uint64_t /*_id*/, const void* /*_data*/, std::uint32_t /*_size*/) final
	{
	}

	virtual void screenShot(const char* /*_filePath*/, std::uint32_t /*_width*/, std::uint32_t /*_height*/,
							std::uint32_t /*_pitch*/, const void* /*_data*/, std::uint32_t /*_size*/,
							bool /*_yflip*/) final
	{
	}

	virtual void captureBegin(std::uint32_t /*_width*/, std::uint32_t /*_height*/, std::uint32_t /*_pitch*/,
							  texture_format /*_format*/, bool /*_yflip*/) final
	{
	}

	virtual void captureEnd() final
	{
	}

	virtual void captureFrame(const void* /*_data*/, std::uint32_t /*_size*/) final
	{
	}
};

void set_platform_data(const platform_data& _data)
{
	bgfx::setPlatformData(_data);
}

void shutdown()
{
	if(s_initted)
		bgfx::shutdown();
}

bool init(renderer_type _type, std::uint16_t _vendorId, std::uint16_t _deviceId)
{
	static gfx_callback callback;

	s_initted = bgfx::init(_type, _vendorId, _deviceId, &callback);

	return s_initted;
}

void vertex_pack(const float _input[], bool _inputNormalized, attribute _attr, const vertex_layout& _decl,
				 void* _data, uint32_t _index)
{
	bgfx::vertexPack(_input, _inputNormalized, _attr, _decl, _data, _index);
}

void vertex_unpack(float _output[], attribute _attr, const vertex_layout& _decl, const void* _data,
				   uint32_t _index)
{
	bgfx::vertexUnpack(_output, _attr, _decl, _data, _index);
}

void vertex_convert(const vertex_layout& _destDecl, void* _destData, const vertex_layout& _srcDecl,
					const void* _srcData, uint32_t _num)
{
	bgfx::vertexConvert(_destDecl, _destData, _srcDecl, _srcData, _num);
}

uint16_t weld_vertices(uint16_t* _output, const vertex_layout& _decl, const void* _data, uint16_t _num,
					   float _epsilon)
{
	return bgfx::weldVertices(_output, _decl, _data, _num, _epsilon);
}

uint32_t topology_convert(topology_conversion _conversion, void* _dst, uint32_t _dstSize,
						  const void* _indices, uint32_t _numIndices, bool _index32)
{
	return bgfx::topologyConvert(_conversion, _dst, _dstSize, _indices, _numIndices, _index32);
}

void topology_sort_tri_list(topology_sort _sort, void* _dst, uint32_t _dstSize, const float _dir[],
							const float _pos[], const void* _vertices, uint32_t _stride, const void* _indices,
							uint32_t _numIndices, bool _index32)
{
	bgfx::topologySortTriList(_sort, _dst, _dstSize, _dir, _pos, _vertices, _stride, _indices, _numIndices,
							  _index32);
}

shader_handle create_embedded_shader(const embedded_shader* _es, renderer_type _type, const char* _name)
{
	return bgfx::createEmbeddedShader(_es, _type, _name);
}

uint8_t get_supported_renderers(uint8_t _max, renderer_type* _enum)
{
	return bgfx::getSupportedRenderers(_max, _enum);
}

const char* get_renderer_name(renderer_type _type)
{
	return bgfx::getRendererName(_type);
}

void reset(uint32_t _width, uint32_t _height, uint32_t _flags)
{
	bgfx::reset(_width, _height, _flags);
}

encoder* begin()
{
	return bgfx::begin();
}

void end(encoder* _encoder)
{
	bgfx::end(_encoder);
}

uint32_t frame(bool _capture)
{
	return bgfx::frame(_capture);
}

renderer_type get_renderer_type()
{
	return bgfx::getRendererType();
}

const caps* get_caps()
{
	return bgfx::getCaps();
}

const hmd* get_hmd()
{
	return bgfx::getHMD();
}

const stats* get_stats()
{
	return bgfx::getStats();
}

const memory_view* alloc(uint32_t _size)
{
	return bgfx::alloc(_size);
}

const memory_view* copy(const void* _data, uint32_t _size)
{
	return bgfx::copy(_data, _size);
}

const memory_view* make_ref(const void* _data, uint32_t _size)
{
	return bgfx::makeRef(_data, _size);
}

const memory_view* make_ref(const void* _data, uint32_t _size, release_fn _releaseFn, void* _userData)
{
	return bgfx::makeRef(_data, _size, _releaseFn, _userData);
}

void set_debug(uint32_t _debug)
{
	bgfx::setDebug(_debug);
}

void dbg_text_clear(uint8_t _attr, bool _small)
{
	bgfx::dbgTextClear(_attr, _small);
}

void dbg_text_printf(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format)
{
	bgfx::dbgTextPrintf(_x, _y, _attr, _format);
}

void dbg_text_vprintf(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, va_list _argList)
{
	bgfx::dbgTextPrintfVargs(_x, _y, _attr, _format, _argList);
}

void dbg_text_image(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const void* _data,
					uint16_t _pitch)
{
	bgfx::dbgTextImage(_x, _y, _width, _height, _data, _pitch);
}

index_buffer_handle create_index_buffer(const memory_view* _mem, uint16_t _flags)
{
	return bgfx::createIndexBuffer(_mem, _flags);
}

void destroy(index_buffer_handle _handle)
{
	bgfx::destroy(_handle);
}

vertex_buffer_handle create_vertex_buffer(const memory_view* _mem, const vertex_layout& _decl,
										  uint16_t _flags)
{
	return bgfx::createVertexBuffer(_mem, _decl, _flags);
}

void destroy(vertex_buffer_handle _handle)
{
	bgfx::destroy(_handle);
}

dynamic_index_buffer_handle create_dynamic_index_buffer(uint32_t _num, uint16_t _flags)
{
	return bgfx::createDynamicIndexBuffer(_num, _flags);
}

dynamic_index_buffer_handle create_dynamic_index_buffer(const memory_view* _mem, uint16_t _flags)
{
	return bgfx::createDynamicIndexBuffer(_mem, _flags);
}

void update_dynamic_index_buffer(dynamic_index_buffer_handle _handle, uint32_t _startIndex,
								 const memory_view* _mem)
{
	bgfx::updateDynamicIndexBuffer(_handle, _startIndex, _mem);
}

void destroy(dynamic_index_buffer_handle _handle)
{
	bgfx::destroy(_handle);
}

dynamic_vertex_buffer_handle create_dynamic_vertex_buffer(uint32_t _num, const vertex_layout& _decl,
														  uint16_t _flags)
{
	return bgfx::createDynamicVertexBuffer(_num, _decl, _flags);
}

dynamic_vertex_buffer_handle create_dynamic_vertex_buffer(const memory_view* _mem, const vertex_layout& _decl,
														  uint16_t _flags)
{
	return bgfx::createDynamicVertexBuffer(_mem, _decl, _flags);
}

void update_dynamic_vertex_buffer(dynamic_vertex_buffer_handle _handle, uint32_t _startVertex,
								  const memory_view* _mem)
{
	bgfx::updateDynamicVertexBuffer(_handle, _startVertex, _mem);
}

void destroy(dynamic_vertex_buffer_handle _handle)
{
	bgfx::destroy(_handle);
}

uint32_t get_avail_transient_index_buffer(uint32_t _num)
{
	return bgfx::getAvailTransientIndexBuffer(_num);
}

uint32_t get_avail_transient_vertex_buffer(uint32_t _num, const vertex_layout& _decl)
{
	return bgfx::getAvailTransientVertexBuffer(_num, _decl);
}

uint32_t get_avail_instance_data_buffer(uint32_t _num, uint16_t _stride)
{
	return bgfx::getAvailInstanceDataBuffer(_num, _stride);
}

void alloc_transient_index_buffer(transient_index_buffer* _tib, uint32_t _num)
{
	bgfx::allocTransientIndexBuffer(_tib, _num);
}

void alloc_transient_vertex_buffer(transient_vertex_buffer* _tvb, uint32_t _num, const vertex_layout& _decl)
{
	bgfx::allocTransientVertexBuffer(_tvb, _num, _decl);
}

bool alloc_transient_buffers(transient_vertex_buffer* _tvb, const vertex_layout& _decl, uint32_t _numVertices,
							 transient_index_buffer* _tib, uint32_t _numIndices)
{
	return bgfx::allocTransientBuffers(_tvb, _decl, _numVertices, _tib, _numIndices);
}

void alloc_instance_data_buffer(instance_data_buffer* _idb, uint32_t _num, uint16_t _stride)
{
	bgfx::allocInstanceDataBuffer(_idb, _num, _stride);
}

indirect_buffer_handle create_indirect_buffer(uint32_t _num)
{
	return bgfx::createIndirectBuffer(_num);
}

void destroy(indirect_buffer_handle _handle)
{
	bgfx::destroy(_handle);
}

shader_handle create_shader(const memory_view* _mem)
{
	return bgfx::createShader(_mem);
}

uint16_t get_shader_uniforms(shader_handle _handle, uniform_handle* _uniforms, uint16_t _max)
{
	return bgfx::getShaderUniforms(_handle, _uniforms, _max);
}

void get_uniform_info(uniform_handle _handle, uniform_info& _info)
{
	bgfx::getUniformInfo(_handle, _info);
}

void set_name(shader_handle _handle, const char* _name)
{
	bgfx::setName(_handle, _name);
}

void destroy(shader_handle _handle)
{
	bgfx::destroy(_handle);
}

program_handle create_program(shader_handle _vsh, shader_handle _fsh, bool _destroyShaders)
{
	return bgfx::createProgram(_vsh, _fsh, _destroyShaders);
}

program_handle create_program(shader_handle _csh, bool _destroyShaders)
{
	return bgfx::createProgram(_csh, _destroyShaders);
}

void destroy(program_handle _handle)
{
	bgfx::destroy(_handle);
}

bool is_texture_valid(uint16_t _depth, bool _cubeMap, uint16_t _numLayers, texture_format _format,
					  uint32_t _flags)
{
	return bgfx::isTextureValid(_depth, _cubeMap, _numLayers, _format, _flags);
}

void calc_texture_size(texture_info& _info, uint16_t _width, uint16_t _height, uint16_t _depth, bool _cubeMap,
					   bool _hasMips, uint16_t _numLayers, texture_format _format)
{
	bgfx::calcTextureSize(_info, _width, _height, _depth, _cubeMap, _hasMips, _numLayers, _format);
}

texture_handle create_texture(const memory_view* _mem, uint32_t _flags, uint8_t _skip, texture_info* _info)
{
	return bgfx::createTexture(_mem, _flags, _skip, _info);
}

texture_handle create_texture_2d(uint16_t _width, uint16_t _height, bool _hasMips, uint16_t _numLayers,
								 texture_format _format, uint32_t _flags, const memory_view* _mem)
{
	return bgfx::createTexture2D(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);
}

texture_handle create_texture_2d(backbuffer_ratio _ratio, bool _hasMips, uint16_t _numLayers,
								 texture_format _format, uint32_t _flags)
{
	return bgfx::createTexture2D(_ratio, _hasMips, _numLayers, _format, _flags);
}

texture_handle create_texture_3d(uint16_t _width, uint16_t _height, uint16_t _depth, bool _hasMips,
								 texture_format _format, uint32_t _flags, const memory_view* _mem)
{
	return bgfx::createTexture3D(_width, _height, _depth, _hasMips, _format, _flags, _mem);
}

texture_handle create_texture_cube(uint16_t _size, bool _hasMips, uint16_t _numLayers, texture_format _format,
								   uint32_t _flags, const memory_view* _mem)
{
	return bgfx::createTextureCube(_size, _hasMips, _numLayers, _format, _flags, _mem);
}

void update_texture_2d(texture_handle _handle, uint16_t _layer, uint8_t _mip, uint16_t _x, uint16_t _y,
					   uint16_t _width, uint16_t _height, const memory_view* _mem, uint16_t _pitch)
{
	bgfx::updateTexture2D(_handle, _layer, _mip, _x, _y, _width, _height, _mem, _pitch);
}

void update_texture_3d(texture_handle _handle, uint8_t _mip, uint16_t _x, uint16_t _y, uint16_t _z,
					   uint16_t _width, uint16_t _height, uint16_t _depth, const memory_view* _mem)
{
	bgfx::updateTexture3D(_handle, _mip, _x, _y, _z, _width, _height, _depth, _mem);
}

void update_texture_cube(texture_handle _handle, uint16_t _layer, uint8_t _side, uint8_t _mip, uint16_t _x,
						 uint16_t _y, uint16_t _width, uint16_t _height, const memory_view* _mem,
						 uint16_t _pitch)
{
	bgfx::updateTextureCube(_handle, _layer, _side, _mip, _x, _y, _width, _height, _mem, _pitch);
}

uint32_t read_texture(texture_handle _handle, void* _data, uint8_t _mip)
{
	return bgfx::readTexture(_handle, _data, _mip);
}

void set_name(texture_handle _handle, const char* _name)
{
	bgfx::setName(_handle, _name);
}

void destroy(texture_handle _handle)
{
	bgfx::destroy(_handle);
}

frame_buffer_handle create_frame_buffer(uint16_t _width, uint16_t _height, texture_format _format,
										uint32_t _textureFlags)
{
	return bgfx::createFrameBuffer(_width, _height, _format, _textureFlags);
}

frame_buffer_handle create_frame_buffer(backbuffer_ratio _ratio, texture_format _format,
										uint32_t _textureFlags)
{
	return bgfx::createFrameBuffer(_ratio, _format, _textureFlags);
}

frame_buffer_handle create_frame_buffer(uint8_t _num, const texture_handle* _handles, bool _destroyTextures)
{
	return bgfx::createFrameBuffer(_num, _handles, _destroyTextures);
}

frame_buffer_handle create_frame_buffer(uint8_t _num, const attachment* _attachment, bool _destroyTextures)
{
	return bgfx::createFrameBuffer(_num, _attachment, _destroyTextures);
}

frame_buffer_handle create_frame_buffer(void* _nwh, uint16_t _width, uint16_t _height,
										texture_format _depthFormat)
{
	return bgfx::createFrameBuffer(_nwh, _width, _height, _depthFormat);
}

texture_handle get_texture(frame_buffer_handle _handle, uint8_t _attachment)
{
	return bgfx::getTexture(_handle, _attachment);
}

void destroy(frame_buffer_handle _handle)
{
	bgfx::destroy(_handle);
}

uniform_handle create_uniform(const char* _name, uniform_type _type, uint16_t _num)
{
	return bgfx::createUniform(_name, _type, _num);
}

void destroy(uniform_handle _handle)
{
	bgfx::destroy(_handle);
}

occlusion_query_handle create_occlusion_query()
{
	return bgfx::createOcclusionQuery();
}

occlusion_query_result get_result(occlusion_query_handle _handle, int32_t* _result)
{
	return bgfx::getResult(_handle, _result);
}

void destroy(occlusion_query_handle _handle)
{
	bgfx::destroy(_handle);
}

void set_palette_color(uint8_t _index, uint32_t _rgba)
{
	bgfx::setPaletteColor(_index, _rgba);
}

void set_palette_color(uint8_t _index, const float _rgba[])
{
	bgfx::setPaletteColor(_index, _rgba);
}

void set_palette_color(uint8_t _index, float _r, float _g, float _b, float _a)
{
	bgfx::setPaletteColor(_index, _r, _g, _b, _a);
}

void set_view_name(view_id _id, const char* _name)
{
	bgfx::setViewName(_id, _name);
}

void set_view_rect(view_id _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
{
	bgfx::setViewRect(_id, _x, _y, _width, _height);
}

void set_view_rect(view_id _id, uint16_t _x, uint16_t _y, backbuffer_ratio _ratio)
{
	bgfx::setViewRect(_id, _x, _y, _ratio);
}

void set_view_scissor(view_id _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
{
	bgfx::setViewScissor(_id, _x, _y, _width, _height);
}

void set_view_clear(view_id _id, uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil)
{
	bgfx::setViewClear(_id, _flags, _rgba, _depth, _stencil);
}

void set_view_clear(view_id _id, uint16_t _flags, float _depth, uint8_t _stencil, uint8_t _0, uint8_t _1,
					uint8_t _2, uint8_t _3, uint8_t _4, uint8_t _5, uint8_t _6, uint8_t _7)
{
	bgfx::setViewClear(_id, _flags, _depth, _stencil, _0, _1, _2, _3, _4, _5, _6, _7);
}

void set_view_mode(view_id _id, view_mode _mode)
{
	bgfx::setViewMode(_id, _mode);
}

void set_view_frame_buffer(view_id _id, frame_buffer_handle _handle)
{
	bgfx::setViewFrameBuffer(_id, _handle);
}

void set_view_transform(view_id _id, const void* _view, const void* _proj)
{
	bgfx::setViewTransform(_id, _view, _proj);
}

void set_view_transform(view_id _id, const void* _view, const void* _projL, uint8_t _flags,
						const void* _projR)
{
	bgfx::setViewTransform(_id, _view, _projL, _flags, _projR);
}

void set_view_order(view_id _id, uint16_t _num, const view_id* _order)
{
	bgfx::setViewOrder(_id, _num, _order);
}

void reset_view(view_id _id)
{
	bgfx::resetView(_id);
}

void set_marker(const char* _marker)
{
	bgfx::setMarker(_marker);
}

void set_state(uint64_t _state, uint32_t _rgba)
{
	bgfx::setState(_state, _rgba);
}

void set_condition(occlusion_query_handle _handle, bool _visible)
{
	bgfx::setCondition(_handle, _visible);
}

void set_stencil(uint32_t _fstencil, uint32_t _bstencil)
{
	bgfx::setStencil(_fstencil, _bstencil);
}

uint16_t set_scissor(uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height)
{
	return bgfx::setScissor(_x, _y, _width, _height);
}

void set_scissor(uint16_t _cache)
{
	bgfx::setScissor(_cache);
}

uint32_t set_transform(const void* _mtx, uint16_t _num)
{
	return bgfx::setTransform(_mtx, _num);
}

uint32_t alloc_transform(transform* _transform, uint16_t _num)
{
	return bgfx::allocTransform(_transform, _num);
}

void set_transform(uint32_t _cache, uint16_t _num)
{
	bgfx::setTransform(_cache, _num);
}

void set_uniform(uniform_handle _handle, const void* _value, uint16_t _num)
{
	bgfx::setUniform(_handle, _value, _num);
}

void set_index_buffer(index_buffer_handle _handle, uint32_t _firstIndex, uint32_t _numIndices)
{
	bgfx::setIndexBuffer(_handle, _firstIndex, _numIndices);
}

void set_index_buffer(dynamic_index_buffer_handle _handle, uint32_t _firstIndex, uint32_t _numIndices)
{
	bgfx::setIndexBuffer(_handle, _firstIndex, _numIndices);
}

void set_index_buffer(const transient_index_buffer* _tib, uint32_t _firstIndex, uint32_t _numIndices)
{
	bgfx::setIndexBuffer(_tib, _firstIndex, _numIndices);
}

void set_vertex_buffer(uint8_t _stream, vertex_buffer_handle _handle, uint32_t _startVertex,
					   uint32_t _numVertices)
{
	bgfx::setVertexBuffer(_stream, _handle, _startVertex, _numVertices);
}

void set_vertex_buffer(uint8_t _stream, dynamic_vertex_buffer_handle _handle, uint32_t _startVertex,
					   uint32_t _numVertices)
{
	bgfx::setVertexBuffer(_stream, _handle, _startVertex, _numVertices);
}

void set_vertex_buffer(uint8_t _stream, const transient_vertex_buffer* _tvb, uint32_t _startVertex,
					   uint32_t _numVertices)
{
	bgfx::setVertexBuffer(_stream, _tvb, _startVertex, _numVertices);
}

void set_instance_data_buffer(const instance_data_buffer* _idb, uint32_t _num)
{
	bgfx::setInstanceDataBuffer(_idb, _num);
}

void set_instance_data_buffer(vertex_buffer_handle _handle, uint32_t _startVertex, uint32_t _num)
{
	bgfx::setInstanceDataBuffer(_handle, _startVertex, _num);
}

void set_instance_data_buffer(dynamic_vertex_buffer_handle _handle, uint32_t _startVertex, uint32_t _num)
{
	bgfx::setInstanceDataBuffer(_handle, _startVertex, _num);
}

void set_texture(uint8_t _stage, uniform_handle _sampler, texture_handle _handle, uint32_t _flags)
{
	bgfx::setTexture(_stage, _sampler, _handle, _flags);
}

void touch(view_id _id)
{
	bgfx::touch(_id);
}

void submit(view_id _id, program_handle _handle, int32_t _depth, bool _preserveState)
{
	bgfx::submit(_id, _handle, _depth, _preserveState);
}

void submit(view_id _id, program_handle _program, occlusion_query_handle _occlusionQuery, int32_t _depth,
			bool _preserveState)
{
	bgfx::submit(_id, _program, _occlusionQuery, _depth, _preserveState);
}

void submit(view_id _id, program_handle _handle, indirect_buffer_handle _indirectHandle, uint16_t _start,
			uint16_t _num, int32_t _depth, bool _preserveState)
{
	bgfx::submit(_id, _handle, _indirectHandle, _start, _num, _depth, _preserveState);
}

void set_image(uint8_t _stage, uniform_handle _sampler, texture_handle _handle, uint8_t _mip, access _access,
			   texture_format _format)
{
	bgfx::setImage(_stage, _sampler, _handle, _mip, _access, _format);
}

void set_buffer(uint8_t _stage, index_buffer_handle _handle, access _access)
{
	bgfx::setBuffer(_stage, _handle, _access);
}

void set_buffer(uint8_t _stage, vertex_buffer_handle _handle, access _access)
{
	bgfx::setBuffer(_stage, _handle, _access);
}

void set_buffer(uint8_t _stage, dynamic_index_buffer_handle _handle, access _access)
{
	bgfx::setBuffer(_stage, _handle, _access);
}

void set_buffer(uint8_t _stage, dynamic_vertex_buffer_handle _handle, access _access)
{
	bgfx::setBuffer(_stage, _handle, _access);
}

void set_buffer(uint8_t _stage, indirect_buffer_handle _handle, access _access)
{
	bgfx::setBuffer(_stage, _handle, _access);
}

void dispatch(view_id _id, program_handle _handle, uint32_t _numX, uint32_t _numY, uint32_t _numZ,
			  uint8_t _flags)
{
	bgfx::dispatch(_id, _handle, _numX, _numY, _numZ, _flags);
}

void dispatch(view_id _id, program_handle _handle, indirect_buffer_handle _indirectHandle, uint16_t _start,
			  uint16_t _num, uint8_t _flags)
{
	bgfx::dispatch(_id, _handle, _indirectHandle, _start, _num, _flags);
}

void discard()
{
	bgfx::discard();
}

void blit(view_id _id, texture_handle _dst, uint16_t _dstX, uint16_t _dstY, texture_handle _src,
		  uint16_t _srcX, uint16_t _srcY, uint16_t _width, uint16_t _height)
{
	bgfx::blit(_id, _dst, _dstX, _dstY, _src, _srcX, _srcY, _width, _height);
}

void blit(view_id _id, texture_handle _dst, uint8_t _dstMip, uint16_t _dstX, uint16_t _dstY, uint16_t _dstZ,
		  texture_handle _src, uint8_t _srcMip, uint16_t _srcX, uint16_t _srcY, uint16_t _srcZ,
		  uint16_t _width, uint16_t _height, uint16_t _depth)
{
	bgfx::blit(_id, _dst, _dstMip, _dstX, _dstY, _dstZ, _src, _srcMip, _srcX, _srcY, _srcZ, _width, _height,
			   _depth);
}

void request_screen_shot(frame_buffer_handle _handle, const char* _filePath)
{
	bgfx::requestScreenShot(_handle, _filePath);
}

void flush()
{
	bgfx::frame();
	bgfx::frame();
}

std::uint64_t screen_quad(float dest_width, float dest_height, float depth, float width, float height)
{
	float texture_half = get_half_texel();
	bool origin_bottom_left = is_origin_bottom_left();

	if(3 == getAvailTransientVertexBuffer(3, pos_texcoord0_vertex::get_layout()))
	{
		transient_vertex_buffer vb;
		alloc_transient_vertex_buffer(&vb, 3, pos_texcoord0_vertex::get_layout());
		auto vertex = reinterpret_cast<pos_texcoord0_vertex*>(vb.data);

		const float minx = -width;
		const float maxx = width;
		const float miny = 0.0f;
		const float maxy = height * 2.0f;

		const float texel_half_w = texture_half / dest_width;
		const float texel_half_h = texture_half / dest_height;
		const float minu = -1.0f + texel_half_w;
		const float maxu = 1.0f + texel_half_h;

		const float zz = depth;

		float minv = texel_half_h;
		float maxv = 2.0f + texel_half_h;

		if(origin_bottom_left)
		{
			float temp = minv;
			minv = maxv;
			maxv = temp;

			minv -= 1.0f;
			maxv -= 1.0f;
		}

		vertex[0].x = minx;
		vertex[0].y = miny;
		vertex[0].z = zz;
		vertex[0].u = minu;
		vertex[0].v = minv;

		vertex[1].x = maxx;
		vertex[1].y = miny;
		vertex[1].z = zz;
		vertex[1].u = maxu;
		vertex[1].v = minv;

		vertex[2].x = maxx;
		vertex[2].y = maxy;
		vertex[2].z = zz;
		vertex[2].u = maxu;
		vertex[2].v = maxv;

		bgfx::setVertexBuffer(0, &vb);
	}

	return 0;
}

std::uint64_t clip_quad(float depth, float width, float height)
{
	// float texture_half = get_half_texel();
	bool origin_bottom_left = is_origin_bottom_left();

	if(4 == getAvailTransientVertexBuffer(4, pos_texcoord0_vertex::get_layout()))
	{
		transient_vertex_buffer vb;
		alloc_transient_vertex_buffer(&vb, 4, pos_texcoord0_vertex::get_layout());
		auto vertex = reinterpret_cast<pos_texcoord0_vertex*>(vb.data);

		const float minx = -width;
		const float maxx = width;
		const float miny = -height;
		const float maxy = height;

		// const float texel_half_w = texture_half;
		// const float texel_half_h = texture_half;
		const float minu = 0.0f;
		const float maxu = 1.0f;

		const float zz = depth;

		float minv = 1.0f;
		float maxv = 0.0f;

		if(origin_bottom_left)
		{
			minv = 1.0f - minv;
			maxv = 1.0f - maxv;
		}

		vertex[0].x = minx;
		vertex[0].y = maxy;
		vertex[0].z = zz;
		vertex[0].u = minu;
		vertex[0].v = maxv;

		vertex[1].x = maxx;
		vertex[1].y = maxy;
		vertex[1].z = zz;
		vertex[1].u = maxu;
		vertex[1].v = maxv;

		vertex[2].x = minx;
		vertex[2].y = miny;
		vertex[2].z = zz;
		vertex[2].u = minu;
		vertex[2].v = minv;

		vertex[3].x = maxx;
		vertex[3].y = miny;
		vertex[3].z = zz;
		vertex[3].u = maxu;
		vertex[3].v = minv;

		bgfx::setVertexBuffer(0, &vb);
	}

	return BGFX_STATE_PT_TRISTRIP;
}

void get_size_from_ratio(backbuffer_ratio _ratio, std::uint16_t& _width, std::uint16_t& _height)
{
	auto stats = bgfx::getStats();
	_width = stats->width;
	_height = stats->height;
	switch(_ratio)
	{
		case backbuffer_ratio::Half:
			_width /= 2;
			_height /= 2;
			break;
		case backbuffer_ratio::Quarter:
			_width /= 4;
			_height /= 4;
			break;
		case backbuffer_ratio::Eighth:
			_width /= 8;
			_height /= 8;
			break;
		case backbuffer_ratio::Sixteenth:
			_width /= 16;
			_height /= 16;
			break;
		case backbuffer_ratio::Double:
			_width *= 2;
			_height *= 2;
			break;

		default:
			break;
	}

	_width = std::max<std::uint16_t>(1, _width);
	_height = std::max<std::uint16_t>(1, _height);
}

const std::string& get_renderer_filename_extension()
{
	static const std::map<renderer_type, std::string> types = {
		{renderer_type::Direct3D9, ".dx9"},   {renderer_type::Direct3D11, ".dx11"},
		{renderer_type::Direct3D12, ".dx12"}, {renderer_type::Gnm, ".gnm"},
		{renderer_type::Metal, ".metal"},	 {renderer_type::OpenGL, ".gl"},
		{renderer_type::OpenGLES, ".gles"},   {renderer_type::Noop, ".noop"}};

	const auto it = types.find(bgfx::getRendererType());
	if(it != types.cend())
	{
		return it->second;
	}
	static std::string unknown = ".unknown";
	return unknown;
}

bool is_homogeneous_depth()
{
	return bgfx::getCaps()->homogeneousDepth;
}

bool is_origin_bottom_left()
{
	return bgfx::getCaps()->originBottomLeft;
}

std::uint32_t get_max_blend_transforms()
{
	return 128;
}

float get_half_texel()
{
	const renderer_type renderer = bgfx::getRendererType();
	float half_texel = renderer_type::Direct3D9 == renderer ? 0.5f : 0.0f;
	return half_texel;
}
}
