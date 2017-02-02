#pragma once

#include "graphics/graphics.h"
#include "core/math/math_includes.h"
#include <vector>
#include <map>
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
	std::uint32_t start_index = 0;
	std::uint32_t num_indices = 0;
	std::uint32_t start_vertex = 0;
	std::uint32_t num_vertices = 0;
};

struct Group
{
	std::string material;
	std::vector<Subset> subsets;
	std::uint32_t vertices = 0;
	std::uint32_t indices = 0;
	std::uint32_t primitives = 0;
	std::shared_ptr<VertexBuffer> vertex_buffer;
	std::shared_ptr<IndexBuffer> index_buffer;
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

	/// Vertex declaration for this mesh
	gfx::VertexDecl decl;
	/// All subset groups
	std::vector<Group> groups;
	/// Local bounding box.
	math::bbox aabb;
	/// Mesh info
	MeshInfo info;
};




namespace experimental
{
	namespace TriangleFlags
	{
		enum Base
		{
			None = 0,
			Degenerate = 0x1,
		};

	} // End namespace : TriangleFlags

	namespace MeshStatus
	{
		enum E
		{
			NotPrepared,
			Preparing,
			Prepared
		};

	}; // End Namespace : MeshPrepareStatus

	namespace MeshCreateOrigin
	{
		enum E
		{
			Bottom,
			Center,
			Top
		};

	}; // End Namespace : MeshCreateOrigin

	class Mesh
	{
	public:
		//-------------------------------------------------------------------------
		// Public Structures, Typedefs and Enumerations
		//-------------------------------------------------------------------------
		// Structure describing an individual "piece" of the mesh, often grouped
		// by material, but can be any arbitrary collection of triangles.
		struct Subset
		{
			/// The unique user assigned "data group" that can be used to separate subsets.
			std::uint32_t data_group_id = 0;
			/// The beginning vertex for this batch.
			std::int32_t vertex_start = -1;
			/// Number of vertices included in this batch.
			std::int32_t vertex_count = 0;
			/// The initial face, from the index buffer, to render in this batch
			std::int32_t face_start = -1;
			/// Number of faces to render in this batch.
			std::int32_t face_count = 0;
		};

		// Structure describing data for a single triangle in the mesh.
		struct Triangle
		{
			std::uint32_t data_group_id = 0;
			std::uint32_t indices[3];
			std::uint8_t flags = 0;
		};

		using TriangleArray = std::vector<Triangle>;
		using SubsetArray = std::vector<Subset*>;
		using UInt32Array = std::vector<uint32_t>;

		// Mesh Construction Structures
		struct LoadData
		{
			/// The format of the vertex data currently being used to prepare the mesh.
			gfx::VertexDecl	vertex_format;
			/// Final vertex buffer currently being prepared.
			std::vector<std::uint8_t> vertex_data;
			/// Total number of vertices currently stored here.
			std::uint32_t vertex_count = 0;
			/// This is used to store the current face / triangle data.
			TriangleArray triangle_data;
			/// Total number of triangles currently stored here.
			std::uint32_t triangle_count = 0;
		};

		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		Mesh();
		~Mesh();

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		// Rendering methods
		void dispose();
		void draw();
		void draw(std::uint32_t faceCount);
		void draw_subset(std::uint32_t nDataGroupId);

		// Mesh creation methods
		bool prepare_mesh(const gfx::VertexDecl& vertexFormat, bool rollBackPrepare = false);
		bool prepare_mesh(const gfx::VertexDecl& vertexFormat, void * vertices, std::uint32_t vertexCount, const TriangleArray & faces, bool hardwareCopy = true, bool weld = true, bool optimize = true);
		bool set_vertex_source(void * source, std::uint32_t vertexCount, const gfx::VertexDecl& sourceFormat);
		bool add_primitives(const TriangleArray & triangles);

