#pragma once

#include "graphics/graphics.h"
#include "core/math/math_includes.h"
#include <vector>
#include <memory>
struct VertexBuffer;
struct IndexBuffer;


struct MeshInfo
{
	std::uint32_t vertices = 0;
	std::uint32_t indices = 0;
	std::uint32_t primitives = 0;
};

struct Subset
{
	std::string name;
	std::uint32_t m_startIndex = 0;
	std::uint32_t m_numIndices = 0;
	std::uint32_t m_startVertex = 0;
	std::uint32_t m_numVertices = 0;
};

struct Group
{
	std::string material;
	std::vector<Subset> subsets;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
};

struct Mesh
{
	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;

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
	/// All subset groups
	std::vector<Group> groups;
	/// Local bounding box.
	math::bbox aabb;
	/// Mesh info
	MeshInfo info;
};