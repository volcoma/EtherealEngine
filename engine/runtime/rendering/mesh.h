#pragma once

#include "core/graphics/graphics.h"
#include "core/math/math_includes.h"
#include "core/reflection/registration.h"
#include "core/serialization/serialization.h"
#include <vector>
#include <map>
#include <memory>

struct vertex_buffer;
struct index_buffer;

namespace triangle_flags
{
	enum e
	{
		none = 0,
		degenerate = 0x1,
	};
}

enum class mesh_status
{
	not_prepared,
	preparing,
	prepared
};

enum class mesh_create_origin
{
	bottom,
	center,
	top
};

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : skin_bind_data (Class)
/// <summary>
/// Structure describing how a skinned mesh should be bound to any bones
/// that influence its vertices.
/// </summary>
//-----------------------------------------------------------------------------
class skin_bind_data
{
	REFLECTABLE(skin_bind_data)
	SERIALIZABLE(skin_bind_data)
public:
	//-------------------------------------------------------------------------
	// Public Typedefs, Structures & Enumerations
	//-------------------------------------------------------------------------
	// Describes how a bone influences a specific vertex.
	struct vertex_influence
	{
		std::uint32_t vertex_index = 0;
		float weight = 0.0f;

		// Constructors
		vertex_influence() {}
		vertex_influence(std::uint32_t _index, float _weight) :
			vertex_index(_index), weight(_weight) {}
	};
	using vertex_influence_array_t = std::vector<vertex_influence>;
	// Describes the vertices that are connected to the referenced bone, and how much influence it has on them.
	struct bone_influence
	{
		/// Unique identifier of the bone from which transformation information should be taken.
		std::string bone_id;
		/// The "bind pose" or "offset" transform that describes how the bone was positioned in relation to the original vertex data.
		math::transform bind_pose_transform;
		/// List of vertices influenced by the referenced bone.
		vertex_influence_array_t influences;
	};
	using bone_influence_array_t = std::vector<bone_influence>;
	// Contains per-vertex influence and weight information.
	struct vertex_data
	{
		/// List of bones that influence this vertex.
		std::vector<std::int32_t> influences;
		/// List of weights that describe how this vertex is influenced.
		std::vector<float> weights;
		/// Index of the palette to which this vertex has been assigned.
		std::int32_t palette;
		/// The index of the original vertex stored in the mesh.
		std::uint32_t original_vertex;
	};
	using vertex_data_array_t = std::vector<vertex_data>;
	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : add_bone()
	/// <summary>
	/// Add influence information for a specific bone.
	/// </summary>
	//-----------------------------------------------------------------------------
	void add_bone(const bone_influence& bone);

	//-----------------------------------------------------------------------------
	//  Name : remove_empty_bones()
	/// <summary>
	/// Strip out any bones that did not contain any influences.
	/// </summary>
	//-----------------------------------------------------------------------------
	void remove_empty_bones();

	//-----------------------------------------------------------------------------
	//  Name : build_vertex_table()
	/// <summary>
	/// Construct a list of bone influences and weights for each vertex based 
	/// on the binding data provided.
	/// </summary>
	//-----------------------------------------------------------------------------
	void build_vertex_table(std::uint32_t vertex_count, const std::vector<std::uint32_t>& vertex_remap, vertex_data_array_t& table);

	//-----------------------------------------------------------------------------
	//  Name : remap_vertices()
	/// <summary>
	/// remap the vertex references stored in the binding based on the supplied
	/// remap array (Array[old vertex index] = new vertex index). Store an index
	/// of 0xFFFFFFFF to indicate that the vertex was removed, or use an index
	/// greater than the remap array size to indicate the new location of a split
	/// vertex.
	/// </summary>
	//-----------------------------------------------------------------------------
	void remap_vertices(const std::vector<std::uint32_t>& remap);

	//-----------------------------------------------------------------------------
	//  Name : get_bones ()
	/// <summary>
	/// Retrieve a list of all bones that influence the skin in some way.
	/// </summary>
	//-----------------------------------------------------------------------------
	const bone_influence_array_t& get_bones() const;