		bool create_cube(const gfx::VertexDecl& format, float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool create_cube(const gfx::VertexDecl& format, float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, float texUScale, float texVScale, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool create_sphere(const gfx::VertexDecl& format, float radius, std::uint32_t stacks, std::uint32_t slices, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool create_cylinder(const gfx::VertexDecl& format, float radius, float height, std::uint32_t stacks, std::uint32_t slices, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool create_capsule(const gfx::VertexDecl& format, float radius, float height, std::uint32_t stacks, std::uint32_t slices, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool create_cone(const gfx::VertexDecl& format, float radius, float radiusTip, float height, std::uint32_t stacks, std::uint32_t slices, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool create_torus(const gfx::VertexDecl& format, float outerRadius, float innerRadius, std::uint32_t bands, std::uint32_t sides, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);

		bool end_prepare(bool hardwareCopy = true, bool weld = true, bool optimize = true);

		// Utility functions
		bool generate_adjacency(UInt32Array & adjacency);

		// Object access methods
		std::uint32_t get_face_count() const;
		std::uint32_t get_vertex_count() const;
		std::uint8_t* get_system_vb();
		std::uint32_t* get_system_ib();
		const gfx::VertexDecl& get_vertex_format() const;
		const Subset* get_subset(std::uint32_t dataGroupId = 0) const;
		//-------------------------------------------------------------------------
		// Public Inline Methods
		//-------------------------------------------------------------------------
		inline const math::bbox& get_bounds() const { return _bbox; }
		inline MeshStatus::E get_status() const { return _prepare_status; }
		inline std::size_t get_subset_count() const { return _mesh_subsets.size(); }

	protected:
		//-------------------------------------------------------------------------
		// Protected Structures, Typedefs and Enumerations
		//-------------------------------------------------------------------------
		using DataGroupSubsetMap = std::map<std::uint32_t, SubsetArray>;
		using ByteArray = std::vector<std::uint8_t>;

		// Mesh Construction Structures
		struct PreparationData
		{
			enum Flags
			{
				SourceContainsNormal = 0x1,
				SourceContainsBinormal = 0x2,
				SourceContainsTangent = 0x4
			};

			/// The source vertex data currently being used to prepare the mesh.
			std::uint8_t* vertex_source = nullptr;
			/// Do we own the source data?
			bool owns_source = false;
			/// The format of the vertex data currently being used to prepare the mesh.
			gfx::VertexDecl	source_format;
			/// Records the location in the vertex buffer that each vertex has been placed during data insertion.
			UInt32Array vertex_records;
			/// Final vertex buffer currently being prepared.
			ByteArray vertex_data;
			/// Provides additional descriptive information about the vertices in the above buffer (i.e. source provided a normal, etc.)
			ByteArray vertex_flags;
			/// This is used to store the current face / triangle data.
			TriangleArray triangle_data;
			/// Total number of triangles currently stored here.
			std::uint32_t triangle_count = 0;
			/// Total number of vertices currently stored here.
			std::uint32_t vertex_count = 0;
			/// Vertex normals should be computed (at least for any vertices where none were supplied).
			bool compute_normals = false;
			/// Vertex binormals should be computed (at least for any vertices where none were supplied).
			bool compute_binormals = false;
			/// Vertex binormals should be computed (at least for any vertices where none were supplied).
			bool compute_tangents = false;
			/// Vertex barycentric should be computed (at least for any vertices where none were supplied).
			bool compute_barycentric = false;

		}; // End Struct PreparationData

		// Mesh Sorting / Optimization structures
		struct OptimizerVertexInfo
		{
			/// The position of the vertex in the pseudo-cache
			std::int32_t cache_position = -1;
			/// The score associated with this vertex
			float vertex_score = 0.0f;
			/// Total number of triangles that reference this vertex that have not yet been added
			std::uint32_t unused_triangle_references = 0;
			/// List of all of the triangles referencing this vertex
			UInt32Array triangle_references;

		}; // End Struct OptimizerVertexInfo

		struct OptimizerTriangleInfo
		{
			/// The sum of all three child vertex scores.
			float triangle_score = 0.0f;
			/// Has the triangle been added to the draw list already?
			bool added = false;

		}; // End Struct OptimizerTriangleInfo

		struct AdjacentEdgeKey
		{
			/// Pointer to the first vertex in the edge
			const math::vec3* vertex1 = nullptr;
			/// Pointer to the second vertex in the edge
			const math::vec3* vertex2 = nullptr;

		}; // End Struct AdjacentEdgeKey

		struct MeshSubsetKey
		{
			/// The data group identifier for this subset.
			std::uint32_t data_group_id = 0;

			// Constructors
			MeshSubsetKey() :
				data_group_id(0) {}
			MeshSubsetKey(std::uint32_t _dataGroupId) :
				data_group_id(_dataGroupId) {}

		}; // End Struct MeshSubsetKey

		using SubsetKeyMap = std::map<MeshSubsetKey, Subset*>;
		using SubsetKeyArray = std::vector<MeshSubsetKey>;
		// Simple structure to allow us to leverage the hierarchical properties of a map
		// to accelerate the weld operation.
		struct WeldKey
		{
			// Pointer to the vertex for easy access.
			std::uint8_t* vertex;
			// Format of the above vertex for easy access.
			gfx::VertexDecl format;
			// The tolerance we're using to weld (transport only, these should be the same for every key).
			float tolerance;
		};
		//-------------------------------------------------------------------------
		// Friend List
		//-------------------------------------------------------------------------
		friend bool operator < (const AdjacentEdgeKey& key1, const AdjacentEdgeKey& key2);
		friend bool operator < (const MeshSubsetKey& key1, const MeshSubsetKey& key2);
		friend bool operator < (const WeldKey& key1, const WeldKey& key2);
		//-------------------------------------------------------------------------
		// Protected Methods
		//-------------------------------------------------------------------------
		bool generate_vertex_components(bool weld);
		bool generate_vertex_normals(std::uint32_t* adjacency, UInt32Array* remapArray = nullptr);
		bool generate_vertex_barycentrics(std::uint32_t* adjacency);
		bool generate_vertex_tangents();
		bool weld_vertices(float tolerance = 0.000001f, UInt32Array* vertexRemap = nullptr);
		bool sort_mesh_data(bool optimize, bool buildHardwareBuffers);

		//-------------------------------------------------------------------------
		// Protected Static Functions
		//-------------------------------------------------------------------------
		static void build_optimized_index_buffer(const Subset* subset, std::uint32_t* sourceBuffer, std::uint32_t* destinationBuffer, std::uint32_t minimumVertex, std::uint32_t maximumVertex);
		static float find_vertex_optimizer_score(const OptimizerVertexInfo* vertexInfo);

		//-------------------------------------------------------------------------
		// Protected Variables
		//-------------------------------------------------------------------------
		// Resource loading properties.
		/// Should we force the re-generation of tangent space vectors?
		bool _force_tangent_generation;
		/// Should we force the re-generation of vertex normals?
		bool _force_normal_generation;
		/// Should we force the re-generation of vertex barycentric coords?
		bool _force_barycentric_generation;
		/// Allows derived classes to disable / enable the automatic re-sort operation that happens during several operations such as setFaceMaterial(), etc.
		bool _disable_final_sort;

		// Mesh data
		/// The vertex data as it exists during data insertion (prior to the actual build) and also used as the system memory copy.
		std::uint8_t* _system_vb = nullptr;
		/// Vertex format used for the mesh internal vertex data.
		gfx::VertexDecl _vertex_format;
		/// The final system memory copy of the index buffer.
		std::uint32_t* _system_ib = nullptr;
		/// Material and data group information for each triangle.
		SubsetKeyArray _triangle_data;
		/// After constructing the mesh, this will contain the actual hardware vertex buffer resource
		std::shared_ptr<VertexBuffer> _hardware_vb = std::make_shared<VertexBuffer>();
		/// After constructing the mesh, this will contain the actual hardware index buffer resource
		std::shared_ptr<IndexBuffer> _hardware_ib = std::make_shared<IndexBuffer>();

		// Mesh data look up tables
		/// The actual list of subsets maintained by this mesh.
		SubsetArray _mesh_subsets;
		/// A map containing lookup information which maps data groups to subsets batched by material.
		DataGroupSubsetMap _data_groups;
		/// Quick binary tree lookup of existing subsets based on material AND data group id.
		SubsetKeyMap _subset_lookup;

		// Mesh properties
		/// Does the mesh use a hardware vertex/index buffer?
		bool _hardware_mesh;
		/// Was the mesh optimized when it was prepared?
		bool _optimize_mesh;
		/// Axis aligned bounding box describing object dimensions (in object space)
		math::bbox _bbox;
		/// Total number of faces in the prepared mesh.
		std::uint32_t _face_count;
		/// Total number of vertices in the prepared mesh.
		std::uint32_t _vertex_count;

		// Mesh data preparation
		/// Preparation status of the mesh (i.e. has it been constructed yet).
		MeshStatus::E _prepare_status;
		/// Input data used for constructing the final mesh.
		PreparationData _preparation_data;
	};

	//-----------------------------------------------------------------------------
	// Global Operators
	//-----------------------------------------------------------------------------
	inline bool operator < (const Mesh::AdjacentEdgeKey& key1, const Mesh::AdjacentEdgeKey& key2);
	inline bool operator < (const Mesh::MeshSubsetKey& key1, const Mesh::MeshSubsetKey& key2);
	inline bool operator < (const Mesh::WeldKey& key1, const Mesh::WeldKey& key2);
}