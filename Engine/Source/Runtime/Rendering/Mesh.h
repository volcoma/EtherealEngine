#pragma once

#include "Graphics/graphics.h"
#include "Core/math/math_includes.h"
#include <vector>
#include <memory>
struct VertexBuffer;
struct IndexBuffer;

struct Primitive
{
	std::string name;
	uint32_t m_startIndex;
	uint32_t m_numIndices;
	uint32_t m_startVertex;
	uint32_t m_numVertices;
};

struct Group
{
	std::string material;
	std::vector<Primitive> primitives;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
};

struct Mesh
{
	//-----------------------------------------------------------------------------
	//  Name : isValid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isValid() const;

	//-----------------------------------------------------------------------------
	//  Name : submit ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void submit(uint8_t _id, gfx::ProgramHandle _program, const float* _mtx, uint64_t _state) const;

	/// Vertex declaration for this mesh
	gfx::VertexDecl decl;
	/// All primitive groups
	std::vector<Group> groups;
	/// Local bounding box.
	math::bbox aabb;
};