	//-----------------------------------------------------------------------------
	//  Name : get_bones ()
	/// <summary>
	/// Retrieve a list of all bones that influence the skin in some way.
	/// </summary>
	//-----------------------------------------------------------------------------
	bone_influence_array_t& get_bones();

	//-----------------------------------------------------------------------------
	//  Name : getBones ()
	/// <summary>
	/// Retrieve whether or not his is a valid skin data
	/// </summary>
	//-----------------------------------------------------------------------------
	bool has_bones() const;

	//-----------------------------------------------------------------------------
	//  Name : find_bone_by_id ()
	/// <summary>
	/// Finds a bone by id.
	/// </summary>
	//-----------------------------------------------------------------------------
	const bone_influence* find_bone_by_id(const std::string& id) const;

	//-----------------------------------------------------------------------------
	//  Name : clear_vertex_influences()
	/// <summary>
	/// Release memory allocated for vertex influences in each stored bone.
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear_vertex_influences();

	//-----------------------------------------------------------------------------
	//  Name : clear()
	/// <summary>
	/// Clears out the bone information stored in this object.
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear();
private:
	//-------------------------------------------------------------------------
	// Private Variables
	//-------------------------------------------------------------------------
	/// List of bones that influence the skin mesh vertices.
	bone_influence_array_t _bones;

}; // End Class skin_bind_data

//-----------------------------------------------------------------------------
//  Name : bone_palette (Class)
/// <summary>
/// Outlines a collection of bones that influence a given set of faces / 
/// vertices in the mesh.
/// </summary>
//-----------------------------------------------------------------------------
class bone_palette
{
public:
	//-------------------------------------------------------------------------
	// Public Typedefs, Structures & Enumerations
	//-------------------------------------------------------------------------
	using bone_index_map_t = std::map<std::uint32_t, std::uint32_t>;
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	bone_palette(std::uint32_t paletteSize);
	bone_palette(const bone_palette & init);
	~bone_palette();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : get_skinning_matrices()
	/// <summary>
	/// Gather the bone / palette information and matrices ready for
	/// drawing the skinned mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::vector<math::transform> get_skinning_matrices(
        const math::transform& root_transform,
		const std::vector<math::transform>& node_transforms,
		const skin_bind_data& bind_data,
		bool compute_inverse_transpose) const;

	//-----------------------------------------------------------------------------
	//  Name : compute_palette_fit()
	/// <summary>
	/// Determine the relevant "fit" information that can be used to 
	/// discover if and how the specified combination of bones will fit into 
	/// this palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	void compute_palette_fit(
		bone_index_map_t& input, 
		std::int32_t& current_space, 
		std::int32_t& common_base, 
		std::int32_t& additional_bones);

	//-----------------------------------------------------------------------------
	//  Name : assign_bones()
	/// <summary>
	/// Assign the specified bones (and faces) to this bone palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	void assign_bones(bone_index_map_t& bones, std::vector<std::uint32_t>& faces);

	//-----------------------------------------------------------------------------
	//  Name : assign_bones()
	/// <summary>
	/// Assign the specified bones to this bone palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	void assign_bones(const std::vector<std::uint32_t>& bones);

	//-----------------------------------------------------------------------------
	//  Name : translate_bone_to_palette ()
	/// <summary>
	/// Translate the specified bone index into its associated position in 
	/// the palette. If it does not exist, a value of -1 will be returned.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t translate_bone_to_palette(std::uint32_t bone_index) const;

	//-----------------------------------------------------------------------------
	//  Name : get_maximum_blend_index ()
	/// <summary>
	/// Retrieve the maximum vertex blend index for this palette (i.e. if 
	/// every vertex was only influenced by one bone, this variable would 
	/// contain a value of 0).
	/// </summary>
	//-----------------------------------------------------------------------------
	std::int32_t get_maximum_blend_index() const;

	//-----------------------------------------------------------------------------
	//  Name : get_maximum_size ()
	/// <summary>
	/// Retrieve the maximum size of the palette -- the maximum number of bones
	/// capable of being referenced.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t get_maximum_size() const;

	//-----------------------------------------------------------------------------
	//  Name : get_data_group ()
	/// <summary>
	/// Retrieve the identifier of the data group assigned to the subset of the
	/// mesh reserved for this bone palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t get_data_group() const;

	//-----------------------------------------------------------------------------
	//  Name : get_influenced_faces ()
	/// <summary>
	/// Retrieve the list of faces assigned to this palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::vector<std::uint32_t>& get_influenced_faces();

	//-----------------------------------------------------------------------------
	//  Name : get_bones ()
	/// <summary>
	/// Retrieve the indices of the bones referenced by this palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<std::uint32_t>& get_bones() const;

	//-----------------------------------------------------------------------------
	//  Name : set_maximum_blend_index ()
	/// <summary>
	/// Set the maximum vertex blend index for this palette (i.e. if every 
	/// vertex was only influenced by one bone, this variable would contain a
	/// value of 0).
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_maximum_blend_index(int index);

	//-----------------------------------------------------------------------------
	//  Name : set_data_group ()
	/// <summary>
	/// Set the identifier of the data group assigned to the subset of the mesh 
	/// reserved for this bone palette.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_data_group(std::uint32_t group);

	//-----------------------------------------------------------------------------
	//  Name : clear_influenced_faces ()
	/// <summary>
	/// Clear out the temporary face influences array.
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear_influenced_faces();

protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	/// Sorted list of bones in this palette. References the elements in the standard list.
	bone_index_map_t _bones_lut;
	/// Main palette of indices that reference the bones outlined in the main skin binding data.
	std::vector<std::uint32_t> _bones;
	/// List of faces assigned to this palette.
	std::vector<std::uint32_t> _faces;
	/// The data group identifier used to separate the mesh data into subsets relevant to this bone palette.
	std::uint32_t _data_group_id;
	/// The maximum size of this palette.
	std::uint32_t _maximum_size;
	/// The maximum vertex blend index for this palette (i.e. if every vertex was only influenced by one bone, this variable would contain a value of 0).
	std::int32_t _maximum_blend_index;

}; // End Class bone_palette


class mesh
{
public:
	//-------------------------------------------------------------------------
	// Public Structures, Typedefs and Enumerations
	//-------------------------------------------------------------------------
	// Structure describing an individual "piece" of the mesh, often grouped
	// by material, but can be any arbitrary collection of triangles.
	struct subset
	{
		/// The unique user assigned "data group" that can be used to separate subsets.
		std::uint32_t data_group_id = 0;
		/// The beginning vertex for this batch.
		std::int32_t vertex_start = -1;
		/// Number of vertices included in this batch.
        std::uint32_t vertex_count = 0;
		/// The initial face, from the index buffer, to render in this batch
		std::int32_t face_start = -1;
		/// Number of faces to render in this batch.
        std::uint32_t face_count = 0;
	};

	struct info
	{
		std::uint32_t vertices = 0;
		std::uint32_t primitives = 0;
		std::uint32_t subsets = 0;
	};

	// Structure describing data for a single triangle in the mesh.
	struct triangle
	{
		std::uint32_t data_group_id = 0;
        std::uint32_t indices[3] = {0, 0, 0};
		std::uint8_t flags = 0;
	};

	using triangle_array_t = std::vector<triangle>;
	using subset_array_t = std::vector<subset*>;
	using bone_palette_array_t = std::vector<bone_palette>;

	struct armature_node
	{
		std::string name;
        math::transform local_transform;
        math::transform world_transform;
		std::vector<std::unique_ptr<armature_node>> children;

	};

	// mesh Construction Structures
	struct load_data
	{
		struct load_material
		{
			enum texture_type
			{
				BaseColor,
				Emissive,
				Normal,
				AO,
				Roughness,
				Metalness,
				Count
			};

			math::color base_color = { 0.0f, 0.0f, 0.0f, 0.0f };
			math::color emissive_color = { 0.0f, 0.0f, 0.0f, 0.0f };
			std::vector<std::string> textures = std::vector<std::string>(texture_type::Count);
		};

