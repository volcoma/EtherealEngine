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


namespace MeshStatus
{
	enum E
	{
		NotPrepared,
		Preparing,
		Prepared
	};

}; // End Namespace : MeshPrepareStatus

namespace MeshDrawMode
{
	enum E
	{
		Automatic,
		Simple
	};

}; // End Namespace : MeshDrawMode

namespace MeshCreateOrigin
{
	enum E
	{
		Bottom,
		Center,
		Top
	};

}; // End Namespace : MeshCreateOrigin

namespace ScaleMode
{
	enum E
	{
		Absolute = 0,
		Relative = 1
	};

}; // End Namespace : ScaleMode
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

	class Mesh
	{
	public:
		//-------------------------------------------------------------------------
		// Public Structures, Typedefs and Enumerations
		//-------------------------------------------------------------------------
		// Structure describing an individual "piece" of the mesh, often grouped
		// by material , but can be any arbitrary collection of triangles.
		struct MeshSubset
		{
			std::uint32_t            dataGroupId;    // The unique user assigned "data group" that can be used to separate subsets.
			std::int32_t             vertexStart;    // The beginning vertex for this batch.
			std::int32_t             vertexCount;    // Number of vertices included in this batch.
			std::int32_t             faceStart;      // The initial face, from the index buffer, to render in this batch
			std::int32_t             faceCount;      // Number of faces to render in this batch.

			// Constructor
			MeshSubset()
			{
				dataGroupId = 0;
				vertexStart = -1;
				vertexCount = 0;
				faceStart = -1;
				faceCount = 0;

			} // End Constructor

		};

		// Structure describing data for a single triangle in the mesh.
		struct Triangle
		{
			Triangle() : dataGroupId(0), flags(0) {}

			std::uint32_t            dataGroupId;
			std::uint32_t            indices[3];
			std::uint8_t              flags;

		};

		using TriangleArray = std::vector<Triangle>;
		using SubsetArray = std::vector<MeshSubset*>;
		using UInt32Array = std::vector<uint32_t>;
		// Mesh Construction Structures
		struct LoadData
		{
			LoadData()
				: vertexCount(0)
				, triangleCount(0)
			{}


			gfx::VertexDecl	vertexFormat;       // The format of the vertex data currently being used to prepare the mesh.
			std::vector<std::uint8_t>		vertexData;         // Final vertex buffer currently being prepared.
			std::uint32_t			vertexCount;        // Total number of vertices currently stored here.
			TriangleArray   triangleData;       // This is used to store the current face / triangle data.
			std::uint32_t			triangleCount;      // Total number of triangles currently stored here.
		};



		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		Mesh();
		virtual ~Mesh();

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		// Rendering methods
		void dispose();
		void                    draw();
		void                    draw(std::uint32_t faceCount);
		void                    drawSubset();
		void                    drawSubset(std::uint32_t nDataGroupId);

		// Mesh creation methods
		bool                    prepareMesh(const gfx::VertexDecl& vertexFormat, bool rollBackPrepare = false);
		bool                    prepareMesh(const gfx::VertexDecl& vertexFormat, void * vertices, std::uint32_t vertexCount, const TriangleArray & faces, bool hardwareCopy = true, bool weld = true, bool optimize = true);
		bool                    setVertexSource(void * source, std::uint32_t vertexCount, const gfx::VertexDecl& sourceFormat);
		bool                    addPrimitives(const TriangleArray & triangles);

		void                    renderMeshData(std::uint32_t faceStart, std::uint32_t faceCount, std::uint32_t vertexStart, std::uint32_t vertexCount);
		bool                    createBox(const gfx::VertexDecl& format, float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);
		bool                    createBox(const gfx::VertexDecl& format, float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, float texUScale, float texVScale, bool inverted, MeshCreateOrigin::E origin, bool hardwareCopy = true);

		bool                    endPrepare(bool hardwareCopy = true, bool weld = true, bool optimize = true);

		// Utility functions
		bool                    scaleMeshData(float scale);
		bool                    generateAdjacency(UInt32Array & adjacency);

		// Object access methods
		std::uint32_t					getFaceCount() const;
		std::uint32_t					getVertexCount() const;

		gfx::VertexDecl&	getVertexFormat();

		const MeshSubset      * getSubset(std::uint32_t dataGroupId = 0) const;
		//-------------------------------------------------------------------------
		// Public Inline Methods
		//-------------------------------------------------------------------------
		inline const math::bbox& getBoundingBox() const { return mBoundingBox; }
		inline MeshStatus::E    getPrepareStatus() const { return mPrepareStatus; }
		inline size_t			getSubsetCount() const { return mMeshSubsets.size(); }


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

			std::uint8_t        *	vertexSource;       // The source vertex data currently being used to prepare the mesh.
			bool            ownsSource;         // Do we own the source data?
			gfx::VertexDecl	sourceFormat;       // The format of the vertex data currently being used to prepare the mesh.
			UInt32Array		vertexRecords;      // Records the location in the vertex buffer that each vertex has been placed during data insertion.
			ByteArray		vertexData;         // Final vertex buffer currently being prepared.
			ByteArray		vertexFlags;        // Provides additional descriptive information about the vertices in the above buffer (i.e. source provided a normal, etc.)
			TriangleArray   triangleData;       // This is used to store the current face / triangle data.
			std::uint32_t			triangleCount;      // Total number of triangles currently stored here.
			std::uint32_t			vertexCount;        // Total number of vertices currently stored here.


			bool            computeNormals;     // Vertex normals should be computed (at least for any vertices where none were supplied).
			bool            computeBinormals;   // Vertex binormals should be computed (at least for any vertices where none were supplied).
			bool            computeTangents;    // Vertex binormals should be computed (at least for any vertices where none were supplied).

		}; // End Struct PreparationData

		// Mesh Sorting / Optimization structures
		struct OptimizerVertexInfo
		{
			std::int32_t       cachePosition;                // The position of the vertex in the pseudo-cache
			float       vertexScore;                  // The score associated with this vertex
			std::uint32_t      unusedTriangleReferences;     // Total number of triangles that reference this vertex that have not yet been added
			UInt32Array triangleReferences;           // List of all of the triangles referencing this vertex

			// Constructor
			OptimizerVertexInfo() :
				cachePosition(-1),
				vertexScore(0.0f),
				unusedTriangleReferences(0) {}

		}; // End Struct OptimizerVertexInfo

		struct OptimizerTriangleInfo
		{
			float     triangleScore;              // The sum of all three child vertex scores.
			bool        added;                      // Has the triangle been added to the draw list already?

			// Constructor
			OptimizerTriangleInfo() :
				triangleScore(0.0f),
				added(false) {}

		}; // End Struct OptimizerTriangleInfo

		struct AdjacentEdgeKey
		{
			const math::vec3 * vertex1;            // Pointer to the first vertex in the edge
			const math::vec3 * vertex2;            // Pointer to the second vertex in the edge

		}; // End Struct AdjacentEdgeKey

		struct MeshSubsetKey
		{
			std::uint32_t            dataGroupId;        // The data group identifier for this subset.

			// Constructors
			MeshSubsetKey() :
				dataGroupId(0) {}
			MeshSubsetKey(std::uint32_t _dataGroupId) :
				dataGroupId(_dataGroupId) {}

		}; // End Struct MeshSubsetKey

		using SubsetKeyMap = std::map<MeshSubsetKey, MeshSubset*>;
		using SubsetKeyArray = std::vector<MeshSubsetKey>;

		// Simple structure to allow us to leverage the hierarchical properties of a map
		// to accelerate the weld operation.
		struct WeldKey
		{
			std::uint8_t  * vertex;         // Pointer to the vertex for easy access.
			gfx::VertexDecl format;         // Format of the above vertex for easy access.
			float         tolerance;      // The tolerance we're using to weld (transport only, these should be the same for every key).
		};


		//-------------------------------------------------------------------------
		// Friend List
		//-------------------------------------------------------------------------
		friend bool operator < (const AdjacentEdgeKey & key1, const AdjacentEdgeKey & key2);
		friend bool operator < (const MeshSubsetKey & key1, const MeshSubsetKey & key2);

		//-------------------------------------------------------------------------
		// Protected Methods
		//-------------------------------------------------------------------------
		bool                    generateVertexComponents(bool weld);
		bool                    generateVertexNormals(std::uint32_t * adjacency, UInt32Array * remapArray = nullptr);
		bool                    generateVertexTangents();
		bool                    weldVertices(UInt32Array * vertexRemap = nullptr);
		void                    renderMeshData(MeshDrawMode::E mode, /*const MaterialHandle * material, */std::uint32_t faceStart, std::uint32_t faceCount, std::uint32_t vertexStart, std::uint32_t vertexCount);
		bool                    sortMeshData(bool optimize, bool buildHardwareBuffers);

		//-------------------------------------------------------------------------
		// Protected Static Functions
		//-------------------------------------------------------------------------
		static void             buildOptimizedIndexBuffer(const MeshSubset * subset, std::uint32_t * sourceBuffer, std::uint32_t * destinationBuffer, std::uint32_t minimumVertex, std::uint32_t maximumVertex);
		static float			findVertexOptimizerScore(const OptimizerVertexInfo * vertexInfo);
		static bool             subsetSortPredicate(const MeshSubset * lhs, const MeshSubset * rhs);

		//-------------------------------------------------------------------------
		// Protected Variables
		//-------------------------------------------------------------------------
		// Resource loading properties.
		bool                    mForceTangentGen;       // Should we force the re-generation of tangent space vectors?
		bool                    mForceNormalGen;        // Should we force the re-generation of vertex normals?
		bool                    mDisableFinalSort;      // Allows derived classes to disable / enable the automatic re-sort operation that happens during several operations such as setFaceMaterial(), etc.

		// Mesh data
		std::uint8_t                *	mSystemVB;              // The vertex data as it exists during data insertion (prior to the actual build) and also used as the system memory copy.
		gfx::VertexDecl      mVertexFormat;          // Vertex format used for the mesh internal vertex data.
		std::uint32_t              *	mSystemIB;              // The final system memory copy of the index buffer.
		SubsetKeyArray          mTriangleData;          // Material and data group information for each triangle.
		std::shared_ptr<VertexBuffer>		mHardwareVB = std::make_shared<VertexBuffer>();            // After constructing the mesh, this will contain the actual hardware vertex buffer resource
		std::shared_ptr<IndexBuffer>		mHardwareIB = std::make_shared<IndexBuffer>();            // After constructing the mesh, this will contain the actual hardware index buffer resource

		// Mesh data look up tables
		SubsetArray             mMeshSubsets;           // The actual list of subsets maintained by this mesh.
		DataGroupSubsetMap      mDataGroups;            // A map containing lookup information which maps data groups to subsets batched by material.

		SubsetKeyMap            mSubsetLookup;          // Quick binary tree lookup of existing subsets based on material AND data group id.

		// Mesh properties
		bool                    mHardwareMesh;          // Does the mesh use a hardware vertex/index buffer?
		bool                    mOptimizedMesh;         // Was the mesh optimized when it was prepared?
		math::bbox				mBoundingBox;           // Axis aligned bounding box describing object dimensions (in object space)
		std::uint32_t					mFaceCount;             // Total number of faces in the prepared mesh.
		std::uint32_t					mVertexCount;           // Total number of vertices in the prepared mesh.

		// Mesh data preparation
		MeshStatus::E			mPrepareStatus;         // Preparation status of the mesh (i.e. has it been constructed yet).
		PreparationData         mPrepareData;           // Input data used for constructing the final mesh.

	};

	//-----------------------------------------------------------------------------
	// Global Operators
	//-----------------------------------------------------------------------------
	inline bool operator < (const Mesh::AdjacentEdgeKey & key1, const Mesh::AdjacentEdgeKey & key2);
	inline bool operator < (const Mesh::MeshSubsetKey & key1, const Mesh::MeshSubsetKey & key2);
}