		/// The format of the vertex data currently being used to prepare the mesh.
		gfx::VertexDecl	vertex_format;
		/// Final vertex buffer currently being prepared.
		std::vector<std::uint8_t> vertex_data;
		/// Total number of vertices currently stored here.
		std::uint32_t vertex_count = 0;
		/// This is used to store the current face / triangle data.
		triangle_array_t triangle_data;
		/// Total number of triangles currently stored here.
		std::uint32_t triangle_count = 0;
		/// Skin data for this import
		skin_bind_data skin_data;
		/// Imported nodes
		std::unique_ptr<armature_node> root_node = nullptr;
		/// Use texture_type as index
		std::vector<load_material> materials;
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	mesh();
	~mesh();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	// Rendering methods
	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	/// Clears out all the mesh data.
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose();

	//-----------------------------------------------------------------------------
	//  Name : draw ()
	/// <summary>
	/// Draw the mesh in its entirety.
	/// </summary>
	//-----------------------------------------------------------------------------
	void draw();

	//-----------------------------------------------------------------------------
	//  Name : draw ()
	/// <summary>
	/// Draw the mesh (face count = num_faces).
	/// Note : Used during volume rendering.
	/// </summary>
	//-----------------------------------------------------------------------------
	void draw(std::uint32_t face_count);

	//-----------------------------------------------------------------------------
	//  Name : draw_subset ()
	/// <summary>
	/// Draw an individual subset of the mesh based on the material AND
	/// data group specified.
	/// </summary>
	//-----------------------------------------------------------------------------
	void draw_subset(std::uint32_t data_group_id);

	// mesh creation methods
	//-----------------------------------------------------------------------------
	//  Name : prepare_mesh ()
	/// <summary>
	/// Prepare the mesh immediately with the specified data.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool prepare_mesh(const gfx::VertexDecl& vertex_format);

	//-----------------------------------------------------------------------------
	//  Name : prepare_mesh ()
	/// <summary>
	/// Prepare the mesh immediately with the specified data.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool prepare_mesh(
		const gfx::VertexDecl& vertex_format,
		void * vertices,
		std::uint32_t vertex_count,
		const triangle_array_t& faces,
		bool hardware_copy = true,
		bool weld = true,
		bool optimize = true);

	//-----------------------------------------------------------------------------
	//  Name : set_vertex_source ()
	/// <summary>
	/// While preparing the mesh, this function should be called in order to
	/// specify the source of the vertex buffer to pull data from.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool set_vertex_source(void* source, std::uint32_t vertex_count, const gfx::VertexDecl& source_format);

	//-----------------------------------------------------------------------------
	//  Name : add_primitives ()
	/// <summary>
	/// Called by an external source (such as the geometry loader) in order
	/// to populate the internal buffers ready for building the mesh.
	/// Note : This may be called multiple times until the mesh is ready to be built
	/// via the 'endPrepare' method.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool add_primitives(const triangle_array_t& triangles);

	//-----------------------------------------------------------------------------
	//  Name : bind_skin ()
	/// <summary>
	/// Given the specified skin binding data, convert the internal mesh data 
	/// into the format required for skinning.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool bind_skin(const skin_bind_data& bind_data);

	//-----------------------------------------------------------------------------
	//  Name : bind_armature ()
	/// <summary>
	/// Bind the armature tree.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool bind_armature(std::unique_ptr<armature_node>& root);

	//-----------------------------------------------------------------------------
	//  Name : create_cube ()
	/// <summary>
	/// Create cube geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_cube(
		const gfx::VertexDecl& format,
		float width,
		float height,
		float depth,
		std::uint32_t width_segments,
		std::uint32_t height_segments,
		std::uint32_t depth_segments,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_cube ()
	/// <summary>
	/// Create cube geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_cube(
		const gfx::VertexDecl& format,
		float width,
		float height,
		float depth,
		std::uint32_t width_segments,
		std::uint32_t height_segments,
		std::uint32_t depth_segments,
		float tex_u_scale,
		float tex_v_scale,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_sphere ()
	/// <summary>
	/// Create sphere geometry.
	/// </summary>
	//-----------------------------------------------------------------------------	
	bool create_sphere(
		const gfx::VertexDecl& format,
		float radius,
		std::uint32_t stacks,
		std::uint32_t slices,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_cylinder ()
	/// <summary>
	/// Create cylinder geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_cylinder(
		const gfx::VertexDecl& format,
		float radius,
		float height,
		std::uint32_t stacks,
		std::uint32_t slices,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_capsule ()
	/// <summary>
	/// Create capsule geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_capsule(
		const gfx::VertexDecl& format,
		float radius,
		float height,
		std::uint32_t stacks,
		std::uint32_t slices,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_cone ()
	/// <summary>
	/// Create cone geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_cone(
		const gfx::VertexDecl& format,
		float radius,
		float radiusTip,
		float height,
		std::uint32_t stacks,
		std::uint32_t slices,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_torus ()
	/// <summary>
	/// Create torus geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_torus(
		const gfx::VertexDecl& format,
		float outer_radius,
		float inner_radius,
		std::uint32_t bands,
		std::uint32_t sides,
		bool inverted,
		mesh_create_origin origin,
		bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_teapot ()
	/// <summary>
	/// Create teapot geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_teapot(const gfx::VertexDecl& format, bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_tetrahedron ()
	/// <summary>
	/// Create tetrahedron geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_tetrahedron(const gfx::VertexDecl& format, bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_octahedron ()
	/// <summary>
	/// Create octahedron geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_octahedron(const gfx::VertexDecl& format, bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_icosahedron ()
	/// <summary>
	/// Create icosahedron geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_icosahedron(const gfx::VertexDecl& format, bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_dodecahedron ()
	/// <summary>
	/// Create dodecahedron geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_dodecahedron(const gfx::VertexDecl& format, bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : create_dodecahedron ()
	/// <summary>
	/// Create icosphere geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool create_icosphere(const gfx::VertexDecl& format, int tesselation_level, bool hardware_copy = true);
	//-----------------------------------------------------------------------------
	//  Name : end_prepare ()
	/// <summary>
	/// All data has been added to the mesh and we should now build the
	/// render data for the mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool end_prepare(bool hardware_copy = true, bool weld = true, bool optimize = true, bool build_buffers = true);

	//-----------------------------------------------------------------------------
	//  Name : build_vb ()
	/// <summary>
	/// Builds internal vertex buffer
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void build_vb(bool hardware_copy = true);

	//-----------------------------------------------------------------------------
	//  Name : build_ib ()
	/// <summary>
	/// Builds internal index buffer
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void build_ib(bool hardware_copy = true);

	// Utility functions
	//-----------------------------------------------------------------------------
	//  Name : generate_adjacency ()
	/// <summary>
	/// Generates edge-triangle adjacency information for the mesh data either
	/// prior to, or after building the hardware buffers. Input array will be
	/// automatically sized, and will contain 3 values per triangle contained
	/// in the mesh representing the indices to adjacent faces for each edge in 
	/// the triangle (or 0xFFFFFFFF if there is no adjacent face).
	/// </summary>
	//-----------------------------------------------------------------------------
	bool generate_adjacency(std::vector<std::uint32_t>& adjacency);

	// Object access methods

	//-----------------------------------------------------------------------------
	//  Name : get_face_count ()
	/// <summary>
	/// Determine the number of faces stored here. If the mesh has already
	/// been finalized with a call to endPrepare(), this will be the total
	/// number of triangles stored. Otherwise, this will be the number of
	/// windings maintained within the mesh ready for preparation.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t get_face_count() const;

	//-----------------------------------------------------------------------------
	//  Name : get_vertex_count ()
	/// <summary>
	/// Determine the number of vertices stored here. If the mesh has already
	/// been finalized with a call to endPrepare(), this will be the total
	/// number of vertices stored. Otherwise, this will be the number of
	/// vertices currently maintained within the mesh ready for preparation.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t get_vertex_count() const;

	//-----------------------------------------------------------------------------
	//  Name : get_system_vb ()
	/// <summary>
	/// Retrieve the underlying vertex data from the mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint8_t* get_system_vb();

	//-----------------------------------------------------------------------------
	//  Name : get_system_ib ()
	/// <summary>
	/// Retrieve the underlying index data from the mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t* get_system_ib();

	//-----------------------------------------------------------------------------
	//  Name : get_vertex_format ()
	/// <summary>
	/// Retrieve the format of the underlying mesh vertex data.
	/// </summary>
	//-----------------------------------------------------------------------------
	const gfx::VertexDecl& get_vertex_format() const;

	//-----------------------------------------------------------------------------
	//  Name : get_skin_bind_data ()
	/// <summary>
	/// If this mesh has been bound as a skin, this method can be called to
	/// retrieve the original data that was used to bind it.
	/// </summary>
	//-----------------------------------------------------------------------------
	const skin_bind_data&	get_skin_bind_data() const;

	//-----------------------------------------------------------------------------
	//  Name : get_bone_palettes ()
	/// <summary>
	/// If this mesh has been bound as a skin, this method can be called to
	/// retrieve the compiled bone combination palette data.
	/// </summary>
	//-----------------------------------------------------------------------------
	const bone_palette_array_t& get_bone_palettes() const;


    const armature_node* get_armature() const;
	//-----------------------------------------------------------------------------
	//  Name : get_subset ()
	/// <summary>
	/// Retrieve information about the subset of the mesh that is associated with
	/// the specified material and data group identifier.
	/// </summary>
	//-----------------------------------------------------------------------------
	const subset* get_subset(std::uint32_t data_group_id = 0) const;
	//-------------------------------------------------------------------------
	// Public Inline Methods
	//-------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	//  Name : get_bounds ()
	/// <summary>
	/// Gets the local bounding box for this mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::bbox& get_bounds() const { return _bbox; }

	//-----------------------------------------------------------------------------
	//  Name : get_status ()
	/// <summary>
	/// Gets the peparation status for this mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline mesh_status get_status() const { return _prepare_status; }

	//-----------------------------------------------------------------------------
	//  Name : get_subset_count ()
	/// <summary>
	/// Gets the number of subsets for this mesh
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::size_t get_subset_count() const { return _mesh_subsets.size(); }

protected:
	//-------------------------------------------------------------------------
	// Protected Structures, Typedefs and Enumerations
	//-------------------------------------------------------------------------
	using data_group_subset_map_t = std::map<std::uint32_t, subset_array_t>;
	using byte_array_t = std::vector<std::uint8_t>;

	// mesh Construction Structures
	struct preparation_data
	{
		enum flags
		{
			source_contains_normal = 0x1,
			source_contains_binormal = 0x2,
			source_contains_tangent = 0x4
		};

		/// The source vertex data currently being used to prepare the mesh.
		std::uint8_t* vertex_source = nullptr;
		/// Do we own the source data?
		bool owns_source = false;
		/// The format of the vertex data currently being used to prepare the mesh.
		gfx::VertexDecl	source_format;
		/// Records the location in the vertex buffer that each vertex has been placed during data insertion.
		std::vector<std::uint32_t> vertex_records;
		/// Final vertex buffer currently being prepared.
		byte_array_t vertex_data;
		/// Provides additional descriptive information about the vertices in the above buffer (i.e. source provided a normal, etc.)
		byte_array_t vertex_flags;
		/// This is used to store the current face / triangle data.
		triangle_array_t triangle_data;
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

	}; // End Struct preparation_data

	// mesh Sorting / Optimization structures
	struct optimizer_vertex_info
	{
		/// The position of the vertex in the pseudo-cache
		std::int32_t cache_position = -1;
		/// The score associated with this vertex
		float vertex_score = 0.0f;
		/// Total number of triangles that reference this vertex that have not yet been added
		std::uint32_t unused_triangle_references = 0;
		/// List of all of the triangles referencing this vertex
		std::vector<std::uint32_t> triangle_references;

	}; // End Struct optimizer_vertex_info

	struct optimizer_triangle_info
	{
		/// The sum of all three child vertex scores.
		float triangle_score = 0.0f;
		/// Has the triangle been added to the draw list already?
		bool added = false;

	}; // End Struct optimizer_triangle_info

	struct adjacent_edge_key
	{
		/// Pointer to the first vertex in the edge
		const math::vec3* vertex1 = nullptr;
		/// Pointer to the second vertex in the edge
		const math::vec3* vertex2 = nullptr;

	}; // End Struct adjacent_edge_key

	struct mesh_subset_key
	{
		/// The data group identifier for this subset.
		std::uint32_t data_group_id = 0;

		// Constructors
		mesh_subset_key() :
			data_group_id(0) {}
		mesh_subset_key(std::uint32_t _dataGroupId) :
			data_group_id(_dataGroupId) {}

	}; // End Struct mesh_subset_key

	using subset_key_map_t = std::map<mesh_subset_key, subset*>;
	using subset_key_array_t = std::vector<mesh_subset_key>;

	// Simple structure to allow us to leverage the hierarchical properties of a map
	// to accelerate the weld operation.
	struct weld_key
	{
		// Pointer to the vertex for easy access.
		std::uint8_t* vertex;
		// Format of the above vertex for easy access.
		gfx::VertexDecl format;
		// The tolerance we're using to weld (transport only, these should be the same for every key).
		float tolerance;
	};

	struct face_influences
	{
		bone_palette::bone_index_map_t bones;          // List of unique bones that influence a given number of faces.
	};

	// Simple structure to allow us to leverage the hierarchical properties of a map
	// to accelerate the bone index combination process.
	struct bone_combination_key
	{
		face_influences* influences = nullptr;

		// Constructor
		bone_combination_key(face_influences * _influences) :
			influences(_influences) {}

	};
	using bone_combination_map_t = std::map<bone_combination_key, std::vector<std::uint32_t>*>;

	//-------------------------------------------------------------------------
	// Friend List
	//-------------------------------------------------------------------------
	friend bool operator < (const adjacent_edge_key& key1, const adjacent_edge_key& key2);
	friend bool operator < (const mesh_subset_key& key1, const mesh_subset_key& key2);
	friend bool operator < (const weld_key& key1, const weld_key& key2);
	friend bool operator < (const bone_combination_key& key1, const bone_combination_key& key2);
	//-------------------------------------------------------------------------
	// Protected Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : generate_vertex_components () (Private)
	/// <summary>
	/// Some vertex components potentially need to be generated. This may
	/// include vertex normals, binormals or tangents. This function will
	/// generate any such components which were not provided.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool generate_vertex_components(bool weld);

	//-----------------------------------------------------------------------------
	//  Name : generate_vertex_normals () (Private)
	/// <summary>
	/// Generates any vertex normals that may have been requested but not
	/// provided when adding vertex data.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool generate_vertex_normals(std::uint32_t* adjacency_ptr, std::vector<std::uint32_t>* remap_array_ptr = nullptr);

	//-----------------------------------------------------------------------------
	//  Name : generate_vertex_barycentrics ()
	/// <summary>
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool generate_vertex_barycentrics(std::uint32_t* adjacency);

	//-----------------------------------------------------------------------------
	//  Name : generate_vertex_tangents () 
	/// <summary>
	/// Builds the tangent space vectors for this polygon. 
	/// Credit to Terathon Software - http://www.terathon.com/code/tangent.html 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool generate_vertex_tangents();

	//-----------------------------------------------------------------------------
	//  Name : weld_vertices ()
	/// <summary>
	/// Weld all of the vertices together that can be combined.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool weld_vertices(float tolerance = 0.000001f, std::vector<std::uint32_t>* vertexRemap = nullptr);

	//-----------------------------------------------------------------------------
	// Name : sort_mesh_data() (Protected)
	/// <summary>
	/// Sort the data in the mesh into material & datagroup order.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool sort_mesh_data(bool optimize, bool hardware_copy, bool build_buffer);

	//-----------------------------------------------------------------------------
	//  Name : render_mesh_data () (Private)
	/// <summary>
	/// Given the specified subset values, simply render the selected batch
	/// of primitives the required number of times based on their material.
	/// </summary>
	//-----------------------------------------------------------------------------
	void render_mesh_data(std::uint32_t face_start, std::uint32_t face_count, std::uint32_t vertex_start, std::uint32_t vertex_count);

	//-------------------------------------------------------------------------
	// Protected Static Functions
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : build_optimized_index_buffer () (Private, Static)
	/// <summary>
	/// Calculate the best order for triangle data, optimizing for efficient
	/// use of the hardware vertex cache, given an unkown vertex cache size
	/// and implementation.
	/// Note : Thanks to Tom Forsyth for the fantastic implementation on which
	/// this is based.
	/// URL  : http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
	/// </summary>
	//-----------------------------------------------------------------------------
	static void build_optimized_index_buffer(
		const subset* subset,
		std::uint32_t* source_buffer_ptr, 
		std::uint32_t* destination_buffer_ptr,
		std::uint32_t minimum_vertex, 
		std::uint32_t maximum_vertex);

	//-----------------------------------------------------------------------------
	//  Name : find_vertex_optimizer_score () (Private, Static)
	/// <summary>
	/// During optimization, this method will generate scores used to
	/// identify important vertices when ordering triangle data.
	/// Note : Thanks to Tom Forsyth for the fantastic implementation on which
	/// this is based.
	/// URL  : http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
	/// </summary>
	//-----------------------------------------------------------------------------
	static float find_vertex_optimizer_score(const optimizer_vertex_info* vertex_info_ptr);

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

	// mesh data
	/// The vertex data as it exists during data insertion (prior to the actual build) and also used as the system memory copy.
	std::uint8_t* _system_vb = nullptr;
	/// Vertex format used for the mesh internal vertex data.
	gfx::VertexDecl _vertex_format;
	/// The final system memory copy of the index buffer.
	std::uint32_t* _system_ib = nullptr;
	/// Material and data group information for each triangle.
	subset_key_array_t _triangle_data;
	/// After constructing the mesh, this will contain the actual hardware vertex buffer resource
    std::shared_ptr<vertex_buffer> _hardware_vb;
	/// After constructing the mesh, this will contain the actual hardware index buffer resource
    std::shared_ptr<index_buffer> _hardware_ib;

	// mesh data look up tables
	/// The actual list of subsets maintained by this mesh.
	subset_array_t _mesh_subsets;
	/// A map containing lookup information which maps data groups to subsets batched by material.
	data_group_subset_map_t _data_groups;
	/// Quick binary tree lookup of existing subsets based on material AND data group id.
	subset_key_map_t _subset_lookup;

	// mesh properties
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

	// mesh data preparation
	/// Preparation status of the mesh (i.e. has it been constructed yet).
	mesh_status _prepare_status;
	/// Input data used for constructing the final mesh.
	preparation_data _preparation_data;

	// Skin binding information
	/// Data that describes how the mesh should be bound as a skin with supplied bone matrices.
	skin_bind_data _skin_bind_data;
	/// List of each of the unique combinations of bones to use during rendering.
	bone_palette_array_t _bone_palettes;
	/// List of each of armature nodes
	std::unique_ptr<armature_node> _root = nullptr;
};

//-----------------------------------------------------------------------------
// Global Operators
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : operator < () (adjacent_edge_key&, adjacent_edge_key&)
/// <summary>
/// Perform less than comparison on the adjacent_edge_key structure.
/// </summary>
//-----------------------------------------------------------------------------
inline bool operator < (const mesh::adjacent_edge_key& key1, const mesh::adjacent_edge_key& key2);

//-----------------------------------------------------------------------------
//  Name : operator < () (mesh_subset_key&, mesh_subset_key&)
/// <summary>
/// Perform less than comparison on the mesh_subset_key structure.
/// </summary>
//-----------------------------------------------------------------------------
inline bool operator < (const mesh::mesh_subset_key& key1, const mesh::mesh_subset_key& key2);

//-----------------------------------------------------------------------------
//  Name : operator < () (weld_key&, weld_key&)
/// <summary>
/// Perform less than comparison on the weld_key structure.
/// </summary>
//-----------------------------------------------------------------------------
inline bool operator < (const mesh::weld_key& key1, const mesh::weld_key& key2);

//-----------------------------------------------------------------------------
//  Name : operator < () (bone_combination_key&, bone_combination_key&)
/// <summary>
/// Perform less than comparison on the bone_combination_key structure.
/// </summary>
//-----------------------------------------------------------------------------
inline bool operator < (const mesh::bone_combination_key& key1, const mesh::bone_combination_key& key2);
