#include "mesh.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "core/memory/checked_delete.h"
#include "core/logging/logging.h"
#include <algorithm>


//-----------------------------------------------------------------------------
// Local Module Level Namespaces.
//-----------------------------------------------------------------------------
// Settings for the mesh optimizer.
namespace MeshOptimizer
{
	const float CacheDecayPower = 1.5f;
	const float LastTriScore = 0.75f;
	const float ValenceBoostScale = 2.0f;
	const float ValenceBoostPower = 0.5f;
	const std::int32_t MaxVertexCacheSize = 32;

};

Mesh::Mesh()
{
	// Initialize variable to sensible defaults
	_bbox.reset();
	_prepare_status = MeshStatus::NotPrepared;
	_face_count = 0;
	_vertex_count = 0;
	_hardware_mesh = false;
	_optimize_mesh = false;
	_disable_final_sort = false;
	_preparation_data.vertex_source = nullptr;
	_preparation_data.owns_source = false;
	_preparation_data.triangle_count = 0;
	_preparation_data.vertex_count = 0;
	_preparation_data.compute_normals = false;
	_preparation_data.compute_binormals = false;
	_preparation_data.compute_tangents = false;
	_system_vb = nullptr;
	_system_ib = nullptr;
	_force_tangent_generation = false;
	_force_normal_generation = false;
	_force_barycentric_generation = true;
}

Mesh::~Mesh()
{
	dispose();
}

void Mesh::dispose()
{
	// Iterate through the different subsets in the mesh and clean up
	for (auto subset : _mesh_subsets)
	{
		// Just perform a standard 'disconnect' in the 
		// regular unload case.
		checked_delete(subset);
	}

	_mesh_subsets.clear();
	_subset_lookup.clear();
	_data_groups.clear();


	// Release bone palettes and skin data (if any)
	_bone_palettes.clear();
	_skin_bind_data.clear();

	// Clean up preparation data.
	if (_preparation_data.owns_source == true)
		checked_array_delete(_preparation_data.vertex_source);
	_preparation_data.vertex_source = nullptr;
	_preparation_data.source_format = {};
	_preparation_data.owns_source = false;
	_preparation_data.vertex_data.clear();
	_preparation_data.vertex_flags.clear();
	_preparation_data.vertex_records.clear();
	_preparation_data.triangle_data.clear();

	// Release mesh data memory
	checked_array_delete(_system_vb);
	checked_array_delete(_system_ib);

	_triangle_data.clear();

	// Release resources
	_hardware_vb.reset();
	_hardware_ib.reset();

	// Clear variables
	_preparation_data.vertex_source = nullptr;
	_preparation_data.owns_source = false;
	_preparation_data.source_format = {};
	_preparation_data.triangle_count = 0;
	_preparation_data.vertex_count = 0;
	_preparation_data.compute_normals = false;
	_preparation_data.compute_binormals = false;
	_preparation_data.compute_tangents = false;
	_prepare_status = MeshStatus::NotPrepared;
	_face_count = 0;
	_vertex_count = 0;
	_system_vb = nullptr;
	_vertex_format = {};
	_system_ib = nullptr;
	_force_tangent_generation = false;
	_force_normal_generation = false;
	_force_barycentric_generation = true;

	// Reset structures
	_bbox.reset();
}

//-----------------------------------------------------------------------------
// Name : bindSkin ()
// Desc : Given the specified skin binding data, convert the internal mesh data 
//        into the format required for skinning.
//-----------------------------------------------------------------------------
bool Mesh::bind_skin(const SkinBindData& bind_data)
{
	if (!bind_data.has_bones())
		return true;

	SkinBindData::BindVertexArray vertex_table;
	BoneCombinationMap bone_combinations;

	// Get access to required systems and limits
	std::uint32_t palette_size = gfx::get_max_blend_transforms();

	// Destroy any previous palette entries.
	_bone_palettes.clear();

	_skin_bind_data.clear();
	_skin_bind_data = bind_data;

	  // If the mesh has already been prepared, roll it back.
	if (_prepare_status == MeshStatus::Prepared)
		prepare_mesh(_vertex_format, true);

	FaceInfluences used_bones;
	// Build a list of all bone indices and associated weights for each vertex.
	_skin_bind_data.build_vertex_table(_preparation_data.vertex_count, _preparation_data.vertex_records, vertex_table);

	// Vertex based influences are no longer required. Clear them to save space.
	_skin_bind_data.clear_vertex_influences();

	// Now build a list of unique bone combinations that influences each face in the mesh
	TriangleArray& tri_data = _preparation_data.triangle_data;
	for (std::uint32_t i = 0; i < _preparation_data.triangle_count; ++i)
	{
		// Clear out any previous bone references and set up for new run.
		used_bones.bones.clear();

		// Collect all unique bone indices from each of the three face vertices.
		for (std::uint32_t j = 0; j < 3; ++j)
		{
			std::uint32_t vertex = tri_data[i].indices[j];

			// Add each influencing bone if unique.
			for (size_t k = 0; k < vertex_table[vertex].influences.size(); ++k)
				used_bones.bones[vertex_table[vertex].influences[k]] = 1; // Just set to any value, we're only using a key/value dictionary for acceleration

		} // Next Vertex

		  // Now that we have a unique list of bones that influence this face,
		  // determine if any faces with identical influences already exist and 
		  // add it to the list. Alternatively this may be the first face with
		  // these exact influences, in which case a new entry will be created.
		auto it_combination = bone_combinations.find(BoneCombinationKey(&used_bones));
		if (it_combination == bone_combinations.end())
		{
			UInt32Array* face_list_ptr = new UInt32Array();
			FaceInfluences* influences_ptr = new FaceInfluences(used_bones);
			bone_combinations.insert(BoneCombinationMap::value_type(BoneCombinationKey(influences_ptr), face_list_ptr));

			// Assign face to this combination.
			face_list_ptr->push_back(i);

		} // End if doesn't exist yet
		else
		{
			// Assign face to this combination.
			it_combination->second->push_back(i);

		} // End if already exists

	} // Next Face

	  // We now have a complete list of the unique combinations of bones that
	  // influence every face in the mesh. The next task is to combine these
	  // unique lists into as few combined bone "palettes" as possible, containing
	  // as many bones as possible. To do so, we must continuously search for
	  // face influence combinations that will fit into any existing palettes.

	  // Keep searching until we have consumed all unique face influence combinations.
	for (; bone_combinations.empty() == false;)
	{
		BoneCombinationMap::iterator it_combination;
		BoneCombinationMap::iterator it_best_combination = bone_combinations.end();
		BoneCombinationMap::iterator it_largest_combination = bone_combinations.end();
		int max_common = -1, max_bones = -1;
		int palette_id = -1;

		// Search face influences for the next best combination to add to a palette. We search
		// for two specific properties; A) does it share a large amount in common with an existing 
		// palette and B) does this have the largest list of bones. We'll work out which of these 
		// properties we're interested in later.
		for (it_combination = bone_combinations.begin(); it_combination != bone_combinations.end(); ++it_combination)
		{
			FaceInfluences* influences_ptr = it_combination->first.influences;
			UInt32Array* face_list_ptr = it_combination->second;

			// Record the combination with the largest set of bones in case we can't
			// find a suitable palette to insert any influence into at this point.
			if ((int)influences_ptr->bones.size() > max_bones)
			{
				it_largest_combination = it_combination;
				max_bones = (int)influences_ptr->bones.size();

			} // End if largest

			  // Test against each palette
			for (size_t i = 0; i < _bone_palettes.size(); ++i)
			{
				std::int32_t common_bones, additional_bones, remaining_space;
				auto& palette = _bone_palettes[i];

				// Also compute how the combination might fit into this palette if at all
				palette.compute_palette_fit(influences_ptr->bones, remaining_space, common_bones, additional_bones);

				// Now that we know how many bones this batch of faces has in common with the
				// palette, and how many new bones it will add, we can work out if it's a good
				// idea to use this batch next (assuming it will fit at all).
				if (additional_bones <= remaining_space)
				{
					// Does it have the most in common so far with this palette?
					if (common_bones > max_common)
					{
						max_common = common_bones;
						it_best_combination = it_combination;
						palette_id = (std::int32_t)i;

					} // End if better choice

				} // End if smaller

			} // Next Palette

		} // Next influence combination

		  // If nothing was found then we have run out of influences relevant to the source material
		if (it_largest_combination == bone_combinations.end())
			break;

		// We should hopefully have selected at least one good candidate for insertion
		// into an existing palette that we can now process. If not, we must create
		// a new palette into which we will store the combination with the largest
		// number of bones discovered.
		if (palette_id >= 0)
		{
			FaceInfluences* influences_ptr = it_best_combination->first.influences;
			UInt32Array* face_list_ptr = it_best_combination->second;

			// At least one good combination was found that can be added to an existing palette.
			_bone_palettes[palette_id].assign_bones(influences_ptr->bones, *face_list_ptr);

			// We should now remove the selected combination.
			bone_combinations.erase(it_best_combination);
			checked_delete(influences_ptr);
			checked_delete(face_list_ptr);

		} // End if found fit
		else
		{
			FaceInfluences* influences_ptr = it_largest_combination->first.influences;
			UInt32Array* face_list_ptr = it_largest_combination->second;

			// No combination of face influences was able to fit into an existing palette.
			// We must generate a new one and store the largest combination discovered.
			BonePalette new_palette(palette_size);
			new_palette.set_data_group((std::uint32_t)_bone_palettes.size());
			new_palette.assign_bones(influences_ptr->bones, *face_list_ptr);
			_bone_palettes.push_back(new_palette);

			// We should now remove the selected combination.
			bone_combinations.erase(it_largest_combination);
			checked_delete(influences_ptr);
			checked_delete(face_list_ptr);

		} // End if none found

	} // Next iteration

	  // Bone palettes are now fully constructed. Mesh vertices must now be split as 
	  // necessary in cases where they are shared by faces in alternate palettes.
	for (size_t i = 0; i < _bone_palettes.size(); ++i)
	{
		auto& palette = _bone_palettes[i];
		UInt32Array& faces = palette.get_influenced_faces();
		for (size_t j = 0; j < faces.size(); ++j)
		{

			// Assign face to correct data group.
			std::uint32_t face_index = faces[j];
			tri_data[face_index].data_group_id = palette.get_data_group();

			// Update vertex information
			for (std::uint32_t k = 0; k < 3; ++k)
			{
				auto& data = vertex_table[tri_data[face_index].indices[k]];

				// Record the largest blend index necessary for processing this palette.
				if (((int)data.influences.size() - 1) > palette.get_maximum_blend_index())
					palette.set_maximum_blend_index(std::min<int>(3, ((int)data.influences.size() - 1)));

				// If this vertex has already been assigned to an alternative
				// palette, then it must be duplicated.
				if (data.palette != -1 && data.palette != (std::int32_t)i)
				{
					std::uint32_t new_index = (std::uint32_t)vertex_table.size();

					// Split vertex
					SkinBindData::VertexData pNewVertex(data);
					pNewVertex.palette = (std::int32_t)i;
					vertex_table.push_back(pNewVertex);

					// Update triangle indices
					tri_data[face_index].indices[k] = new_index;

				} // End if already assigned
				else
				{
					// Assign to palette
					data.palette = (std::int32_t)i;

				} // End if not assigned

			} // Next triangle vertex

		} // Next Face

		  // We've finished with the palette's assigned face list. 
		  // This is only a temporary array.
		palette.clear_influenced_faces();

	} // Next Palette Entry

	  // Vertex format must be adjusted to include blend weights and indices 
	  // (if they don't already exist).
	gfx::VertexDecl new_format(_vertex_format);
	gfx::VertexDecl original_format = _vertex_format;
	bool has_weights = new_format.has(gfx::Attrib::Weight);
	bool has_indices = new_format.has(gfx::Attrib::Indices);
	if (!has_weights || !has_indices)
	{
		new_format.m_hash = 0;
		if (!has_weights)
			new_format.add(bgfx::Attrib::Weight, 4, bgfx::AttribType::Float);
		if (!has_indices)
			new_format.add(bgfx::Attrib::Indices, 4, bgfx::AttribType::Int16, false, true);

		new_format.end();
		// Add to format database.
		_vertex_format = new_format;
		// Vertex format was updated.

	} // End if needs new format

	  // Get access to final data offset information.
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Now we need to update the vertex data as required. It's possible
	// that the buffer also needs to grow if / when vertices were split.
	// First convert original data into the new format and also ensure
	// that it is large enough to contain our entire final data set.
	std::uint32_t original_vertex_count = (std::uint32_t)_preparation_data.vertex_count;
	if (_vertex_format.m_hash != original_format.m_hash)
	{
		// Format has changed, run conversion.
		ByteArray original_buffer(_preparation_data.vertex_data);
		_preparation_data.vertex_data.clear();
		_preparation_data.vertex_data.resize(vertex_table.size() * vertex_stride);
		_preparation_data.vertex_flags.resize(vertex_table.size());

		gfx::vertexConvert(_vertex_format, &_preparation_data.vertex_data[0], original_format, &original_buffer[0], original_vertex_count);

	} // End if convert
	else
	{
		// No conversion required, just add space for the new vertices.
		_preparation_data.vertex_data.resize(vertex_table.size() * vertex_stride);
		_preparation_data.vertex_flags.resize(vertex_table.size());

	} // End if !convert

	  // Populate with newly constructed information.
	std::uint8_t* src_vertices_ptr = &_preparation_data.vertex_data[0];
	for (size_t i = 0; i < vertex_table.size(); ++i)
	{
		auto& data = vertex_table[i];

		//// TODO is this correct?
		if (data.palette < 0)
			continue;

		const auto& palette = _bone_palettes[data.palette];

		// If this is a new vertex, duplicate data from original vertex (it will have 
		// already been converted to its final format by this point).
		if (i >= original_vertex_count)
		{
			// This is a new vertex. Duplicate data from original vertex (it will have already been
			// converted to its final format by this point).
			memcpy(src_vertices_ptr + (i * vertex_stride),
				src_vertices_ptr + (data.original_vertex * vertex_stride),
				vertex_stride);

			// Also duplicate additional vertex data.
			_preparation_data.vertex_flags[i] = _preparation_data.vertex_flags[data.original_vertex];

		} // End if new vertex

		  // Assign bone indices (the index to the relevant entry in the palette,
		  // not the main bone list index) and weights.
		math::vec4 blend_weights(0.0f, 0.0f, 0.0f, 0.0f);
		std::uint32_t blend_indices = 0, max_bones = std::min<std::uint32_t>(4, std::uint32_t(data.influences.size()));
		for (std::uint32_t j = 0; j < max_bones; ++j)
		{
			// Store vertex indices and weights
			blend_indices |= (std::uint32_t)((palette.translate_bone_to_palette(data.influences[j]) & 0xFF) << (8 * j));
			blend_weights[j] = data.weights[j];

		} // Next Influence
		for (size_t j = max_bones; j < 4; ++j)
			blend_indices |= (std::uint32_t)(0xFF << (8 * j));

		//vec4 a_weight : BLENDWEIGHT;
		//ivec4 a_indices : BLENDINDICES;
		// Push to vertex.
		gfx::vertexPack(math::value_ptr(blend_weights), false, gfx::Attrib::Weight, _vertex_format, src_vertices_ptr, std::uint32_t(i));

		math::vec4 ind = math::color::u32_to_float4(blend_indices);
		gfx::vertexPack(math::value_ptr(ind), false, gfx::Attrib::Indices, _vertex_format, src_vertices_ptr, std::uint32_t(i));

	} // Next Vertex

	  // Update vertex count to match final size.
	_preparation_data.vertex_count = (std::uint32_t)vertex_table.size();

	vertex_table.clear();

	// Skin is now bound?
	return true;

}

bool Mesh::bind_armature(std::unique_ptr<ArmatureNode>& root)
{
	_root = std::move(root);
	return true;
}

bool Mesh::prepare_mesh(const gfx::VertexDecl& format, bool bRollBackPrepare /* = false */)
{
	// If we are already in the process of preparing, this is a no-op.
	if (_prepare_status == MeshStatus::Preparing)
		return false;

	// Should we roll back an earlier call to 'endPrepare' ?
	if (_prepare_status == MeshStatus::Prepared && bRollBackPrepare == true)
	{
		// Reset required values.
		_preparation_data.triangle_count = 0;
		_preparation_data.triangle_data.clear();
		_preparation_data.vertex_count = 0;
		_preparation_data.vertex_data.clear();
		_preparation_data.vertex_flags.clear();
		_preparation_data.vertex_records.clear();
		_preparation_data.compute_normals = false;
		_preparation_data.compute_binormals = false;
		_preparation_data.compute_tangents = false;

		// We can release the prior prepared triangle data early as this information
		// will be reconstructed from the existing mesh subset table.
		_triangle_data.clear();

		// Release prior hardware buffers if they were constructed.
		_hardware_vb.reset();
		_hardware_ib.reset();

		// Set the size of the preparation buffer so that we can add
		// the existing buffer data to it.
		auto new_stride = format.getStride();
		_preparation_data.vertex_data.resize(_vertex_count * new_stride);
		_preparation_data.vertex_count = _vertex_count;

		// Create enough space in our triangle data array for 
		// the number of faces that existed in the prior final buffer.
		_preparation_data.triangle_data.resize(_face_count);

		// Copy all of the vertex data back into the preparation 
		// structures, converting if necessary.
		if (format.m_hash == _vertex_format.m_hash)
			memcpy(&_preparation_data.vertex_data[0], _system_vb, _preparation_data.vertex_data.size());
		else
			gfx::vertexConvert(format, &_preparation_data.vertex_data[0], _vertex_format, _system_vb, _vertex_count);

		// Clear out the vertex buffer
		checked_array_delete(_system_vb);
		_vertex_count = 0;

		// Iterate through each subset and extract triangle data.
		for (auto subset : _mesh_subsets)
		{
			// Iterate through each face in the subset
			std::uint32_t* current_index_ptr = &_system_ib[(subset->face_start * 3)];
			for (std::int32_t i = 0; i < subset->face_count; ++i, current_index_ptr += 3)
			{
				// Generate winding data
				Triangle& tri = _preparation_data.triangle_data[_preparation_data.triangle_count++];
				tri.data_group_id = subset->data_group_id;
				memcpy(tri.indices, current_index_ptr, 3 * sizeof(std::uint32_t));

			} // Next Face

		} // Next subset

		  // Release additional memory
		checked_array_delete(_system_ib);
		_face_count = 0;

		// Determine which components the original vertex data actually contained.
		bool source_has_normals = _vertex_format.has(gfx::Attrib::Normal);
		bool source_has_binormal = _vertex_format.has(gfx::Attrib::Bitangent);
		bool source_has_tangent = _vertex_format.has(gfx::Attrib::Tangent);

		// The 'PreparationData::vertexFlags' array contains a record of the above for each vertex
		// that currently exists in the preparation buffer. This is required when performing processes 
		// such as the generation of vertex normals, etc.
		std::uint8_t vertex_flags = 0;
		if (source_has_normals)
			vertex_flags |= PreparationData::SourceContainsNormal;
		if (source_has_binormal)
			vertex_flags |= PreparationData::SourceContainsBinormal;
		if (source_has_tangent)
			vertex_flags |= PreparationData::SourceContainsTangent;

		// Record the information.
		_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
		for (std::uint32_t i = 0; i < _preparation_data.vertex_count; ++i)
			_preparation_data.vertex_flags[i] = vertex_flags;

		// If skin binding data was available (i.e. this is a skinned mesh), reverse engineer 
		// the existing data into a newly populated skin binding structure.
		if (_skin_bind_data.has_bones())
		{
			// Clear out any previous influence data.
			_skin_bind_data.clear_vertex_influences();

			// First, retrieve the offsets to the weight and bone palette index data
			// stored in each vertex in the mesh.

			std::uint16_t vertex_stride = _vertex_format.getStride();
			std::uint8_t* vertex_data_ptr = &_preparation_data.vertex_data[0];

			// Search through each bone palette to get influence data.
			SkinBindData::BoneArray& bones = _skin_bind_data.get_bones();
			for (const auto& palette :_bone_palettes)
			{
				// Find the subset associated with this bone palette.
				Subset* subset_ptr = _subset_lookup[MeshSubsetKey(palette.get_data_group())];

				// Process faces in this subset to retrieve referenced vertex data.
				std::int32_t max_blend_index = palette.get_maximum_blend_index();
				UInt32Array bones_references = palette.get_bones();
				for (std::int32_t nFace = subset_ptr->face_start; nFace < (subset_ptr->face_start + subset_ptr->face_count); ++nFace)
				{
					Triangle& tri = _preparation_data.triangle_data[nFace];
					for (size_t i = 0; i < 3; ++i)
					{
						float weights[4];
						gfx::vertexUnpack(weights, gfx::Attrib::Weight, _vertex_format, vertex_data_ptr, tri.indices[i]);
						float indices[4];
						gfx::vertexUnpack(indices, gfx::Attrib::Indices, _vertex_format, vertex_data_ptr, tri.indices[i]);

						float* weights_ptr = weights;
						std::uint32_t ind = math::color::float4_to_u32(math::vec4{ indices[0], indices[1], indices[2], indices[3] });
						std::uint8_t* indices_ptr = (std::uint8_t*)&ind;
	
						// Add influence data back to the referenced bones.
						for (std::int32_t j = 0; j <= max_blend_index; ++j)
						{
							if (indices_ptr[j] != 0xFF)
							{
								auto& bone = bones[bones_references[indices_ptr[j]]];
								bone.influences.push_back(SkinBindData::VertexInfluence(tri.indices[i], weights_ptr[j]));

								// Prevent duplicate insertions
								indices_ptr[j] = 0xFF;

							} // End if valid

						} // Next blend reference

					} // Next triangle index

				} // Next face

			} // Next Palette

		} // End if is skin

		// Clean up heap allocated subset structures
		for (auto subset : _mesh_subsets)
		{
			checked_delete(subset);
		}
		// Reset prepared data arrays and variables.
		_data_groups.clear();
		_mesh_subsets.clear();
		_subset_lookup.clear();
		_hardware_mesh = false;
		_optimize_mesh = false;

	} // End if roll back an earlier prepare
	else if ((_prepare_status != MeshStatus::Preparing && bRollBackPrepare) || !bRollBackPrepare)
	{
		// Clear out anything which is currently loaded in the mesh.
		dispose();

	} // End if not rolling back or no need to roll back

	  // We are in the process of preparing the mesh
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	return true;
}

//-----------------------------------------------------------------------------
//  Name : prepareMesh ()
/// <summary>
/// Prepare the mesh immediately with the specified data.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::prepare_mesh(const gfx::VertexDecl& format, void* vertices_ptr, std::uint32_t vertex_count, const TriangleArray& faces, bool bHardwareCopy /* = true */, bool bWeld /* = true */, bool bOptimize /* = true */)
{
	// Clear out anything which is currently loaded in the mesh.
	dispose();

	// We are in the process of preparing the mesh
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Populate preparation structures.
	_preparation_data.triangle_count = (std::uint32_t)faces.size();
	_preparation_data.triangle_data = faces;
	_preparation_data.vertex_count = vertex_count;

	// Copy vertex data.
	_preparation_data.vertex_data.resize(vertex_count * format.getStride());
	_preparation_data.vertex_flags.resize(vertex_count);
	memset(&_preparation_data.vertex_flags[0], 0, vertex_count);
	memcpy(&_preparation_data.vertex_data[0], vertices_ptr, vertex_count * format.getStride());

	// Generate the bounding box data for the new geometry.
	std::int32_t position_offset = format.getOffset(gfx::Attrib::Position);
	std::int32_t stride = (std::int32_t)format.getStride();
	if (format.has(gfx::Attrib::Position))
	{
		std::uint8_t* src_ptr = ((std::uint8_t*)vertices_ptr) + position_offset;
		for (std::uint32_t i = 0; i < vertex_count; ++i, src_ptr += stride)
			_bbox.add_point(*((math::vec3*)src_ptr));

	} // End if has position

	  // Finish up
	return end_prepare(bHardwareCopy, bWeld, bOptimize);
}

//-----------------------------------------------------------------------------
//  Name : setVertexSource ()
/// <summary>
/// While preparing the mesh, this function should be called in order to
/// specify the source of the vertex buffer to pull data from.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::set_vertex_source(void * source_ptr, std::uint32_t vertex_count, const gfx::VertexDecl& source_format)
{
	// We can only do this if we are in the process of preparing the mesh
	if (_prepare_status != MeshStatus::Preparing)
	{
		auto logger = logging::get("Log");
		logger->error().write("Attempting to set a mesh vertex source without first calling 'prepareMesh' is not allowed.\n");
		return false;

	} // End if not preparing

	  // Clear any existing source information.
	if (_preparation_data.owns_source == true)
		checked_array_delete(_preparation_data.vertex_source);
	_preparation_data.vertex_source = nullptr;
	_preparation_data.source_format = {};
	_preparation_data.owns_source = false;
	_preparation_data.vertex_records.clear();

	// If specifying nullptr (i.e. to clear) then we're done.
	if (source_ptr == nullptr)
		return true;

	// Validate requirements
	if (vertex_count == 0)
		return false;

	// If source format matches the format we're using to prepare
	// then just store the pointer for this vertex source. Otherwise
	// we need to allocate a temporary buffer and convert the data.
	_preparation_data.source_format = source_format;
	if (source_format.m_hash == _vertex_format.m_hash)
	{
		_preparation_data.vertex_source = (std::uint8_t*)source_ptr;

	} // End if matching
	else
	{
		_preparation_data.vertex_source = new std::uint8_t[vertex_count * _vertex_format.getStride()];
		_preparation_data.owns_source = true;
		gfx::vertexConvert(_vertex_format, _preparation_data.vertex_source, source_format, (std::uint8_t*)source_ptr, vertex_count);
	} // End if !matching

	  // Allocate the vertex records for the new vertex buffer
	_preparation_data.vertex_records.clear();
	_preparation_data.vertex_records.resize(vertex_count);

	// Fill with 0xFFFFFFFF initially to indicate that no vertex
	// originally in this location has yet been inserted into the
	// final vertex list.
	memset(&_preparation_data.vertex_records[0], 0xFF, vertex_count * sizeof(std::uint32_t));

	// Some data needs computing? These variables are essentially 'toggles'
	// that are set largely so that we can early out if it was NEVER necessary 
	// to generate these components (i.e. not one single vertex needed it).
	if (!source_format.has(gfx::Attrib::Normal) && _vertex_format.has(gfx::Attrib::Normal))
		_preparation_data.compute_normals = true;
	if (!source_format.has(gfx::Attrib::Bitangent) && _vertex_format.has(gfx::Attrib::Bitangent))
		_preparation_data.compute_binormals = true;
	if (!source_format.has(gfx::Attrib::Tangent) && _vertex_format.has(gfx::Attrib::Tangent))
		_preparation_data.compute_tangents = true;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : addPrimitives ()
/// <summary>
/// Called by an external source (such as the geometry loader) in order
/// to populate the internal buffers ready for building the mesh.
/// Note : This may be called multiple times until the mesh is ready to be built
/// via the 'endPrepare' method.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::add_primitives(const TriangleArray & aTriangles)
{
	std::uint32_t orig_index, index;

	// We can only do this if we are in the process of preparing the mesh
	if (_prepare_status != MeshStatus::Preparing)
	{
		auto logger = logging::get("Log");
		logger->error().write("Attempting to add primitives to a mesh without first calling 'prepareMesh' is not allowed.\n");
		return false;

	} // End if not preparing

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// During the construction process we test to see if any specified
	// vertex normal contains invalid data. If the original source vertex
	// data did not contain a normal, we can optimize and skip this step.
	bool source_has_normals = _preparation_data.source_format.has(gfx::Attrib::Normal);
	bool source_has_binormal = _preparation_data.source_format.has(gfx::Attrib::Bitangent);
	bool source_has_tangent = _preparation_data.source_format.has(gfx::Attrib::Tangent);

	// In addition, we also record which of the required components each
	// vertex actually contained based on the following information.
	std::uint8_t vertex_flags = 0;
	if (source_has_normals == true)
		vertex_flags |= PreparationData::SourceContainsNormal;
	if (source_has_binormal == true)
		vertex_flags |= PreparationData::SourceContainsBinormal;
	if (source_has_tangent == true)
		vertex_flags |= PreparationData::SourceContainsTangent;

	// Loop through the specified faces and process them.
	std::uint8_t* src_vertices_ptr = _preparation_data.vertex_source;
	std::uint32_t face_count = (std::uint32_t)aTriangles.size();
	for (std::uint32_t i = 0; i < face_count; ++i)
	{
		const Triangle& src_tri = aTriangles[i];

		// Retrieve vertex positions (if there are any) so that we can perform degenerate testing.
		if (has_position)
		{
			math::vec3 v1;
			float vf1[4];
			gfx::vertexUnpack(vf1, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, src_tri.indices[0]);
			math::vec3 v2;
			float vf2[4];
			gfx::vertexUnpack(vf2, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, src_tri.indices[1]);
			math::vec3 v3;
			float vf3[4];
			gfx::vertexUnpack(vf3, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, src_tri.indices[2]);
			memcpy(&v1[0], vf1, 3 * sizeof(float));
			memcpy(&v2[0], vf2, 3 * sizeof(float));
			memcpy(&v3[0], vf3, 3 * sizeof(float));

			// Skip triangle if it is degenerate.
			if ((math::epsilonEqual(v1, v2, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}) ||
				(math::epsilonEqual(v1, v3, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}) ||
				(math::epsilonEqual(v2, v3, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}))
				continue;
		} // End if has position.

		  // Prepare a triangle structure ready for population
		_preparation_data.triangle_count++;
		_preparation_data.triangle_data.resize(_preparation_data.triangle_count);
		Triangle& triangle_data = _preparation_data.triangle_data[_preparation_data.triangle_count - 1];

		// Set triangle's subset information.
		triangle_data.data_group_id = src_tri.data_group_id;

		// For each index in the face
		for (std::uint32_t j = 0; j < 3; ++j)
		{
			// Extract the original index from the specified index buffer
			orig_index = src_tri.indices[j];

			// Retrieve the vertex record for the original vertex
			index = _preparation_data.vertex_records[orig_index];

			// Have we inserted this vertex into the vertex buffer previously?
			if (index == 0xFFFFFFFF)
			{
				// Vertex does not yet exist in the vertex buffer we are preparing
				// so copy the vertex in and record the index mapping for this vertex.
				index = _preparation_data.vertex_count++;
				_preparation_data.vertex_records[orig_index] = index;

				// Resize the output vertex buffer ready to hold this new data.
				size_t nInitialSize = _preparation_data.vertex_data.size();
				_preparation_data.vertex_data.resize(nInitialSize + vertex_stride);

				// Copy the data in.
				std::uint8_t * src_ptr = src_vertices_ptr + (orig_index * vertex_stride);
				std::uint8_t * pDst = &_preparation_data.vertex_data[nInitialSize];
				memcpy(pDst, src_ptr, vertex_stride);

				// Also record other pertenant details about this vertex.
				_preparation_data.vertex_flags.push_back(vertex_flags);

				// Clear any invalid normals (completely messes up HDR if ANY NaNs make it this far)
				if (has_normal && source_has_normals == true)
				{
					float fnorm[4];
					gfx::vertexUnpack(fnorm, gfx::Attrib::Normal, _vertex_format, pDst);
					if (_isnan(fnorm[0]) || _isnan(fnorm[1]) || _isnan(fnorm[2]))
						gfx::vertexPack(fnorm, true, gfx::Attrib::Normal, _vertex_format, pDst);

				} // End if have normal

				  // Grow the size of the bounding box
				if (has_position)
				{
					float fpos[4];
					gfx::vertexUnpack(fpos, gfx::Attrib::Position, _vertex_format, pDst);
					_bbox.add_point(math::vec3(fpos[0], fpos[1], fpos[2]));
				}

			} // End if vertex not recorded in this buffer yet

			  // Copy the index in
			triangle_data.indices[j] = index;

		} // Next Index

	} // Next Face

	  // Success!
	return true;
}


//-----------------------------------------------------------------------------
//  Name : createCylinder ()
/// <summary>
/// Create cylinder geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_cylinder(const gfx::VertexDecl& format, float fRadius, float fHeight, std::uint32_t nStacks, std::uint32_t nSlices, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	math::vec3 current_pos, vNormal;
	math::vec2 current_tex;

	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_texcoord = _vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Compute the number of faces and vertices that will be required for this box
	_preparation_data.triangle_count = (nSlices * nStacks) * 2;
	_preparation_data.vertex_count = (nSlices + 1) * (nStacks + 1);

	// Add vertices and faces for caps
	std::uint32_t caps_start = _preparation_data.vertex_count;
	_preparation_data.vertex_count += nSlices * 2;
	_preparation_data.triangle_count += (nSlices - 2) * 2;

	// Allocate enough space for the new vertex and triangle data
	_preparation_data.vertex_data.resize(_preparation_data.vertex_count * vertex_stride);
	_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
	_preparation_data.triangle_data.resize(_preparation_data.triangle_count);

	// For each stack
	std::uint8_t* pCurrentVertex = &_preparation_data.vertex_data[0];
	std::uint8_t* pCurrentFlags = &_preparation_data.vertex_flags[0];
	for (std::uint32_t stack = 0; stack <= nStacks; ++stack)
	{
		// Generate a ring of vertices which describe the top edges of that stack's geometry
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t slice = 0; slice <= nSlices; ++slice)
		{
			// Precompute any reusable values
			float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;

			// Compuse vertex normal at this location around the cylinder.
			vNormal.x = math::sin(a);
			vNormal.y = 0;
			vNormal.z = math::cos(a);

			// Position is simply a scaled version of the normal
			// with the correctly computed height.
			current_pos.x = vNormal.x * fRadius;
			current_pos.y = (float)stack * (fHeight / (float)nStacks);
			current_pos.z = vNormal.z * fRadius;

			// Compute the texture coordinate.
			current_tex.x = (1.0f / (float)nSlices) * (float)slice;
			current_tex.y = (1.0f / (float)nStacks) * (float)stack;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Center)
				current_pos.y -= fHeight * 0.5f;
			else if (Origin == MeshCreateOrigin::Top)
				current_pos.y -= fHeight;

			// Should we invert the vertex normal
			if (bInverted)
				vNormal = -vNormal;

			// Store!
			// Store vertex components
			if (has_position)
				gfx::vertexPack(&current_pos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&current_tex[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(current_pos);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;

		} // Next Slice

	} // Next Stack

	  // Now cmpute the vertices for the base cylinder cap geometry.
	for (std::uint32_t slice = 0; slice < nSlices; ++slice)
	{
		// Precompute any reusable values
		float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;

		// Compute the vertex components.
		current_pos = math::vec3(math::sin(a), 0, math::cos(a));
		vNormal = (fHeight >= 0) ? math::vec3(0, -1, 0) : math::vec3(0, 1, 0);
		current_tex.x = (current_pos.x * 0.5f) + 0.5f;
		current_tex.y = (current_pos.z * 0.5f) + 0.5f;
		current_pos.x *= fRadius;
		current_pos.z *= fRadius;

		// Position in center or at base/tip?
		if (Origin == MeshCreateOrigin::Center)
			current_pos.y -= fHeight * 0.5f;
		else if (Origin == MeshCreateOrigin::Top)
			current_pos.y -= fHeight;

		// Should we invert the vertex normal
		if (bInverted)
			vNormal = -vNormal;

		// Store!
		if (has_position)
			gfx::vertexPack(&current_pos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
		if (has_normal)
			gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
		if (has_texcoord)
			gfx::vertexPack(&current_tex[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

		// Set flags for this vertex (we want to generate tangents 
		// and binormals if we need them).
		*pCurrentFlags++ = PreparationData::SourceContainsNormal;

		// Grow the object space bounding box for this mesh
		// by including the computed position.
		_bbox.add_point(current_pos);

		// Move on to next vertex
		pCurrentVertex += vertex_stride;

	} // Next Slice

	  // And the vertices for the end cylinder cap geometry.
	for (std::uint32_t slice = 0; slice < nSlices; ++slice)
	{
		// Precompute any reusable values
		float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;

		// Compute the vertex components.
		current_pos = math::vec3(math::sin(a), fHeight, math::cos(a));
		vNormal = (fHeight >= 0) ? math::vec3(0, 1, 0) : math::vec3(0, -1, 0);
		current_tex.x = (current_pos.x * -0.5f) + 0.5f;
		current_tex.y = (current_pos.z * -0.5f) + 0.5f;
		current_pos.x *= fRadius;
		current_pos.z *= fRadius;

		// Position in center or at base/tip?
		if (Origin == MeshCreateOrigin::Center)
			current_pos.y -= fHeight * 0.5f;
		else if (Origin == MeshCreateOrigin::Top)
			current_pos.y -= fHeight;

		// Should we invert the vertex normal
		if (bInverted)
			vNormal = -vNormal;

		// Store!
		if (has_position)
			gfx::vertexPack(&current_pos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
		if (has_normal)
			gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
		if (has_texcoord)
			gfx::vertexPack(&current_tex[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

		// Set flags for this vertex (we want to generate tangents 
		// and binormals if we need them).
		*pCurrentFlags++ = PreparationData::SourceContainsNormal;

		// Grow the object space bounding box for this mesh
		// by including the computed position.
		_bbox.add_point(current_pos);

		// Move on to next vertex
		pCurrentVertex += vertex_stride;

	} // Next Slice


	  // Now compute the indices. For each stack (except the top and bottom)
	Triangle * pCurrentTriangle = &_preparation_data.triangle_data[0];
	for (std::uint32_t stack = 0; stack < nStacks; ++stack)
	{
		// Generate two triangles for the quad on each slice
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			// If height was negative (i.e. faces are inverted)
			// we need to flip the order of the indices
			if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle++;

			} // End if inverted
			else
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[2] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle++;

			} // End if not inverted

		} // Next Slice

	} // Next Stack

	  // Add cylinder cap geometry
	for (std::uint32_t slice = 0; slice < nSlices - 2; ++slice)
	{
		// If height was negative (i.e. faces are inverted)
		// we need to flip the order of the indices
		if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
		{
			// Base Cap
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = caps_start;
			pCurrentTriangle->indices[1] = caps_start + slice + 1;
			pCurrentTriangle->indices[2] = caps_start + slice + 2;
			pCurrentTriangle++;

			// End Cap
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = caps_start + nSlices + slice + 2;
			pCurrentTriangle->indices[1] = caps_start + nSlices + slice + 1;
			pCurrentTriangle->indices[2] = caps_start + nSlices;
			pCurrentTriangle++;

		} // End if inverted
		else
		{
			// Base Cap
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = caps_start + slice + 2;
			pCurrentTriangle->indices[1] = caps_start + slice + 1;
			pCurrentTriangle->indices[2] = caps_start;
			pCurrentTriangle++;

			// End Cap
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = caps_start + nSlices;
			pCurrentTriangle->indices[1] = caps_start + nSlices + slice + 1;
			pCurrentTriangle->indices[2] = caps_start + nSlices + slice + 2;
			pCurrentTriangle++;

		} // End if not inverted

	} // Next Slice

	// We need to generate binormals / tangents?
	_preparation_data.compute_binormals = _vertex_format.has(gfx::Attrib::Bitangent);
	_preparation_data.compute_tangents = _vertex_format.has(gfx::Attrib::Tangent);

	// Finish up
	return end_prepare(bHardwareCopy, false, false);
}

//-----------------------------------------------------------------------------
//  Name : createCapsule ()
/// <summary>
/// Create capsule geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_capsule(const gfx::VertexDecl& format, float fRadius, float fHeight, std::uint32_t nStacks, std::uint32_t nSlices, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	math::vec3 current_pos, vNormal;
	math::vec2 current_tex;

	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_texcoord = _vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Compute the number of 'stacks' required for the hemisphere caps.
	// This must be the closest multiple of 2 to ensure a valid center division.
	std::uint32_t nSphereStacks = ((nSlices / 2) + 1) & 0xFFFFFFFE;
	if (nSphereStacks < 2) nSphereStacks = 2;

	// Height must be at least equal to radius * 2 (to account for the hemispheres)
	bool bNegateY = (fHeight < 0);
	fRadius = math::abs(fRadius);
	fHeight = math::abs(fHeight);
	fHeight = std::max<float>(fRadius * 2.0f, fHeight);
	float fCylinderHeight = fHeight - (fRadius * 2.0f);
	if (bNegateY)
	{
		fHeight = -fHeight;
		fCylinderHeight = -fCylinderHeight;

	} // End if negated.

	  // Add vertices for the first hemisphere. The cap shares a common row of vertices with the capsule sides.
	_preparation_data.triangle_count = (nSlices * (nSphereStacks / 2)) * 2;
	_preparation_data.vertex_count = (nSlices + 1) * ((nSphereStacks / 2) + 1);

	// Cylinder geometry starts at the last row of the first hemisphere.
	std::uint32_t nCylinderStart = _preparation_data.vertex_count - (nSlices + 1);

	// Compute the number of faces and vertices that will be required by the
	// cylinder shape that exists between the two hemispheres.
	std::uint32_t nCylinderVerts = (nSlices + 1) * (nStacks - 1);
	_preparation_data.triangle_count += (nSlices * nStacks) * 2;
	_preparation_data.vertex_count += nCylinderVerts;

	// Add vertices for the bottom hemisphere. The cap shares a common row of vertices with the capsule sides.
	_preparation_data.triangle_count += (nSlices * (nSphereStacks / 2)) * 2;
	_preparation_data.vertex_count += (nSlices + 1) * ((nSphereStacks / 2) + 1);

	// Allocate enough space for the new vertex and triangle data
	_preparation_data.vertex_data.resize(_preparation_data.vertex_count * vertex_stride);
	_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
	_preparation_data.triangle_data.resize(_preparation_data.triangle_count);

	// First add the top hemisphere
	std::uint8_t * pCurrentVertex = &_preparation_data.vertex_data[0];
	std::uint8_t * pCurrentFlags = &_preparation_data.vertex_flags[0];
	std::int32_t nVerticesAdded = 0;
	for (std::uint32_t stack = 0; stack <= nSphereStacks / 2; ++stack)
	{
		// Generate a ring of vertices which describe the top edges of that stack's geometry
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t slice = 0; slice <= nSlices; ++slice)
		{
			// Precompute any reusable values
			float  a = (math::pi<float>() / (float)nSphereStacks) * (float)stack;
			float  b = (math::two_pi<float>() / (float)nSlices) * (float)slice;
			float xz = math::sin(a);

			// Compute the normal & position of the vertex
			vNormal.x = xz * math::sin(b);
			vNormal.y = math::cos((bNegateY) ? math::pi<float>() - a : a);
			vNormal.z = xz * math::cos(b);
			current_pos = vNormal * fRadius;

			// Offset so that it sits at the top of the central cylinder
			current_pos.y += fCylinderHeight * 0.5f;

			// Compute the texture coordinate.
			current_tex.x = (1.0f / (float)nSlices) * (float)slice;
			current_tex.y = (current_pos.y + (fHeight * 0.5f)) / fHeight;

			// Invert normal if required
			if (bInverted)
				vNormal = -vNormal;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Bottom)
				current_pos.y += fHeight * 0.5f;
			else if (Origin == MeshCreateOrigin::Top)
				current_pos.y -= fHeight * 0.5f;

			// Store vertex components
			if (has_position)
				gfx::vertexPack(&current_pos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&current_tex[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(current_pos);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;
			nVerticesAdded++;

		} // Next Slice

	} // Next Stack

	  // Create cylinder side vertices. We don't generate
	  // vertices for the top/bottom row -- these were added by the 
	  // hemispheres.
	for (std::uint32_t stack = 1; stack < nStacks; ++stack)
	{
		// Generate a ring of vertices which describe the top edges of that stack's geometry
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t slice = 0; slice <= nSlices; ++slice)
		{
			// Precompute any reusable values
			float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;

			// Compuse vertex normal at this location around the cylinder.
			vNormal.x = math::sin(a);
			vNormal.y = 0;
			vNormal.z = math::cos(a);

			// Position is simply a scaled version of the normal
			// with the correctly computed height.
			current_pos.x = vNormal.x * fRadius;
			current_pos.y = (fCylinderHeight - ((float)stack * (fCylinderHeight / (float)nStacks)));
			current_pos.y -= (fCylinderHeight * 0.5f);
			current_pos.z = vNormal.z * fRadius;

			// Compute the texture coordinate.
			current_tex.x = (1.0f / (float)nSlices) * (float)slice;
			current_tex.y = (current_pos.y + (fHeight * 0.5f)) / fHeight;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Bottom)
				current_pos.y += fHeight * 0.5f;
			else if (Origin == MeshCreateOrigin::Top)
				current_pos.y -= fHeight * 0.5f;

			// Should we invert the vertex normal
			if (bInverted)
				vNormal = -vNormal;

			// Store!
			if (has_position)
				gfx::vertexPack(&current_pos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&current_tex[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(current_pos);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;
			nVerticesAdded++;

		} // Next Slice

	} // Next Stack

	  // Now the bottom hemisphere
	for (std::uint32_t stack = nSphereStacks / 2; stack <= nSphereStacks; ++stack)
	{
		// Generate a ring of vertices which describe the top edges of that stack's geometry
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t slice = 0; slice <= nSlices; ++slice)
		{
			// Precompute any reusable values
			float  a = (math::pi<float>() / (float)nSphereStacks) * (float)stack;
			float  b = (math::two_pi<float>() / (float)nSlices) * (float)slice;
			float xz = math::sin(a);

			// Compute the normal & position of the vertex
			vNormal.x = xz * math::sin(b);
			vNormal.y = math::cos((bNegateY) ? math::pi<float>() - a : a);
			vNormal.z = xz * math::cos(b);
			current_pos = vNormal * fRadius;

			// Offset so that it sits at the bottom of the central cylinder
			current_pos.y -= fCylinderHeight * 0.5f;

			// Compute the texture coordinate.
			current_tex.x = (1.0f / (float)nSlices) * (float)slice;
			current_tex.y = (current_pos.y + (fHeight * 0.5f)) / fHeight;

			// Invert normal if required
			if (bInverted)
				vNormal = -vNormal;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Bottom)
				current_pos.y += fHeight * 0.5f;
			else if (Origin == MeshCreateOrigin::Top)
				current_pos.y -= fHeight * 0.5f;

			// Store vertex components
			if (has_position)
				gfx::vertexPack(&current_pos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&current_tex[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(current_pos);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;
			nVerticesAdded++;

		} // Next Slice

	} // Next Stack

	  // Now generate indices for the top hemisphere first.
	std::int32_t nTrianglesAdded = 0;
	Triangle * pCurrentTriangle = &_preparation_data.triangle_data[0];
	for (std::uint32_t stack = 0; stack < nSphereStacks / 2; ++stack)
	{
		// Generate two triangles for the quad on each slice
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			// If height was negative (i.e. faces are inverted)
			// we need to flip the order of the indices
			if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[2] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle++;

				nTrianglesAdded += 2;

			} // End if inverted
			else
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle++;

				nTrianglesAdded += 2;

			} // End if !inverted

		} // Next Slice

	} // Next Stack

	  // Cylinder stacks.
	for (std::uint32_t stack = 0; stack < nStacks; ++stack)
	{
		// Generate two triangles for the quad on each slice
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			// If height was negative (i.e. faces are inverted)
			// we need to flip the order of the indices
			if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderStart + ((stack*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[1] = nCylinderStart + (((stack + 1)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[2] = nCylinderStart + ((stack*(nSlices + 1)) + slice);
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderStart + ((stack*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[1] = nCylinderStart + (((stack + 1)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[2] = nCylinderStart + (((stack + 1)*(nSlices + 1)) + slice);
				pCurrentTriangle++;

				nTrianglesAdded += 2;

			} // End if inverted
			else
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderStart + ((stack*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[1] = nCylinderStart + (((stack + 1)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[2] = nCylinderStart + ((stack*(nSlices + 1)) + slice + 1);
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderStart + (((stack + 1)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[1] = nCylinderStart + (((stack + 1)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[2] = nCylinderStart + ((stack*(nSlices + 1)) + slice + 1);
				pCurrentTriangle++;

				nTrianglesAdded += 2;

			} // End if not inverted

		} // Next Slice

	} // Next Stack

	  // Finally, the indices for the bottom hemisphere.
	for (std::uint32_t stack = nSphereStacks / 2; stack < nSphereStacks; ++stack)
	{
		// Generate two triangles for the quad on each slice
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			// If height was negative (i.e. faces are inverted)
			// we need to flip the order of the indices
			if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderVerts + (((stack + 1)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[1] = nCylinderVerts + (((stack + 2)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[2] = nCylinderVerts + (((stack + 1)*(nSlices + 1)) + slice);
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderVerts + (((stack + 1)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[1] = nCylinderVerts + (((stack + 2)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[2] = nCylinderVerts + (((stack + 2)*(nSlices + 1)) + slice);
				pCurrentTriangle++;

				nTrianglesAdded += 2;

			} // End if inverted
			else
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderVerts + (((stack + 1)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[1] = nCylinderVerts + (((stack + 2)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[2] = nCylinderVerts + (((stack + 1)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = nCylinderVerts + (((stack + 2)*(nSlices + 1)) + slice);
				pCurrentTriangle->indices[1] = nCylinderVerts + (((stack + 2)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle->indices[2] = nCylinderVerts + (((stack + 1)*(nSlices + 1)) + slice + 1);
				pCurrentTriangle++;

				nTrianglesAdded += 2;

			} // End if !inverted

		} // Next Slice

	} // Next Stack

	// We need to generate binormals / tangents?
	_preparation_data.compute_binormals = _vertex_format.has(gfx::Attrib::Bitangent);
	_preparation_data.compute_tangents = _vertex_format.has(gfx::Attrib::Tangent);

	// Finish up
	return end_prepare(bHardwareCopy, false, false);
}

//-----------------------------------------------------------------------------
//  Name : createSphere ()
/// <summary>
/// Create sphere geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_sphere(const gfx::VertexDecl& format, float fRadius, std::uint32_t nStacks, std::uint32_t nSlices, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	math::vec3 vec_position, vec_normal;

	// Clear out old data.
	dispose();

	// Inverting?
	if (bInverted)
		fRadius = -fRadius;

	// We are in the process of preparing.
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_texcoord = _vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Compute the number of faces and vertices that will be required for this sphere
	_preparation_data.triangle_count = (nSlices * nStacks) * 2;
	_preparation_data.vertex_count = (nSlices + 1) * (nStacks + 1);

	// Allocate enough space for the new vertex and triangle data
	_preparation_data.vertex_data.resize(_preparation_data.vertex_count * vertex_stride);
	_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
	_preparation_data.triangle_data.resize(_preparation_data.triangle_count);

	// For each stack
	std::uint8_t * pCurrentVertex = &_preparation_data.vertex_data[0];
	std::uint8_t * pCurrentFlags = &_preparation_data.vertex_flags[0];
	for (std::uint32_t stack = 0; stack <= nStacks; ++stack)
	{
		// Generate a ring of vertices which describe the top edges of that stack's geometry
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t slice = 0; slice <= nSlices; ++slice)
		{
			// Precompute any reusable values
			float  a = (math::pi<float>() / (float)nStacks) * (float)stack;
			float  b = (math::two_pi<float>() / (float)nSlices) * (float)slice;
			float xz = math::sin(a);

			// Compute the normal & position of the vertex
			vec_normal.x = xz * math::sin(b);
			vec_normal.y = math::cos(a);
			vec_normal.z = xz * math::cos(b);
			vec_position = vec_normal * fRadius;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Bottom)
				vec_position.y += math::abs(fRadius);
			else if (Origin == MeshCreateOrigin::Top)
				vec_position.y -= math::abs(fRadius);

			// Store vertex components
			if (has_position)
				gfx::vertexPack(&vec_position[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vec_normal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&math::vec2((1 / (float)nSlices) * (float)slice, (1 / (float)nStacks) * (float)stack)[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(vec_position);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;

		} // Next Slice

	} // Next Stack


	  // Now generate indices. Process each stack (except the top and bottom)
	Triangle * pCurrentTriangle = &_preparation_data.triangle_data[0];
	for (std::uint32_t stack = 0; stack < nStacks; ++stack)
	{
		// Generate two triangles for the quad on each slice
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice;
			pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
			pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
			pCurrentTriangle++;

			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = ((stack + 1)*(nSlices + 1)) + slice;
			pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
			pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
			pCurrentTriangle++;

		} // Next Slice

	} // Next Stack

	  // We need to generate binormals / tangents?
	_preparation_data.compute_binormals = _vertex_format.has(gfx::Attrib::Bitangent);
	_preparation_data.compute_tangents = _vertex_format.has(gfx::Attrib::Tangent);

	// Finish up
	return end_prepare(bHardwareCopy, false, false);
}

//-----------------------------------------------------------------------------
//  Name : createTorus ()
/// <summary>
/// Create torus geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_torus(const gfx::VertexDecl& format, float fOuterRadius, float fInnerRadius, std::uint32_t nBands, std::uint32_t nSides, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	math::vec3 vPosition, vNormal, vCenter;
	math::vec2 vTexCoord;

	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_texcoord = _vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Compute the number of faces and vertices that will be required for this torus
	_preparation_data.triangle_count = (nBands * nSides) * 2;
	_preparation_data.vertex_count = (nBands + 1) * (nSides + 1);

	// Allocate enough space for the new vertex and triangle data
	_preparation_data.vertex_data.resize(_preparation_data.vertex_count * vertex_stride);
	_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
	_preparation_data.triangle_data.resize(_preparation_data.triangle_count);

	// The radius of a circle running through the core of the torus interior
	float fCoreRadius = (fInnerRadius + fOuterRadius) / 2.0f;
	float fBandRadius = (fOuterRadius - fInnerRadius) / 2.0f;

	// Generate vertex data. For each band (around the outside)
	std::uint8_t * pCurrentVertex = &_preparation_data.vertex_data[0];
	std::uint8_t * pCurrentFlags = &_preparation_data.vertex_flags[0];
	for (std::uint32_t nBand = 0; nBand <= nBands; ++nBand)
	{
		// Precompute any re-usable values
		float a = (math::two_pi<float>() / (float)nBands) * nBand;
		float sinBand = math::sin(a);
		float cosBand = math::cos(a);

		// Compute the center point for this band (imagine drawing a circle through the torus interior.)
		vCenter = math::vec3(sinBand * fCoreRadius, 0, cosBand * fCoreRadius);

		// Position in center or at base/tip?
		if (Origin == MeshCreateOrigin::Bottom)
			vCenter.y += math::abs(fBandRadius);
		else if (Origin == MeshCreateOrigin::Top)
			vCenter.y -= math::abs(fBandRadius);

		// Generate a ring of vertices that wrap around this core point.
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t nSide = 0; nSide <= nSides; ++nSide)
		{
			// Precompute any re-usable values
			float b = (math::two_pi<float>() / (float)nSides) * nSide;
			float c = math::sin(b) * fBandRadius;

			// Compute the vertex components
			vPosition.x = vCenter.x + (c * sinBand);
			vPosition.y = vCenter.y + (math::cos(b) * fBandRadius);
			vPosition.z = vCenter.z + (c * cosBand);
			vNormal = math::normalize(vPosition - vCenter);
			vTexCoord = math::vec2((1 / (float)nBands) * (float)nBand, (1 / (float)nSides) * (float)nSide);

			// Inverting?
			if (bInverted)
				vNormal = -vNormal;

			// Store!
			if (has_position)
				gfx::vertexPack(&vPosition[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&vTexCoord[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(vPosition);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;

		} // Next Slice

	} // Next Stack

	  // Now generate indices. For each band.
	Triangle * pCurrentTriangle = &_preparation_data.triangle_data[0];
	for (std::uint32_t nBand = 0; nBand < nBands; ++nBand)
	{
		// Generate two triangles for the quad on each side
		for (std::uint32_t nSide = 0; nSide < nSides; ++nSide)
		{
			if (!bInverted)
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (nBand*(nSides + 1)) + nSide + 1;
				pCurrentTriangle->indices[1] = ((nBand + 1)*(nSides + 1)) + nSide;
				pCurrentTriangle->indices[2] = (nBand*(nSides + 1)) + nSide;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (nBand*(nSides + 1)) + nSide + 1;
				pCurrentTriangle->indices[1] = ((nBand + 1)*(nSides + 1)) + nSide + 1;
				pCurrentTriangle->indices[2] = ((nBand + 1)*(nSides + 1)) + nSide;
				pCurrentTriangle++;

			} // End if !inverted
			else
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (nBand*(nSides + 1)) + nSide;
				pCurrentTriangle->indices[1] = ((nBand + 1)*(nSides + 1)) + nSide;
				pCurrentTriangle->indices[2] = (nBand*(nSides + 1)) + nSide + 1;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = ((nBand + 1)*(nSides + 1)) + nSide;
				pCurrentTriangle->indices[1] = ((nBand + 1)*(nSides + 1)) + nSide + 1;
				pCurrentTriangle->indices[2] = (nBand*(nSides + 1)) + nSide + 1;
				pCurrentTriangle++;

			} // End if inverted

		} // Next Side

	} // Next Band

	  // We need to generate binormals / tangents?
	_preparation_data.compute_binormals = _vertex_format.has(gfx::Attrib::Bitangent);
	_preparation_data.compute_tangents = _vertex_format.has(gfx::Attrib::Tangent);

	// Finish up
	return end_prepare(bHardwareCopy, false, false);
}


//-----------------------------------------------------------------------------
//  Name : createCone ()
/// <summary>
/// Create cone geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_cone(const gfx::VertexDecl& format, float fRadius, float fRadiusTip, float fHeight, std::uint32_t nStacks, std::uint32_t nSlices, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	math::vec3 vec_position, vec_normal;
	math::vec2 vec_tex_coords;

	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_texcoord = _vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Compute the number of faces and vertices that will be required for this cone
	_preparation_data.triangle_count = (nSlices * nStacks) * 2;
	_preparation_data.vertex_count = (nSlices + 1) * (nStacks + 1);

	// Add vertices and faces for caps
	if (fRadiusTip < 0.001f) fRadiusTip = 0.0f;
	std::uint32_t num_caps = (fRadiusTip > 0.0f) ? 2 : 1;
	std::uint32_t caps_start = _preparation_data.vertex_count;
	_preparation_data.vertex_count += nSlices * num_caps;
	_preparation_data.triangle_count += (nSlices - 2) * num_caps;

	// Allocate enough space for the new vertex and triangle data
	_preparation_data.vertex_data.resize(_preparation_data.vertex_count * vertex_stride);
	_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
	_preparation_data.triangle_data.resize(_preparation_data.triangle_count);

	// Generate vertex data. For each stack
	std::uint8_t * pCurrentVertex = &_preparation_data.vertex_data[0];
	std::uint8_t * pCurrentFlags = &_preparation_data.vertex_flags[0];
	for (std::uint32_t stack = 0; stack <= nStacks; ++stack)
	{
		// Generate a ring of vertices which describe the top edges of that stack's geometry
		// The last vertex is a duplicate of the first to ensure we have correct texturing
		for (std::uint32_t slice = 0; slice <= nSlices; ++slice)
		{
			// Precompute any reusable values
			float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;
			float b = fRadius + ((fRadiusTip - fRadius) * ((float)stack / (float)nStacks));

			// Compute the vertex components
			vec_position.x = math::sin(a);
			vec_position.y = (float)stack * (fHeight / (float)nStacks);
			vec_position.z = math::cos(a);
			vec_normal = math::vec3(vec_position.x, 0.0f, vec_position.z);
			vec_position.x *= b;
			vec_position.z *= b;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Center)
				vec_position.y -= fHeight * 0.5f;
			else if (Origin == MeshCreateOrigin::Top)
				vec_position.y -= fHeight;

			// Inverting the normal?
			if (bInverted)
				vec_normal = -vec_normal;

			// Store vertex components
			if (has_position)
				gfx::vertexPack(&vec_position[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vec_normal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&math::vec2((1 / (float)nSlices) * (float)slice, (1 / (float)nStacks) * (float)stack)[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(vec_position);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;

		} // Next Slice

	} // Next Stack

	  // Now cmpute the vertices for the base cylinder cap geometry.
	for (std::uint32_t slice = 0; slice < nSlices; ++slice)
	{
		// Precompute any reusable values
		float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;

		// Compute the vertex components
		vec_position = math::vec3(math::sin(a), 0, math::cos(a));
		vec_normal = (fHeight >= 0) ? math::vec3(0, -1, 0) : math::vec3(0, 1, 0);
		vec_tex_coords = math::vec2((vec_position.x * 0.5f) + 0.5f, (vec_position.z * 0.5f) + 0.5f);
		vec_position.x *= fRadius;
		vec_position.z *= fRadius;

		// Position in center or at base/tip?
		if (Origin == MeshCreateOrigin::Center)
			vec_position.y -= fHeight * 0.5f;
		else if (Origin == MeshCreateOrigin::Top)
			vec_position.y -= fHeight;

		// Inverting the normal?
		if (bInverted)
			vec_normal = -vec_normal;

		// Store vertex components
		if (has_position)
			gfx::vertexPack(&vec_position[0], false, gfx::Attrib::Position, format, pCurrentVertex);
		if (has_normal)
			gfx::vertexPack(&vec_normal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
		if (has_texcoord)
			gfx::vertexPack(&vec_tex_coords[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

		// Set flags for this vertex (we want to generate tangents 
		// and binormals if we need them).
		*pCurrentFlags++ = PreparationData::SourceContainsNormal;

		// Grow the object space bounding box for this mesh
		// by including the computed position.
		_bbox.add_point(vec_position);

		// Move on to next vertex
		pCurrentVertex += vertex_stride;

	} // Next Slice

	  // And the vertices for the end cylinder cap geometry.
	if (num_caps > 1)
	{
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			// Precompute any reusable values
			float a = (math::two_pi<float>() / (float)nSlices) * (float)slice;

			// Compute the vertex components
			vec_position = math::vec3(math::sin(a), fHeight, math::cos(a));
			vec_normal = (fHeight >= 0) ? math::vec3(0, 1, 0) : math::vec3(0, -1, 0);
			vec_tex_coords = math::vec2((vec_position.x * -0.5f) + 0.5f, (vec_position.z * -0.5f) + 0.5f);
			vec_position.x *= fRadiusTip;
			vec_position.z *= fRadiusTip;

			// Position in center or at base/tip?
			if (Origin == MeshCreateOrigin::Center)
				vec_position.y -= fHeight * 0.5f;
			else if (Origin == MeshCreateOrigin::Top)
				vec_position.y -= fHeight;

			// Inverting the normal?
			if (bInverted)
				vec_normal = -vec_normal;

			// Store vertex components
			if (has_position)
				gfx::vertexPack(&vec_position[0], false, gfx::Attrib::Position, format, pCurrentVertex);
			if (has_normal)
				gfx::vertexPack(&vec_normal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
			if (has_texcoord)
				gfx::vertexPack(&vec_tex_coords[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

			// Set flags for this vertex (we want to generate tangents 
			// and binormals if we need them).
			*pCurrentFlags++ = PreparationData::SourceContainsNormal;

			// Grow the object space bounding box for this mesh
			// by including the computed position.
			_bbox.add_point(vec_position);

			// Move on to next vertex
			pCurrentVertex += vertex_stride;

		} // Next Slice

	} // End if add end cap


	  // Now generate indices. Process each stack (except the top and bottom)
	Triangle * pCurrentTriangle = &_preparation_data.triangle_data[0];
	for (std::uint32_t stack = 0; stack < nStacks; ++stack)
	{
		// Generate two triangles for the quad on each slice
		for (std::uint32_t slice = 0; slice < nSlices; ++slice)
		{
			// If height was negative (i.e. faces are inverted)
			// we need to flip the order of the indices
			if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle++;

			} // End if inverted
			else
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle->indices[2] = (stack*(nSlices + 1)) + slice;
				pCurrentTriangle++;

				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = (stack*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[1] = ((stack + 1)*(nSlices + 1)) + slice + 1;
				pCurrentTriangle->indices[2] = ((stack + 1)*(nSlices + 1)) + slice;
				pCurrentTriangle++;

			} // End if not inverted

		} // Next Slice

	} // Next Stack

	  // Add cylinder cap geometry
	for (std::uint32_t slice = 0; slice < nSlices - 2; ++slice)
	{
		// If height was negative (i.e. faces are inverted)
		// we need to flip the order of the indices
		if (((!bInverted) && fHeight < 0) || (bInverted && fHeight > 0))
		{
			// Base Cap
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = caps_start;
			pCurrentTriangle->indices[1] = caps_start + slice + 1;
			pCurrentTriangle->indices[2] = caps_start + slice + 2;
			pCurrentTriangle++;

			// End Cap
			if (num_caps > 1)
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = caps_start + nSlices + slice + 2;
				pCurrentTriangle->indices[1] = caps_start + nSlices + slice + 1;
				pCurrentTriangle->indices[2] = caps_start + nSlices;
				pCurrentTriangle++;

			} // End if add second cap

		} // End if inverted
		else
		{
			// Base Cap
			pCurrentTriangle->data_group_id = 0;
			pCurrentTriangle->indices[0] = caps_start + slice + 2;
			pCurrentTriangle->indices[1] = caps_start + slice + 1;
			pCurrentTriangle->indices[2] = caps_start;
			pCurrentTriangle++;

			// End Cap
			if (num_caps > 1)
			{
				pCurrentTriangle->data_group_id = 0;
				pCurrentTriangle->indices[0] = caps_start + nSlices;
				pCurrentTriangle->indices[1] = caps_start + nSlices + slice + 1;
				pCurrentTriangle->indices[2] = caps_start + nSlices + slice + 2;
				pCurrentTriangle++;

			} // End if add second cap

		} // End if not inverted

	} // Next Slice

	  // We need to generate binormals / tangents?
	_preparation_data.compute_binormals = _vertex_format.has(gfx::Attrib::Bitangent);
	_preparation_data.compute_tangents = _vertex_format.has(gfx::Attrib::Tangent);

	// Finish up
	return end_prepare(bHardwareCopy, false, false);
}

//-----------------------------------------------------------------------------
//  Name : createBox ()
/// <summary>
/// Create box geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_cube(const gfx::VertexDecl& format, float fWidth, float fHeight, float fDepth, std::uint32_t nWidthSegs, std::uint32_t nHeightSegs, std::uint32_t nDepthSegs, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	return create_cube(format, fWidth, fHeight, fDepth, nWidthSegs, nHeightSegs, nDepthSegs, 1.0f, 1.0f, bInverted, Origin, bHardwareCopy);
}

//-----------------------------------------------------------------------------
//  Name : createBox ()
/// <summary>
/// Create box geometry.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::create_cube(const gfx::VertexDecl& format, float fWidth, float fHeight, float fDepth, std::uint32_t nWidthSegs, std::uint32_t nHeightSegs, std::uint32_t nDepthSegs, float fTexUScale, float fTexVScale, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
{
	std::uint32_t  nXCount, nYCount, nCounter;
	math::vec3 current_pos, vDeltaPosX, vDeltaPosY, vNormal;
	math::vec2 current_tex, vDeltaTex;

	// Clear out old data.
	dispose();

	// We are in the process of preparing.
	_prepare_status = MeshStatus::Preparing;
	_vertex_format = format;

	// Determine the correct offset to any relevant elements in the vertex
	bool has_position = _vertex_format.has(gfx::Attrib::Position);
	bool has_texcoord = _vertex_format.has(gfx::Attrib::TexCoord0);
	bool has_normal = _vertex_format.has(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Compute the number of faces and vertices that will be required for this box
	_preparation_data.triangle_count = (4 * (nWidthSegs * nDepthSegs)) + (4 * (nWidthSegs * nHeightSegs)) + (4 * (nHeightSegs * nDepthSegs));
	_preparation_data.vertex_count = (2 * ((nWidthSegs + 1) * (nDepthSegs + 1))) + (2 * ((nWidthSegs + 1) * (nHeightSegs + 1))) + (2 * ((nHeightSegs + 1) * (nDepthSegs + 1)));

	// Allocate enough space for the new vertex and triangle data
	_preparation_data.vertex_data.resize(_preparation_data.vertex_count * vertex_stride);
	_preparation_data.vertex_flags.resize(_preparation_data.vertex_count);
	_preparation_data.triangle_data.resize(_preparation_data.triangle_count);

	// Ensure width and depth are absolute (prevent inverting on those axes)
	fWidth = math::abs(fWidth);
	fDepth = math::abs(fDepth);

	// Generate faces
	std::uint8_t * pCurrentVertex = &_preparation_data.vertex_data[0];
	std::uint8_t * pCurrentFlags = &_preparation_data.vertex_flags[0];
	float fHalfWidth = fWidth / 2, fHalfDepth = fDepth / 2;
	for (std::uint32_t i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // +X
			nXCount = nDepthSegs + 1;
			nYCount = nHeightSegs + 1;
			vDeltaPosX = math::vec3(0, 0, fDepth / (float)nDepthSegs);
			vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
			current_pos = math::vec3(fHalfWidth, fHeight, -fHalfDepth);
			vNormal = math::vec3(1, 0, 0);
			break;

		case 1: // +Y
			nXCount = nWidthSegs + 1;
			nYCount = nDepthSegs + 1;
			vDeltaPosX = math::vec3(fWidth / (float)nWidthSegs, 0, 0);
			vDeltaPosY = math::vec3(0, 0, -fDepth / (float)nDepthSegs);
			current_pos = math::vec3(-fHalfWidth, fHeight, fHalfDepth);
			vNormal = (fHeight > 0.0f) ? math::vec3(0, 1, 0) : math::vec3(0, -1, 0);
			break;

		case 2: // +Z
			nXCount = nWidthSegs + 1;
			nYCount = nHeightSegs + 1;
			vDeltaPosX = math::vec3(-fWidth / (float)nWidthSegs, 0, 0);
			vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
			current_pos = math::vec3(fHalfWidth, fHeight, fHalfDepth);
			vNormal = math::vec3(0, 0, 1);
			break;

		case 3: // -X
			nXCount = nDepthSegs + 1;
			nYCount = nHeightSegs + 1;
			vDeltaPosX = math::vec3(0, 0, -fDepth / (float)nDepthSegs);
			vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
			current_pos = math::vec3(-fHalfWidth, fHeight, fHalfDepth);
			vNormal = math::vec3(-1, 0, 0);
			break;

		case 4: // -Y
			nXCount = nWidthSegs + 1;
			nYCount = nDepthSegs + 1;
			vDeltaPosX = math::vec3(fWidth / (float)nWidthSegs, 0, 0);
			vDeltaPosY = math::vec3(0, 0, fDepth / (float)nDepthSegs);
			current_pos = math::vec3(-fHalfWidth, 0, -fHalfDepth);
			vNormal = (fHeight > 0.0f) ? math::vec3(0, -1, 0) : math::vec3(0, 1, 0);
			break;

		case 5: // -Z
			nXCount = nWidthSegs + 1;
			nYCount = nHeightSegs + 1;
			vDeltaPosX = math::vec3(fWidth / (float)nWidthSegs, 0, 0);
			vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
			current_pos = math::vec3(-fHalfWidth, fHeight, -fHalfDepth);
			vNormal = math::vec3(0, 0, -1);
			break;

		} // End Face Switch

		  // Should we invert the vertex normal
		if (bInverted == true)
			vNormal = -vNormal;

		// Add faces
		current_tex = math::vec2(0, 0);
		vDeltaTex = math::vec2(1.0f / (float)(nXCount - 1), 1.0f / (float)(nYCount - 1));
		for (std::uint32_t y = 0; y < nYCount; ++y)
		{
			for (std::uint32_t x = 0; x < nXCount; ++x)
			{
				math::vec3 vOutputPos = current_pos;
				if (Origin == MeshCreateOrigin::Center)
					vOutputPos.y -= fHeight * 0.5f;
				else if (Origin == MeshCreateOrigin::Top)
					vOutputPos.y -= fHeight;

				// Store vertex components
				if (has_position)
					gfx::vertexPack(&vOutputPos[0], false, gfx::Attrib::Position, format, pCurrentVertex);
				if (has_normal)
					gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, format, pCurrentVertex);
				if (has_texcoord)
					gfx::vertexPack(&math::vec2(current_tex.x * fTexUScale, current_tex.y * fTexVScale)[0], true, gfx::Attrib::TexCoord0, format, pCurrentVertex);

				// Set flags for this vertex (we want to generate tangents 
				// and binormals if we need them).
				*pCurrentFlags++ = PreparationData::SourceContainsNormal;

				// Grow the object space bounding box for this mesh
				// by including the computed position.
				_bbox.add_point(vOutputPos);

				// Move to next vertex position
				current_pos += vDeltaPosX;
				current_tex.x += vDeltaTex.x;
				pCurrentVertex += vertex_stride;

			} // Next Column

			  // Move to next row
			current_pos += vDeltaPosY;
			current_pos -= vDeltaPosX * (float)nXCount;
			current_tex.x = 0.0f;
			current_tex.y += vDeltaTex.y;

		} // Next Row

	} // Next Face


	  // Now generate indices. For each box face.
	nCounter = 0;
	Triangle * pCurrentTriangle = &_preparation_data.triangle_data[0];
	for (std::uint32_t i = 0; i < 6; ++i)
	{
		switch (i)
		{
		case 0: // +X
		case 3: // -X
			nXCount = nDepthSegs + 1;
			nYCount = nHeightSegs + 1;
			break;

		case 1: // +Y
		case 4: // -Y
			nXCount = nWidthSegs + 1;
			nYCount = nDepthSegs + 1;
			break;

		case 2: // +Z
		case 5: // -Z
			nXCount = nWidthSegs + 1;
			nYCount = nHeightSegs + 1;
			break;

		} // End Face Switch

		for (std::uint32_t y = 0; y < nYCount - 1; ++y)
		{
			for (std::uint32_t x = 0; x < nXCount - 1; ++x)
			{
				// If height was negative (i.e. faces are inverted)
				// we need to flip the order of the indices
				if ((bInverted == false && fHeight < 0) || (bInverted == true && fHeight > 0))
				{
					pCurrentTriangle->data_group_id = 0;
					pCurrentTriangle->indices[0] = x + 1 + ((y + 1) * nXCount) + nCounter;
					pCurrentTriangle->indices[1] = x + 1 + (y * nXCount) + nCounter;
					pCurrentTriangle->indices[2] = x + (y * nXCount) + nCounter;
					pCurrentTriangle++;

					pCurrentTriangle->data_group_id = 0;
					pCurrentTriangle->indices[0] = x + ((y + 1) * nXCount) + nCounter;
					pCurrentTriangle->indices[1] = x + 1 + ((y + 1) * nXCount) + nCounter;
					pCurrentTriangle->indices[2] = x + (y * nXCount) + nCounter;
					pCurrentTriangle++;

				} // End if inverted
				else
				{
					pCurrentTriangle->data_group_id = 0;
					pCurrentTriangle->indices[0] = x + (y * nXCount) + nCounter;
					pCurrentTriangle->indices[1] = x + 1 + (y * nXCount) + nCounter;
					pCurrentTriangle->indices[2] = x + 1 + ((y + 1) * nXCount) + nCounter;
					pCurrentTriangle++;

					pCurrentTriangle->data_group_id = 0;
					pCurrentTriangle->indices[0] = x + (y * nXCount) + nCounter;
					pCurrentTriangle->indices[1] = x + 1 + ((y + 1) * nXCount) + nCounter;
					pCurrentTriangle->indices[2] = x + ((y + 1) * nXCount) + nCounter;
					pCurrentTriangle++;

				} // End if normal

			} // Next Column

		} // Next Row

		  // Compute vertex start for next face.
		nCounter += nXCount * nYCount;

	} // Next Face

	  // We need to generate binormals / tangents?
	_preparation_data.compute_binormals = _vertex_format.has(gfx::Attrib::Bitangent);
	_preparation_data.compute_tangents = _vertex_format.has(gfx::Attrib::Tangent);

	// Finish up
	return end_prepare(bHardwareCopy, false, false);
}


bool Mesh::scan_and_generate(bool bWeld /*= true*/)
{
	// Were we previously preparing?
	if (_prepare_status != MeshStatus::Preparing)
	{
		auto logger = logging::get("Log");
		logger->error().write("Attempting to call 'end_prepare' on a mesh without first calling 'prepareMesh' is not allowed.\n");
		return false;

	} // End if previously preparing

	  // Clear out old data that is no longer necessary in order to preserve memory
	gfx::setVertexBuffer(gfx::VertexBufferHandle{ gfx::invalidHandle });
	// Scan the preparation data for degenerate triangles.
	std::uint16_t position_offset = _vertex_format.getOffset(gfx::Attrib::Position);
	std::uint16_t vertex_stride = _vertex_format.getStride();
	std::uint8_t * src_vertices_ptr = &_preparation_data.vertex_data[0] + position_offset;
	for (std::uint32_t i = 0; i < _preparation_data.triangle_count; ++i)
	{
		Triangle & Tri = _preparation_data.triangle_data[i];
		math::vec3 v1;
		float vf1[4];
		gfx::vertexUnpack(vf1, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, Tri.indices[0]);
		math::vec3 v2;
		float vf2[4];
		gfx::vertexUnpack(vf2, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, Tri.indices[1]);
		math::vec3 v3;
		float vf3[4];
		gfx::vertexUnpack(vf3, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, Tri.indices[2]);
		memcpy(&v1[0], vf1, 3 * sizeof(float));
		memcpy(&v2[0], vf2, 3 * sizeof(float));
		memcpy(&v3[0], vf3, 3 * sizeof(float));

		math::vec3 c = math::cross(v2 - v1, v3 - v1);
		if (math::length2(c) < (4.0f * 0.000001f * 0.000001f))
			Tri.flags |= TriangleFlags::Degenerate;

	} // Next triangle

	  // Process the vertex data in order to generate any additional components that may be necessary
	  // (i.e. Normal, Binormal and Tangent)
	return generate_vertex_components(bWeld);

}

//-----------------------------------------------------------------------------
//  Name : endPrepare ()
/// <summary>
/// All data has been added to the mesh and we should now build the
/// renderable data for the mesh.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::end_prepare(bool bHardwareCopy /* = true */, bool bWeld /* = true */, bool bOptimize /* = true */, bool build_buffers /*= true*/)
{
	if (scan_and_generate(bWeld) == false)
		return false;

	// Allocate the system memory vertex buffer ready for population.
	_vertex_count = (std::uint32_t)_preparation_data.vertex_count;
	_system_vb = new std::uint8_t[_vertex_count * _vertex_format.getStride()];

	// Copy vertex data into the new buffer and dispose of the temporary data.
	memcpy(_system_vb, &_preparation_data.vertex_data[0], _vertex_count * _vertex_format.getStride());
	_preparation_data.vertex_data.clear();
	_preparation_data.vertex_flags.clear();
	_preparation_data.vertex_count = 0;

	// Vertex data has been updated and potentially needs to be serialized.


	// Allocate the memory for our system memory index buffer
	_face_count = _preparation_data.triangle_count;
	_system_ib = new std::uint32_t[_face_count * 3];

	// math::transform_t triangle indices, material and data group information
	// to the final triangle data arrays. We keep the latter two handy so
	// that we know precisely which subset each triangle belongs to.
	_triangle_data.resize(_face_count);
	std::uint32_t * pDstIndices = _system_ib;
	for (std::uint32_t i = 0; i < _face_count; ++i)
	{
		// Copy indices.
		const Triangle & TriIn = _preparation_data.triangle_data[i];
		*pDstIndices++ = TriIn.indices[0];
		*pDstIndices++ = TriIn.indices[1];
		*pDstIndices++ = TriIn.indices[2];

		// Copy triangle subset information.
		MeshSubsetKey & TriOut = _triangle_data[i];
		TriOut.data_group_id = TriIn.data_group_id;

	} // Next triangle
	_preparation_data.triangle_count = 0;
	_preparation_data.triangle_data.clear();

	// Index data has been updated and potentially needs to be serialized.
	if (build_buffers)
		build_vb(bHardwareCopy);

	// Skin binding data has potentially been updated and needs to be serialized.


	// Finally perform the final sort of the mesh data in order
	// to build the index buffer and subset tables.
	if (!sort_mesh_data(bOptimize, bHardwareCopy, build_buffers))
		return false;

	// The mesh is now prepared
	_prepare_status = MeshStatus::Prepared;
	_hardware_mesh = bHardwareCopy;
	_optimize_mesh = bOptimize;

	// Success!
	return true;
}

void Mesh::build_vb(bool bHardwareCopy)
{
	// A video memory copy of the mesh was requested?
	if (bHardwareCopy)
	{
		// Calculate the required size of the vertex buffer
		std::uint32_t buffer_size = _vertex_count * _vertex_format.getStride();

		const gfx::Memory* mem = gfx::copy(_system_vb, static_cast<std::uint32_t>(buffer_size));
		_hardware_vb = std::make_shared<VertexBuffer>();
		_hardware_vb->populate(mem, _vertex_format);

	} // End if video memory vertex buffer required
}

void Mesh::build_ib(bool bHardwareCopy)
{
	// Hardware versions of the final buffer were required?
	if (bHardwareCopy)
	{
		// Calculate the required size of the index buffer
		std::uint32_t buffer_size = _face_count * 3 * sizeof(std::uint32_t);

		// Allocate hardware buffer if required (i.e. it does not already exist).
		if (!_hardware_ib || (_hardware_ib && !_hardware_ib->is_valid()))
		{
			const gfx::Memory* mem = gfx::copy(_system_ib, static_cast<std::uint32_t>(buffer_size));
			_hardware_ib = std::make_shared<IndexBuffer>();
			_hardware_ib->populate(mem, BGFX_BUFFER_INDEX32);
		} // End if not allocated

	} // End if hardware buffer required
}

//-----------------------------------------------------------------------------
// Name : sortMeshData() (Protected)
/// <summary>
/// Sort the data in the mesh into material & datagroup order.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::sort_mesh_data(bool bOptimize, bool bHardwareCopy, bool build_buffer)
{
	std::map<MeshSubsetKey, std::uint32_t> SubsetSizes;
	std::map<MeshSubsetKey, std::uint32_t>::iterator itSubsetSize;
	DataGroupSubsetMap::iterator itDataGroup;
	std::uint32_t i, j;

	// Clear out any old data EXCEPT the old subset index
	// We'll need this in order to understand how to update
	// the material reference counting later on.
	_data_groups.clear();
	_subset_lookup.clear();

	// Our first job is to collate all the various subsets and also
	// to determine how many triangles should exist in each.
	for (i = 0; i < _face_count; ++i)
	{
		const MeshSubsetKey & SubsetKey = _triangle_data[i];

		// Already contains this material / data group combination?
		itSubsetSize = SubsetSizes.find(SubsetKey);
		if (itSubsetSize == SubsetSizes.end())
		{
			// Add a new entry for this subset
			SubsetSizes[SubsetKey] = 1;

		} // End if !exists
		else
		{
			// Update the existing subset
			itSubsetSize->second++;

		} // End if already encountered

	} // Next Triangle

	  // We should now have a complete list of subsets and the number of triangles
	  // which should exist in each. Populate mesh subset table and update start / count 
	  // values so that we can correctly generate the new sorted index buffer.
	std::uint32_t nCounter = 0;
	std::uint32_t vCounter = 0;
	SubsetArray NewSubsets;
	for (itSubsetSize = SubsetSizes.begin(); itSubsetSize != SubsetSizes.end(); ++itSubsetSize)
	{
		// Construct a new subset and populate with initial construction 
		// values including the expected starting face location.
		const MeshSubsetKey & Key = itSubsetSize->first;
		Subset * subset = new Subset();
		subset->data_group_id = Key.data_group_id;
		subset->face_start = nCounter;
		nCounter += itSubsetSize->second;

		// Ensure that "FaceCount" defaults to zero at this point
		// so that we can keep a running total during the final buffer 
		// construction.
		subset->face_count = 0;

		// Also reset vertex values as appropriate (will grow
		// using standard 'bounding' value insert).
		subset->vertex_start = 0x7FFFFFFF;
		subset->vertex_count = 0;

		// Add to list for fast linear access, and lookup table
		// for sorted search.
		NewSubsets.push_back(subset);
		_subset_lookup[Key] = subset;

		// Add to data group lookup table
		itDataGroup = _data_groups.find(subset->data_group_id);
		if (itDataGroup == _data_groups.end())
			_data_groups[subset->data_group_id].push_back(subset);
		else
			itDataGroup->second.push_back(subset);


	} // Next Subset

	  // Allocate space for new sorted index buffer and face re-map information
	std::uint32_t * pSrcIndices = _system_ib;
	std::uint32_t * pDstIndices = new std::uint32_t[_face_count * 3];
	std::uint32_t * pFaceRemap = new std::uint32_t[_face_count];

	// Start building new indices
	std::int32_t  index;
	std::uint32_t nIndexStart = 0;
	for (i = 0; i < _face_count; ++i)
	{
		// Find a matching subset for this triangle
		Subset * subset = _subset_lookup[_triangle_data[i]];

		// Copy index data over to new buffer, taking care to record the correct
		// vertex values as required. We'll temporarily use VertexStart and VertexCount
		// as a MathUtility::minValue/max record that we'll come round and correct later.
		nIndexStart = (subset->face_start + subset->face_count) * 3;

		// Index[0]
		index = (std::int32_t)(*pSrcIndices++);
		if (index < subset->vertex_start)
			subset->vertex_start = index;
		if (index > subset->vertex_count)
			subset->vertex_count = index;
		pDstIndices[nIndexStart++] = index;

		// Index[1]
		index = (std::int32_t)(*pSrcIndices++);
		if (index < subset->vertex_start)
			subset->vertex_start = index;
		if (index > subset->vertex_count)
			subset->vertex_count = index;
		pDstIndices[nIndexStart++] = index;

		// Index[2]
		index = (std::int32_t)(*pSrcIndices++);
		if (index < subset->vertex_start)
			subset->vertex_start = index;
		if (index > subset->vertex_count)
			subset->vertex_count = index;
		pDstIndices[nIndexStart++] = index;

		// Store face re-map information so that we can remap data as required
		pFaceRemap[i] = subset->face_start + subset->face_count;

		// We have now recorded a triangle in this subset
		subset->face_count++;

	} // Next Triangle

	auto sort_predicate = [](const Subset* lhs, const Subset* rhs)
	{
		if ((lhs->data_group_id < rhs->data_group_id))
			return true;
		return false;
	};

	// Sort the subset list in order to ensure that all subsets with the same 
	// materials and data groups are added next to one another in the final
	// index buffer. This ensures that we can batch draw all subsets that share 
	// common properties.
	std::sort(NewSubsets.begin(), NewSubsets.end(), sort_predicate);

	// Perform the same sort on the data group and material mapped lists.
	// Also take the time to build the final list of materials used by this mesh
	// (render control batching system requires that we cache this information in a 
	// specific format).
	for (itDataGroup = _data_groups.begin(); itDataGroup != _data_groups.end(); ++itDataGroup)
		std::sort(itDataGroup->second.begin(), itDataGroup->second.end(), sort_predicate);

	// Optimize the faces as we transfer to the final destination index buffer
	// if requested. Otherwise, just copy them over directly.
	pSrcIndices = pDstIndices;
	pDstIndices = _system_ib;

	for (nCounter = 0, i = 0; i < (size_t)NewSubsets.size(); ++i)
	{
		Subset* subset = NewSubsets[i];

		// Note: Remember that at this stage, the subset's 'vertexCount' member still describes
		// a 'max' vertex (not a count)... We're correcting this later.
		if (bOptimize == true)
			build_optimized_index_buffer(subset, pSrcIndices + (subset->face_start * 3), pDstIndices, subset->vertex_start, subset->vertex_count);
		else
			memcpy(pDstIndices, pSrcIndices + (subset->face_start * 3), subset->face_count * 3 * sizeof(std::uint32_t));

		// This subset's starting face now refers to its location 
		// in the final destination buffer rather than the temporary one.
		subset->face_start = nCounter;
		nCounter += subset->face_count;

		// Move on to output next sorted subset.
		pDstIndices += subset->face_count * 3;

	} // Next Subset

	  // Clean up.
	checked_array_delete(pSrcIndices);

	// Rebuild the additional triangle data based on the newly sorted
	// subset data, and also convert the previously recorded maximum
	// vertex value (stored in "VertexCount") into its final form
	for (i = 0; i < (std::uint32_t)NewSubsets.size(); ++i)
	{
		// Convert vertex "Max" to "Count"
		Subset * subset = NewSubsets[i];
		subset->vertex_count = (subset->vertex_count - subset->vertex_start) + 1;

		// Update additional triangle data array.
		for (j = subset->face_start; j < ((std::uint32_t)subset->face_start + (std::uint32_t)subset->face_count); ++j)
		{
			_triangle_data[j].data_group_id = subset->data_group_id;

		} // Next Triangle

	} // Next Subset

	// We're done with the remap data.
	// TODO: Note - we don't actually use the face remap information at
	// the moment, but it could be useful?
	checked_array_delete(pFaceRemap);

	// Hardware versions of the final buffer were required?
	if (build_buffer)
		build_ib(bHardwareCopy);
	// Index data and subsets have been updated and potentially need to be serialized.

	// Destroy old subset data.
	for (i = 0; i < _mesh_subsets.size(); ++i)
		checked_delete(_mesh_subsets[i]);
	_mesh_subsets.clear();

	// Use the new subset data.
	_mesh_subsets = NewSubsets;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : findVertexOptimizerScore () (Private, Static)
/// <summary>
/// During optimization, this method will generate scores used to
/// identify important vertices when ordering triangle data.
/// Note : Thanks to Tom Forsyth for the fantastic implementation on which
/// this is based.
/// URL  : http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
/// </summary>
//-----------------------------------------------------------------------------
float Mesh::find_vertex_optimizer_score(const OptimizerVertexInfo * pVertexInfo)
{
	float Score = 0.0f;

	// Do any remaining triangles use this vertex?
	if (pVertexInfo->unused_triangle_references == 0)
		return -1.0f;

	std::int32_t CachePosition = pVertexInfo->cache_position;
	if (CachePosition < 0)
	{

		// Vertex is not in FIFO cache - no score.
	}
	else
	{
		if (CachePosition < 3)
		{
			// This vertex was used in the last triangle,
			// so it has a fixed score, whichever of the three
			// it's in. Otherwise, you can get very different
			// answers depending on whether you add
			// the triangle 1,2,3 or 3,1,2 - which is silly.
			Score = MeshOptimizer::LastTriScore;
		}
		else
		{
			// Points for being high in the cache.
			const float Scaler = 1.0f / (MeshOptimizer::MaxVertexCacheSize - 3);
			Score = 1.0f - (CachePosition - 3) * Scaler;
			Score = math::pow(Score, MeshOptimizer::CacheDecayPower);
		}

	} // End if already in vertex cache

	  // Bonus points for having a low number of tris still to
	  // use the vert, so we get rid of lone verts quickly.
	float ValenceBoost = math::pow((float)pVertexInfo->unused_triangle_references, -MeshOptimizer::ValenceBoostPower);
	Score += MeshOptimizer::ValenceBoostScale * ValenceBoost;

	// Return the final score
	return Score;
}

//-----------------------------------------------------------------------------
//  Name : buildOptimizedIndexBuffer () (Private, Static)
/// <summary>
/// Calculate the best order for triangle data, optimizing for efficient
/// use of the hardware vertex cache, given an unkown vertex cache size
/// and implementation.
/// Note : Thanks to Tom Forsyth for the fantastic implementation on which
/// this is based.
/// URL  : http://home.comcast.net/~tom_forsyth/papers/fast_vert_cache_opt.html
/// </summary>
//-----------------------------------------------------------------------------
void Mesh::build_optimized_index_buffer(const Subset * subset, std::uint32_t * pSrcBuffer, std::uint32_t * pDestBuffer, std::uint32_t nMinVertex, std::uint32_t nMaxVertex)
{
	OptimizerVertexInfo   * pVertexInfo = nullptr, *pVert;
	OptimizerTriangleInfo * pTriangleInfo = nullptr, *pTri;
	std::uint32_t                i, j, k, vertex_count;
	float                 fBestScore = 0.0f, fScore;
	std::int32_t                 nBestTriangle = -1;
	std::uint32_t                nVertexCacheSize = 0;
	std::uint32_t                index, nTriangleIndex, nTemp;

	// Declare vertex cache storage (plus one to allow them to drop "off the end")
	std::uint32_t pVertexCache[MeshOptimizer::MaxVertexCacheSize + 1];

	// First allocate enough room for the optimization information for each vertex and triangle
	vertex_count = (nMaxVertex - nMinVertex) + 1;
	pVertexInfo = new OptimizerVertexInfo[vertex_count];
	pTriangleInfo = new OptimizerTriangleInfo[subset->face_count];

	// The first pass is to initialize the vertex information with information about the
	// faces which reference them.
	for (i = 0; i < (unsigned)subset->face_count; ++i)
	{
		index = pSrcBuffer[i * 3] - nMinVertex;
		pVertexInfo[index].unused_triangle_references++;
		pVertexInfo[index].triangle_references.push_back(i);
		index = pSrcBuffer[(i * 3) + 1] - nMinVertex;
		pVertexInfo[index].unused_triangle_references++;
		pVertexInfo[index].triangle_references.push_back(i);
		index = pSrcBuffer[(i * 3) + 2] - nMinVertex;
		pVertexInfo[index].unused_triangle_references++;
		pVertexInfo[index].triangle_references.push_back(i);

	} // Next Triangle

	  // Initialize vertex scores
	for (i = 0; i < vertex_count; ++i)
		pVertexInfo[i].vertex_score = find_vertex_optimizer_score(&pVertexInfo[i]);

	// Compute the score for each triangle, and record the triangle with the best score
	for (i = 0; i < (unsigned)subset->face_count; ++i)
	{
		// The triangle score is the sum of the scores of each of
		// its three vertices.
		index = pSrcBuffer[i * 3] - nMinVertex;
		fScore = pVertexInfo[index].vertex_score;
		index = pSrcBuffer[(i * 3) + 1] - nMinVertex;
		fScore += pVertexInfo[index].vertex_score;
		index = pSrcBuffer[(i * 3) + 2] - nMinVertex;
		fScore += pVertexInfo[index].vertex_score;
		pTriangleInfo[i].triangle_score = fScore;

		// Record the triangle with the highest score
		if (fScore > fBestScore)
		{
			fBestScore = fScore;
			nBestTriangle = (signed)i;

		} // End if better than previous score

	} // Next Triangle

	  // Now we can start adding triangles, beginning with the previous highest scoring triangle.
	for (i = 0; i < (unsigned)subset->face_count; ++i)
	{
		// If we don't know the best triangle, for whatever reason, find it
		if (nBestTriangle < 0)
		{
			nBestTriangle = -1;
			fBestScore = 0.0f;

			// Iterate through the entire list of un-added faces
			for (j = 0; j < (unsigned)subset->face_count; ++j)
			{
				if (pTriangleInfo[j].added == false)
				{
					fScore = pTriangleInfo[j].triangle_score;

					// Record the triangle with the highest score
					if (fScore > fBestScore)
					{
						fBestScore = fScore;
						nBestTriangle = (signed)j;

					} // End if better than previous score

				} // End if not added

			} // Next Triangle

		} // End if best triangle is not known

		  // Use the best scoring triangle from last pass and reset score keeping
		nTriangleIndex = (unsigned)nBestTriangle;
		pTri = &pTriangleInfo[nTriangleIndex];
		nBestTriangle = -1;
		fBestScore = 0.0f;

		// This triangle can be added to the 'draw' list, and each
		// of the vertices it references should be updated.
		pTri->added = true;
		for (j = 0; j < 3; ++j)
		{
			// Extract the vertex index and store in the index buffer
			index = pSrcBuffer[(nTriangleIndex * 3) + j];
			*pDestBuffer++ = index;

			// Adjust the index so that it points into our info buffer
			// rather than the actual source vertex itself.
			index = index - nMinVertex;

			// Retrieve the referenced vertex information
			pVert = &pVertexInfo[index];

			// Reduce the 'valence' of this vertex (one less triangle is now referencing)
			pVert->unused_triangle_references--;

			// Remove this triangle from the list of references in the vertex
			auto itReference = std::find(pVert->triangle_references.begin(), pVert->triangle_references.end(), nTriangleIndex);
			if (itReference != pVert->triangle_references.end())
				pVert->triangle_references.erase(itReference);

			// Now we must update the vertex cache to include this vertex. If it was
			// already in the cache, it should be moved to the head, otherwise it should
			// be inserted (pushing one off the end).
			if (pVert->cache_position == -1)
			{
				// Not in the vertex cache, insert it at the head.
				if (nVertexCacheSize > 0)
				{
					// First shuffle EVERYONE up by one position in the cache.
					memmove(&pVertexCache[1], &pVertexCache[0], nVertexCacheSize * sizeof(std::uint32_t));

				} // End if any vertices exist in the cache

				  // Grow the cache if applicable
				if (nVertexCacheSize < MeshOptimizer::MaxVertexCacheSize)
					nVertexCacheSize++;
				else
				{
					// Set the associated index of the vertex which dropped "off the end" of the cache.
					pVertexInfo[pVertexCache[nVertexCacheSize]].cache_position = -1;

				} // End if no more room

				  // Overwrite the first entry
				pVertexCache[0] = index;

			} // End if not in cache
			else if (pVert->cache_position > 0)
			{
				// Already in the vertex cache, move it to the head.
				// Note : If the cache position is already 0, we just ignore
				// it... hence the above 'else if' rather than just 'else'.
				if (pVert->cache_position == 1)
				{
					// We were in the second slot, just swap the two
					nTemp = pVertexCache[0];
					pVertexCache[0] = index;
					pVertexCache[1] = nTemp;

				} // End if simple swap
				else
				{
					// Shuffle EVERYONE up who came before us.
					memmove(&pVertexCache[1], &pVertexCache[0], pVert->cache_position * sizeof(std::uint32_t));

					// Insert this vertex at the head
					pVertexCache[0] = index;

				} // End if memory move required

			} // End if already in cache

			  // Update the cache position records for all vertices in the cache
			for (k = 0; k < nVertexCacheSize; ++k)
				pVertexInfo[pVertexCache[k]].cache_position = k;

		} // Next Index

		  // Recalculate the of all vertices contained in the cache
		for (j = 0; j < nVertexCacheSize; ++j)
		{
			pVert = &pVertexInfo[pVertexCache[j]];
			pVert->vertex_score = find_vertex_optimizer_score(pVert);

		} // Next entry in the vertex cache

		  // Update the score of the triangles which reference this vertex
		  // and record the highest scoring.
		for (j = 0; j < nVertexCacheSize; ++j)
		{
			pVert = &pVertexInfo[pVertexCache[j]];

			// For each triangle referenced
			for (k = 0; k < pVert->unused_triangle_references; ++k)
			{
				nTriangleIndex = pVert->triangle_references[k];
				pTri = &pTriangleInfo[nTriangleIndex];
				fScore = pVertexInfo[pSrcBuffer[(nTriangleIndex * 3)] - nMinVertex].vertex_score;
				fScore += pVertexInfo[pSrcBuffer[(nTriangleIndex * 3) + 1] - nMinVertex].vertex_score;
				fScore += pVertexInfo[pSrcBuffer[(nTriangleIndex * 3) + 2] - nMinVertex].vertex_score;
				pTri->triangle_score = fScore;

				// Highest scoring so far?
				if (fScore > fBestScore)
				{
					fBestScore = fScore;
					nBestTriangle = (signed)nTriangleIndex;

				} // End if better than previous score

			} // Next Triangle

		} // Next entry in the vertex cache

	} // Next Triangle to Add

	  // Destroy the temporary arrays
	checked_array_delete(pVertexInfo);
	checked_array_delete(pTriangleInfo);
}

//-----------------------------------------------------------------------------
//  Name : draw ()
/// <summary>
/// Draw the mesh in its entirety.
/// </summary>
//-----------------------------------------------------------------------------
void Mesh::draw()
{
	// Should we get involved in the rendering process?
	std::int32_t nFaceStart = 0, face_count = 0, nVertexStart = 0, vertex_count = 0;

	for (size_t i = 0; i < _mesh_subsets.size(); ++i)
	{
		draw_subset(std::uint32_t(i));
	}

}


//-----------------------------------------------------------------------------
//  Name : drawSubset ()
/// <summary>
/// Draw an individual subset of the mesh based on the material AND
/// data group specified.
/// </summary>
//-----------------------------------------------------------------------------
void Mesh::draw_subset(std::uint32_t nDataGroupId)
{
	// Attempt to find a matching subset.
	auto itSubset = _subset_lookup.find(MeshSubsetKey(nDataGroupId));
	if (itSubset == _subset_lookup.end())
		return;

	// Process and draw all subsets of the mesh that use the specified material.
	std::int32_t nSubsetVertStart, nSubsetVertEnd;
	std::int32_t nFaceStart = 0, face_count = 0, nVertexStart = 0, nVertexEnd = 0, vertex_count = 0;

	Subset * subset = itSubset->second;
	nFaceStart = subset->face_start;
	face_count = subset->face_count;
	nVertexStart = subset->vertex_start;
	nVertexEnd = nVertexStart + subset->vertex_count - 1;

	// Vertex start/end is a little more complex, but can be computed
	// using a containment style test. First precompute some values to
	// make the tests a little simpler.
	nSubsetVertStart = subset->vertex_start;
	nSubsetVertEnd = nSubsetVertStart + subset->vertex_count - 1;

	// Perform the containment tests
	if (nSubsetVertStart < nVertexStart) nVertexStart = nSubsetVertStart;
	if (nSubsetVertStart > nVertexEnd) nVertexEnd = nSubsetVertStart;
	if (nSubsetVertEnd < nVertexStart) nVertexStart = nSubsetVertEnd;
	if (nSubsetVertEnd > nVertexEnd) nVertexEnd = nSubsetVertEnd;



	// Compute the final vertex count.
	vertex_count = (nVertexEnd - nVertexStart) + 1;

	// Render any batched data.
	if (face_count > 0)
	{
		// Set vertex and index buffer source streams
		gfx::setVertexBuffer(_hardware_vb->handle, (std::uint32_t)0, (std::uint32_t)vertex_count);
		gfx::setIndexBuffer(_hardware_ib->handle, (std::uint32_t)nFaceStart * 3, (std::uint32_t)face_count * 3);

	}
}


//-----------------------------------------------------------------------------
//  Name : draw ()
/// <summary>
/// Draw the mesh (face count = num_faces).
/// Note : Used during volume rendering.
/// </summary>
//-----------------------------------------------------------------------------
void Mesh::draw(std::uint32_t num_faces)
{
	// Set vertex and index buffer source streams
	gfx::setVertexBuffer(_hardware_vb->handle, 0, (std::uint32_t)_vertex_count);
	gfx::setIndexBuffer(_hardware_ib->handle, 0, (std::uint32_t)num_faces * 3);
}


//-----------------------------------------------------------------------------
//  Name : generateVertexComponents () (Private)
/// <summary>
/// Some vertex components potentially need to be generated. This may
/// include vertex normals, binormals or tangents. This function will
/// generate any such components which were not provided.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::generate_vertex_components(bool bWeld)
{
	// Vertex normals were requested (and at least some were not yet provided?)
	if (_force_normal_generation || _preparation_data.compute_normals)
	{
		// Generate the adjacency information for vertex normal computation
		UInt32Array adjacency;
		if (!generate_adjacency(adjacency))
		{
			auto logger = logging::get("Log");
			logger->error().write("Failed to generate adjacency buffer mesh containing {0} faces.\n", _preparation_data.triangle_count);
			return false;

		} // End if failed to generate
		if (_force_barycentric_generation || _preparation_data.compute_barycentric)
		{
			// Generate any vertex barycentric coords that have not been provided
			if (!generate_vertex_barycentrics(&adjacency.front()))
			{
				auto logger = logging::get("Log");
				logger->error().write("Failed to generate vertex barycentric coords for mesh containing {0} faces.\n", _preparation_data.triangle_count);
				return false;

			} // End if failed to generate

		} // End if compute

		// Generate any vertex normals that have not been provided
		if (!generate_vertex_normals(&adjacency.front()))
		{
			auto logger = logging::get("Log");
			logger->error().write("Failed to generate vertex normals for mesh containing {0} faces.\n", _preparation_data.triangle_count);
			return false;

		} // End if failed to generate

	} // End if compute

	// Weld vertices at this point
	if (bWeld)
	{
		if (!weld_vertices())
		{
			auto logger = logging::get("Log");
			logger->error().write("Failed to weld vertices for mesh containing {0} faces.\n", _preparation_data.triangle_count);
			return false;

		} // End if failed to weld

	} // End if optional weld

	  // Binormals and / or tangents were requested (and at least some where not yet provided?)
	if (_force_tangent_generation || _preparation_data.compute_binormals || _preparation_data.compute_tangents)
	{
		// Requires normals
		if (_vertex_format.has(gfx::Attrib::Normal))
		{
			// Generate any vertex tangents that have not been provided
			if (!generate_vertex_tangents())
			{
				auto logger = logging::get("Log");
				logger->error().write("Failed to generate vertex tangents for mesh containing {0} faces.\n", _preparation_data.triangle_count);
				return false;

			} // End if failed to generate

		} // End if has normals

	} // End if compute

	  // Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : generateVertexNormals () (Private)
/// <summary>
/// Generates any vertex normals that may have been requested but not
/// provided when adding vertex data.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::generate_vertex_normals(std::uint32_t* pAdjacency, UInt32Array * pRemapArray /* = nullptr */)
{
	std::uint32_t nStartTri, nPreviousTri, nCurrentTri;
	math::vec3 vecEdge1, vecEdge2, vec_normal;
	std::uint32_t i, j, k, index;

	// Get access to useful data offset information.
	std::uint16_t position_offset = _vertex_format.getOffset(gfx::Attrib::Position);
	std::uint16_t normal_offset = _vertex_format.getOffset(gfx::Attrib::Normal);
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// Final format requests vertex normals?
	if (normal_offset < 0)
		return true;

	// Size the remap array accordingly and populate it with the default mapping.
	std::uint32_t original_vertex_count = _preparation_data.vertex_count;
	if (pRemapArray)
	{
		pRemapArray->resize(_preparation_data.vertex_count);
		for (i = 0; i < _preparation_data.vertex_count; ++i)
			(*pRemapArray)[i] = i;

	} // End if supplied

	  // Pre-compute surface normals for each triangle
	std::uint8_t * src_vertices_ptr = &_preparation_data.vertex_data[0];
	math::vec3 * pNormals = new math::vec3[_preparation_data.triangle_count];
	memset(pNormals, 0, _preparation_data.triangle_count * sizeof(math::vec3));
	for (i = 0; i < _preparation_data.triangle_count; ++i)
	{
		// Retrieve positions of each referenced vertex.
		const Triangle & Tri = _preparation_data.triangle_data[i];
		const math::vec3 * v1 = (math::vec3*)(src_vertices_ptr + (Tri.indices[0] * vertex_stride) + position_offset);
		const math::vec3 * v2 = (math::vec3*)(src_vertices_ptr + (Tri.indices[1] * vertex_stride) + position_offset);
		const math::vec3 * v3 = (math::vec3*)(src_vertices_ptr + (Tri.indices[2] * vertex_stride) + position_offset);

		// Compute the two edge vectors required for generating our normal
		// We normalize here to prevent problems when the triangles are very small.
		vecEdge1 = math::normalize(*v2 - *v1);
		vecEdge2 = math::normalize(*v3 - *v1);

		// Generate the normal
		vec_normal = math::cross(vecEdge1, vecEdge2);
		pNormals[i] = math::normalize(vec_normal);

	} // Next Face

	  // Now compute the actual VERTEX normals using face adjacency information
	for (i = 0; i < _preparation_data.triangle_count; ++i)
	{
		Triangle & Tri = _preparation_data.triangle_data[i];
		if (Tri.flags & TriangleFlags::Degenerate)
			continue;

		// Process each vertex in the face
		for (j = 0; j < 3; ++j)
		{
			// Retrieve the index for this vertex.
			index = Tri.indices[j];

			// Skip this vertex if normal information was already provided.
			if (!_force_normal_generation && (_preparation_data.vertex_flags[index] & PreparationData::SourceContainsNormal))
				continue;

			// To generate vertex normals using the adjacency information we first need to walk backwards
			// through the list to find the first triangle that references this vertex (using entrance/exit edge strategy).
			// Once we have the first triangle, step forwards and sum the normals of each of the faces
			// for each triangle we touch. This is essentially a flood fill through all of the triangles
			// that touch this vertex, without ever having to test the entire set for shared vertices.
			// The initial backwards traversal prevents us from having to store (and test) a 'visited' flag for
			// every triangle in the buffer.

			// First walk backwards...
			nStartTri = i;
			nPreviousTri = i;
			nCurrentTri = pAdjacency[(i * 3) + ((j + 2) % 3)];
			for (; ; )
			{
				// Stop walking if we reach the starting triangle again, or if there
				// is no connectivity out of this edge
				if (nCurrentTri == nStartTri || nCurrentTri == 0xFFFFFFFF)
					break;

				// Find the edge in the adjacency list that we came in through
				for (k = 0; k < 3; ++k)
				{
					if (pAdjacency[(nCurrentTri * 3) + k] == nPreviousTri)
						break;

				} // Next item in adjacency list

				  // If we found the edge we entered through, the exit edge will
				  // be the edge counter-clockwise from this one when walking backwards
				if (k < 3)
				{
					nPreviousTri = nCurrentTri;
					nCurrentTri = pAdjacency[(nCurrentTri * 3) + ((k + 2) % 3)];

				} // End if found entrance edge
				else
				{
					break;

				} // End if failed to find entrance edge

			} // Next Test

			  // We should now be at the starting triangle, we can start to walk forwards
			  // collecting the face normals. First find the exit edge so we can start walking.
			if (nCurrentTri != 0xFFFFFFFF)
			{
				for (k = 0; k < 3; ++k)
				{
					if (pAdjacency[(nCurrentTri * 3) + k] == nPreviousTri)
						break;

				} // Next item in adjacency list
			}
			else
			{
				// Couldn't step back, so first triangle is the current triangle
				nCurrentTri = i;
				k = j;
			}

			if (k < 3)
			{
				nStartTri = nCurrentTri;
				nPreviousTri = nCurrentTri;
				nCurrentTri = pAdjacency[(nCurrentTri * 3) + k];
				vec_normal = pNormals[nStartTri];
				for (; ; )
				{
					// Stop walking if we reach the starting triangle again, or if there
					// is no connectivity out of this edge
					if (nCurrentTri == nStartTri || nCurrentTri == 0xFFFFFFFF)
						break;

					// Add this normal.
					vec_normal += pNormals[nCurrentTri];

					// Find the edge in the adjacency list that we came in through
					for (k = 0; k < 3; ++k)
					{
						if (pAdjacency[(nCurrentTri * 3) + k] == nPreviousTri)
							break;

					} // Next item in adjacency list

					  // If we found the edge we came entered through, the exit edge will
					  // be the edge clockwise from this one when walking forwards
					if (k < 3)
					{
						nPreviousTri = nCurrentTri;
						nCurrentTri = pAdjacency[(nCurrentTri * 3) + ((k + 1) % 3)];

					} // End if found entrance edge
					else
					{
						break;

					} // End if failed to find entrance edge

				} // Next Test

			} // End if found entrance edge

			  // Normalize the new vertex normal
			vec_normal = math::normalize(vec_normal);

			// If the normal we are about to store is significantly different from any normal
			// already stored in this vertex (excepting the case where it is <0,0,0>), we need
			// to split the vertex into two.
			math::vec3 * pRefNormal = (math::vec3*)(src_vertices_ptr + (index * vertex_stride) + normal_offset);
			if (pRefNormal->x == 0.0f && pRefNormal->y == 0.0f && pRefNormal->z == 0.0f)
			{
				*pRefNormal = vec_normal;

			} // End if no normal stored here yet
			else
			{
				// Split and store in a new vertex if it is different (enough)
				if (math::abs(pRefNormal->x - vec_normal.x) >= 1e-3f ||
					math::abs(pRefNormal->y - vec_normal.y) >= 1e-3f ||
					math::abs(pRefNormal->z - vec_normal.z) >= 1e-3f)
				{
					// Make room for new vertex data.
					_preparation_data.vertex_data.resize(_preparation_data.vertex_data.size() + vertex_stride);

					// Ensure that we update the 'src_vertices_ptr' pointer (used throughout the
					// loop). The internal buffer wrapped by the resized vertex data vector
					// may have been re-allocated.
					src_vertices_ptr = &_preparation_data.vertex_data[0];

					// Duplicate the vertex at the end of the buffer
					memcpy(src_vertices_ptr + (_preparation_data.vertex_count * vertex_stride),
						src_vertices_ptr + (index * vertex_stride),
						vertex_stride);

					// Duplicate any other remaining information.
					_preparation_data.vertex_flags.push_back(_preparation_data.vertex_flags[index]);

					// Record the split
					if (pRemapArray)
						(*pRemapArray)[index] = _preparation_data.vertex_count;

					// Store the new normal and finally record the fact that we have
					// added a new vertex.
					index = _preparation_data.vertex_count++;
					pRefNormal = (math::vec3*)(src_vertices_ptr + (index * vertex_stride) + normal_offset);
					*pRefNormal = vec_normal;

					// Update the index
					Tri.indices[j] = index;

				} // End if normal is different

			} // End if normal already stored here

		} // Next Vertex

	} // Next Face

	// We're done with the surface normals
	checked_array_delete(pNormals);

	// If no new vertices were introduced, then it is not necessary
	// for the caller to remap anything.
	if (pRemapArray && original_vertex_count == _preparation_data.vertex_count)
		pRemapArray->clear();

	// Success!
	return true;
}

bool Mesh::generate_vertex_barycentrics(std::uint32_t* adjacency)
{

	return true;
}

//-----------------------------------------------------------------------------
//  Name : generateVertexTangents () 
/// <summary>
/// Builds the tangent space vectors for this polygon. 
/// Credit to Terathon Software - http://www.terathon.com/code/tangent.html 
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::generate_vertex_tangents()
{
	math::vec3* tangents = nullptr, *bitangents = nullptr;
	std::uint32_t i, i1, i2, i3, num_faces, num_verts;
	math::vec3 P, Q, T, B, vCross, vNormal;
	float s1, t1, s2, t2, r;
	math::plane Plane;

	// Get access to useful data offset information.
	std::uint16_t vertex_stride = _vertex_format.getStride();


	bool has_normals = _vertex_format.has(gfx::Attrib::Normal);
	// This will fail if we don't already have normals however.
	if (!has_normals)
		return false;

	// Final format requests tangents?
	bool requires_tangents = _vertex_format.has(gfx::Attrib::Tangent);
	bool requires_bitangents = _vertex_format.has(gfx::Attrib::Bitangent);
	if (!_force_tangent_generation && !requires_bitangents && !requires_tangents)
		return true;

	// Allocate storage space for the tangent and binormal vectors
	// that we will effectively need to average for shared vertices.
	num_faces = _preparation_data.triangle_count;
	num_verts = _preparation_data.vertex_count;
	tangents = new math::vec3[num_verts];
	bitangents = new math::vec3[num_verts];
	memset(tangents, 0, sizeof(math::vec3) * num_verts);
	memset(bitangents, 0, sizeof(math::vec3) * num_verts);

	// Iterate through each triangle in the mesh
	std::uint8_t * src_vertices_ptr = &_preparation_data.vertex_data[0];
	for (i = 0; i < num_faces; ++i)
	{
		Triangle& tri = _preparation_data.triangle_data[i];

		// Compute the three indices for the triangle
		i1 = tri.indices[0];
		i2 = tri.indices[1];
		i3 = tri.indices[2];

		// Retrieve references to the positions of the three vertices in the triangle.
		math::vec3 E;
		float fE[4];
		gfx::vertexUnpack(fE, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, i1);
		math::vec3 F;
		float fF[4];
		gfx::vertexUnpack(fF, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, i2);
		math::vec3 G;
		float fG[4];
		gfx::vertexUnpack(fG, gfx::Attrib::Position, _vertex_format, src_vertices_ptr, i3);
		memcpy(&E[0], fE, 3 * sizeof(float));
		memcpy(&F[0], fF, 3 * sizeof(float));
		memcpy(&G[0], fG, 3 * sizeof(float));

		// Retrieve references to the base texture coordinates of the three vertices in the triangle.
		// TODO: Allow customization of which tex coordinates to generate from.
		math::vec2 Et;
		float fEt[4];
		gfx::vertexUnpack(&fEt[0], gfx::Attrib::TexCoord0, _vertex_format, src_vertices_ptr, i1);
		math::vec2 Ft;
		float fFt[4];
		gfx::vertexUnpack(&fFt[0], gfx::Attrib::TexCoord0, _vertex_format, src_vertices_ptr, i2);
		math::vec2 Gt;
		float fGt[4];
		gfx::vertexUnpack(&fGt[0], gfx::Attrib::TexCoord0, _vertex_format, src_vertices_ptr, i3);
		memcpy(&Et[0], fEt, 2 * sizeof(float));
		memcpy(&Ft[0], fFt, 2 * sizeof(float));
		memcpy(&Gt[0], fGt, 2 * sizeof(float));

		// Compute the known variables P & Q, where "P = F-E" and "Q = G-E"
		// based on our original discussion of the tangent vector
		// calculation.
		P = F - E;
		Q = G - E;

		// Also compute the know variables <s1,t1> and <s2,t2>. Recall that
		// these are the texture coordinate deltas similarly for "F-E"
		// and "G-E".
		s1 = Ft.x - Et.x;
		t1 = Ft.y - Et.y;
		s2 = Gt.x - Et.x;
		t2 = Gt.y - Et.y;

		// Next we can pre-compute part of the equation we developed
		// earlier: "1/(s1 * t2 - s2 * t1)". We do this in two separate
		// stages here in order to ensure that the texture coordinates
		// are not invalid.
		r = (s1 * t2 - s2 * t1);
		if (math::abs(r) < math::epsilon<float>())
			continue;
		r = 1.0f / r;

		// All that's left for us to do now is to run the matrix
		// multiplication and multiply the result by the scalar portion
		// we precomputed earlier.
		T.x = r * (t2 * P.x - t1 * Q.x);
		T.y = r * (t2 * P.y - t1 * Q.y);
		T.z = r * (t2 * P.z - t1 * Q.z);
		B.x = r * (s1 * Q.x - s2 * P.x);
		B.y = r * (s1 * Q.y - s2 * P.y);
		B.z = r * (s1 * Q.z - s2 * P.z);

		// Add the tangent and binormal vectors (summed average) to
		// any previous values computed for each vertex.
		tangents[i1] += T;
		tangents[i2] += T;
		tangents[i3] += T;
		bitangents[i1] += B;
		bitangents[i2] += B;
		bitangents[i3] += B;

	} // Next Triangle 

	  // Generate final tangent vectors 
	for (i = 0; i < num_verts; i++, src_vertices_ptr += vertex_stride)
	{
		// Skip if the original imported data already provided a binormal / tangent.
		bool has_bitangent = ((_preparation_data.vertex_flags[i] & PreparationData::SourceContainsBinormal) != 0);
		bool has_tangent = ((_preparation_data.vertex_flags[i] & PreparationData::SourceContainsTangent) != 0);
		if (!_force_tangent_generation && has_bitangent && has_tangent)
			continue;

		// Retrieve the normal vector from the vertex and the computed
		// tangent vector.
		float normal[4];
		gfx::vertexUnpack(normal, gfx::Attrib::Normal, _vertex_format, src_vertices_ptr);
		memcpy(&vNormal[0], normal, 3 * sizeof(float));

		T = tangents[i];

		// GramSchmidt orthogonalize
		T = T - (vNormal * math::dot(vNormal, T));
		T = math::normalize(T);

		// Store tangent if required
		if (_force_tangent_generation || (!has_tangent && requires_tangents))
			gfx::vertexPack(&math::vec4(T, 1.0f)[0], true, gfx::Attrib::Tangent, _vertex_format, src_vertices_ptr);

		// Compute and store binormal if required
		if (_force_tangent_generation || (!has_bitangent && requires_bitangents))
		{
			// Calculate the new orthogonal binormal
			B = math::cross(vNormal, T);
			B = math::normalize(B);

			// Compute the "handedness" of the tangent and binormal. This
			// ensures the inverted / mirrored texture coordinates still have
			// an accurate matrix.
			vCross = math::cross(vNormal, T);
			if (math::dot(vCross, bitangents[i]) < 0.0f)
			{
				// Flip the binormal
				B = -B;

			} // End if coordinates inverted

			  // Store.
			gfx::vertexPack(&math::vec4(B, 1.0f)[0], true, gfx::Attrib::Bitangent, _vertex_format, src_vertices_ptr);

		} // End if requires binormal   

	} // Next vertex

	  // Cleanup 
	checked_array_delete(tangents);
	checked_array_delete(bitangents);

	// Return success 
	return true;
}

//-----------------------------------------------------------------------------
//  Name : generateAdjacency ()
/// <summary>
/// Generates edge-triangle adjacency information for the mesh data either
/// prior to, or after building the hardware buffers. Input array will be
/// automatically sized, and will contain 3 values per triangle contained
/// in the mesh representing the indices to adjacent faces for each edge in 
/// the triangle (or 0xFFFFFFFF if there is no adjacent face).
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::generate_adjacency(UInt32Array& adjacency)
{
	std::map< AdjacentEdgeKey, std::uint32_t > EdgeTree;
	std::map< AdjacentEdgeKey, std::uint32_t >::iterator itEdge;

	// What is the status of the mesh?
	if (_prepare_status != MeshStatus::Prepared)
	{
		// Validate requirements
		if (_preparation_data.triangle_count == 0)
			return false;

		// Retrieve useful data offset information.
		std::uint16_t position_offset = _vertex_format.getOffset(gfx::Attrib::Position);
		std::uint16_t vertex_stride = _vertex_format.getStride();

		// Insert all edges into the edge tree
		std::uint8_t * src_vertices_ptr = &_preparation_data.vertex_data[0] + position_offset;
		for (std::uint32_t i = 0; i < _preparation_data.triangle_count; ++i)
		{
			AdjacentEdgeKey Edge;

			// Degenerate triangles cannot participate.
			const Triangle & Tri = _preparation_data.triangle_data[i];
			if (Tri.flags & TriangleFlags::Degenerate)
				continue;

			// Retrieve positions of each referenced vertex.
			const math::vec3 * v1 = (math::vec3*)(src_vertices_ptr + (Tri.indices[0] * vertex_stride));
			const math::vec3 * v2 = (math::vec3*)(src_vertices_ptr + (Tri.indices[1] * vertex_stride));
			const math::vec3 * v3 = (math::vec3*)(src_vertices_ptr + (Tri.indices[2] * vertex_stride));

			// Edge 1
			Edge.vertex1 = v1;
			Edge.vertex2 = v2;
			EdgeTree[Edge] = i;

			// Edge 2
			Edge.vertex1 = v2;
			Edge.vertex2 = v3;
			EdgeTree[Edge] = i;

			// Edge 3
			Edge.vertex1 = v3;
			Edge.vertex2 = v1;
			EdgeTree[Edge] = i;

		} // Next Face

		  // Size the output array.
		adjacency.resize(_preparation_data.triangle_count * 3, 0xFFFFFFFF);

		// Now, find any adjacent edges for each triangle edge
		for (std::uint32_t i = 0; i < _preparation_data.triangle_count; ++i)
		{
			AdjacentEdgeKey Edge;

			// Degenerate triangles cannot participate.
			const Triangle & Tri = _preparation_data.triangle_data[i];
			if (Tri.flags & TriangleFlags::Degenerate)
				continue;

			// Retrieve positions of each referenced vertex.
			const math::vec3 * v1 = (math::vec3*)(src_vertices_ptr + (Tri.indices[0] * vertex_stride));
			const math::vec3 * v2 = (math::vec3*)(src_vertices_ptr + (Tri.indices[1] * vertex_stride));
			const math::vec3 * v3 = (math::vec3*)(src_vertices_ptr + (Tri.indices[2] * vertex_stride));

			// Note: Notice below that the order of the edge vertices
			//       is swapped. This is because we want to find the 
			//       matching ADJACENT edge, rather than simply finding
			//       the same edge that we're currently processing.

			// Edge 1
			Edge.vertex2 = v1;
			Edge.vertex1 = v2;

			// Find the matching adjacent edge
			itEdge = EdgeTree.find(Edge);
			if (itEdge != EdgeTree.end())
				adjacency[(i * 3)] = itEdge->second;

			// Edge 2
			Edge.vertex2 = v2;
			Edge.vertex1 = v3;

			// Find the matching adjacent edge
			itEdge = EdgeTree.find(Edge);
			if (itEdge != EdgeTree.end())
				adjacency[(i * 3) + 1] = itEdge->second;

			// Edge 3
			Edge.vertex2 = v3;
			Edge.vertex1 = v1;

			// Find the matching adjacent edge
			itEdge = EdgeTree.find(Edge);
			if (itEdge != EdgeTree.end())
				adjacency[(i * 3) + 2] = itEdge->second;

		} // Next Face

	} // End if not prepared
	else
	{
		// Validate requirements
		if (_face_count == 0)
			return false;

		// Retrieve useful data offset information.
		std::uint16_t position_offset = _vertex_format.getOffset(gfx::Attrib::Position);
		std::uint16_t vertex_stride = _vertex_format.getStride();

		// Insert all edges into the edge tree
		std::uint8_t * src_vertices_ptr = _system_vb + position_offset;
		std::uint32_t * pSrcIndices = _system_ib;
		for (std::uint32_t i = 0; i < _face_count; ++i, pSrcIndices += 3)
		{
			AdjacentEdgeKey Edge;

			// Retrieve positions of each referenced vertex.
			const math::vec3 * v1 = (math::vec3*)(src_vertices_ptr + (pSrcIndices[0] * vertex_stride));
			const math::vec3 * v2 = (math::vec3*)(src_vertices_ptr + (pSrcIndices[1] * vertex_stride));
			const math::vec3 * v3 = (math::vec3*)(src_vertices_ptr + (pSrcIndices[2] * vertex_stride));

			// Edge 1
			Edge.vertex1 = v1;
			Edge.vertex2 = v2;
			EdgeTree[Edge] = i;

			// Edge 2
			Edge.vertex1 = v2;
			Edge.vertex2 = v3;
			EdgeTree[Edge] = i;

			// Edge 3
			Edge.vertex1 = v3;
			Edge.vertex2 = v1;
			EdgeTree[Edge] = i;

		} // Next Face

		  // Size the output array.
		adjacency.resize(_face_count * 3, 0xFFFFFFFF);

		// Now, find any adjacent edges for each triangle edge
		pSrcIndices = _system_ib;
		for (std::uint32_t i = 0; i < _face_count; ++i, pSrcIndices += 3)
		{
			AdjacentEdgeKey Edge;

			// Retrieve positions of each referenced vertex.
			const math::vec3 * v1 = (math::vec3*)(src_vertices_ptr + (pSrcIndices[0] * vertex_stride));
			const math::vec3 * v2 = (math::vec3*)(src_vertices_ptr + (pSrcIndices[1] * vertex_stride));
			const math::vec3 * v3 = (math::vec3*)(src_vertices_ptr + (pSrcIndices[2] * vertex_stride));

			// Note: Notice below that the order of the edge vertices
			//       is swapped. This is because we want to find the 
			//       matching ADJACENT edge, rather than simply finding
			//       the same edge that we're currently processing.

			// Edge 1
			Edge.vertex2 = v1;
			Edge.vertex1 = v2;

			// Find the matching adjacent edge
			itEdge = EdgeTree.find(Edge);
			if (itEdge != EdgeTree.end())
				adjacency[(i * 3)] = itEdge->second;

			// Edge 2
			Edge.vertex2 = v2;
			Edge.vertex1 = v3;

			// Find the matching adjacent edge
			itEdge = EdgeTree.find(Edge);
			if (itEdge != EdgeTree.end())
				adjacency[(i * 3) + 1] = itEdge->second;

			// Edge 3
			Edge.vertex2 = v3;
			Edge.vertex1 = v1;

			// Find the matching adjacent edge
			itEdge = EdgeTree.find(Edge);
			if (itEdge != EdgeTree.end())
				adjacency[(i * 3) + 2] = itEdge->second;

		} // Next Face

	} // End if prepared

	  // Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : weldVertices ()
/// <summary>
/// Weld all of the vertices together that can be combined.
/// </summary>
//-----------------------------------------------------------------------------
bool Mesh::weld_vertices(float tolerance, UInt32Array* vertex_remap_ptr /* = nullptr */)
{
	WeldKey Key;
	std::map< WeldKey, std::uint32_t > vertex_tree;
	std::map< WeldKey, std::uint32_t >::const_iterator itKey;
	ByteArray new_vertex_data, new_vertex_flags;
	std::uint32_t new_vertex_count = 0;

	// Allocate enough space to build the remap array for the existing vertices
	if (vertex_remap_ptr)
		vertex_remap_ptr->resize(_preparation_data.vertex_count);
	std::uint32_t* collapse_map = new std::uint32_t[_preparation_data.vertex_count];

	// Retrieve useful data offset information.
	std::uint16_t vertex_stride = _vertex_format.getStride();

	// For each vertex to be welded.
	for (std::uint32_t i = 0; i < _preparation_data.vertex_count; ++i)
	{
		// Build a new key structure for inserting
		Key.vertex = (&_preparation_data.vertex_data[0]) + (i * vertex_stride);
		Key.format = _vertex_format;
		Key.tolerance = tolerance;

		// Does a vertex with matching details already exist in the tree.
		itKey = vertex_tree.find(Key);
		if (itKey == vertex_tree.end())
		{
			// No matching vertex. Insert into the tree (value = NEW index of vertex).
			vertex_tree[Key] = new_vertex_count;
			collapse_map[i] = new_vertex_count;
			if (vertex_remap_ptr)
				(*vertex_remap_ptr)[i] = new_vertex_count;

			// Store the vertex in the new buffer
			new_vertex_data.resize((new_vertex_count + 1) * vertex_stride);
			memcpy(&new_vertex_data[new_vertex_count * vertex_stride], Key.vertex, vertex_stride);
			new_vertex_flags.push_back(_preparation_data.vertex_flags[i]);
			new_vertex_count++;

		} // End if no matching vertex
		else
		{
			// A vertex already existed at this location.
			// Just mark the 'collapsed' index for this vertex in the remap array.
			collapse_map[i] = itKey->second;
			if (vertex_remap_ptr)
				(*vertex_remap_ptr)[i] = 0xFFFFFFFF;

		} // End if vertex already existed

	} // Next Vertex

	  // If nothing was welded, just bail
	if (_preparation_data.vertex_count == new_vertex_count)
	{
		checked_array_delete(collapse_map);

		if (vertex_remap_ptr)
			vertex_remap_ptr->clear();
		return true;

	} // End if nothing to do

	  // Otherwise, replace the old preparation vertices and remap
	_preparation_data.vertex_data.clear();
	_preparation_data.vertex_data.resize(new_vertex_data.size());
	memcpy(&_preparation_data.vertex_data[0], &new_vertex_data[0], new_vertex_data.size());
	_preparation_data.vertex_flags.clear();
	_preparation_data.vertex_flags.resize(new_vertex_flags.size());
	memcpy(&_preparation_data.vertex_flags[0], &new_vertex_flags[0], new_vertex_flags.size());
	_preparation_data.vertex_count = new_vertex_count;

	// Now remap all the triangle indices
	for (std::uint32_t i = 0; i < _preparation_data.triangle_count; ++i)
	{
		Triangle& tri = _preparation_data.triangle_data[i];
		tri.indices[0] = collapse_map[tri.indices[0]];
		tri.indices[1] = collapse_map[tri.indices[1]];
		tri.indices[2] = collapse_map[tri.indices[2]];

	} // Next Triangle

	  // Clean up
	checked_array_delete(collapse_map);

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : getFaceCount ()
/// <summary>
/// Determine the number of faces stored here. If the mesh has already
/// been finalized with a call to endPrepare(), this will be the total
/// number of triangles stored. Otherwise, this will be the number of
/// windings maintained within the mesh ready for preparation.
/// </summary>
//-----------------------------------------------------------------------------
std::uint32_t Mesh::get_face_count() const
{
	if (_prepare_status == MeshStatus::Prepared)
		return _face_count;
	else if (_prepare_status == MeshStatus::Preparing)
		return (std::uint32_t)_preparation_data.triangle_data.size();
	else
		return 0;
}

//-----------------------------------------------------------------------------
//  Name : getVertexCount ()
/// <summary>
/// Determine the number of vertices stored here. If the mesh has already
/// been finalized with a call to endPrepare(), this will be the total
/// number of vertices stored. Otherwise, this will be the number of
/// vertices currently maintained within the mesh ready for preparation.
/// </summary>
//-----------------------------------------------------------------------------
std::uint32_t Mesh::get_vertex_count() const
{
	if (_prepare_status == MeshStatus::Prepared)
		return _vertex_count;
	else if (_prepare_status == MeshStatus::Preparing)
		return _preparation_data.vertex_count;
	else
		return 0;
}

//-----------------------------------------------------------------------------
//  Name : getSystemVB ()
/// <summary>
/// Retrieve the underlying vertex data from the mesh.
/// </summary>
//-----------------------------------------------------------------------------
std::uint8_t* Mesh::get_system_vb()
{
	return _system_vb;
}

//-----------------------------------------------------------------------------
//  Name : getSystemIB ()
/// <summary>
/// Retrieve the underlying index data from the mesh.
/// </summary>
//-----------------------------------------------------------------------------
std::uint32_t* Mesh::get_system_ib()
{
	return _system_ib;
}

//-----------------------------------------------------------------------------
//  Name : getVertexFormat ()
/// <summary>
/// Retrieve the format of the underlying mesh vertex data.
/// </summary>
//-----------------------------------------------------------------------------
const gfx::VertexDecl& Mesh::get_vertex_format() const
{
	return _vertex_format;
}

//-----------------------------------------------------------------------------
//  Name : getSubset ()
/// <summary>
/// Retrieve information about the subset of the mesh that is associated with
/// the specified material and data group identifier.
/// </summary>
//-----------------------------------------------------------------------------
const Mesh::Subset * Mesh::get_subset(std::uint32_t dataGroupId /* = 0 */) const
{
	auto itSubset = _subset_lookup.find(MeshSubsetKey(dataGroupId));
	if (itSubset == _subset_lookup.end())
		return nullptr;
	return itSubset->second;
}

//-----------------------------------------------------------------------------
//  Name : getSkinBindData ()
/// <summary>
/// If this mesh has been bound as a skin, this method can be called to
/// retrieve the original data that was used to bind it.
/// </summary>
//-----------------------------------------------------------------------------
const SkinBindData& Mesh::get_skin_bind_data() const
{
	return _skin_bind_data;
}

//-----------------------------------------------------------------------------
//  Name : getBonePalettes ()
/// <summary>
/// If this mesh has been bound as a skin, this method can be called to
/// retrieve the compiled bone combination palette data.
/// </summary>
//-----------------------------------------------------------------------------
const Mesh::BonePaletteArray& Mesh::get_bone_palettes() const
{
	return _bone_palettes;
}

///////////////////////////////////////////////////////////////////////////////
// SkinBindData Member Definitions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : addBone()
/// <summary>
/// Add influence information for a specific bone.
/// </summary>
//-----------------------------------------------------------------------------
void SkinBindData::add_bone(const BoneInfluence& bone)
{
	_bones.push_back(bone);
}

//-----------------------------------------------------------------------------
//  Name : removeEmptyBones()
/// <summary>
/// Strip out any bones that did not contain any influences.
/// </summary>
//-----------------------------------------------------------------------------
void SkinBindData::remove_empty_bones()
{
	for (size_t i = 0; i < _bones.size(); )
	{
		if (_bones[i].influences.empty())
		{
			//checked_delete(mBones[i]);
			_bones.erase(_bones.begin() + i);

		} // End if empty
		else
			++i;

	} // Next Bone
}

//-----------------------------------------------------------------------------
//  Name : clearVertexInfluences()
/// <summary>
/// Release memory allocated for vertex influences in each stored bone.
/// </summary>
//-----------------------------------------------------------------------------
void SkinBindData::clear_vertex_influences()
{
	for (size_t i = 0; i < _bones.size(); ++i)
		_bones[i].influences.clear();
}

void SkinBindData::clear()
{
	_bones.clear();
}

//-----------------------------------------------------------------------------
//  Name : remapVertices()
/// <summary>
/// Remap the vertex references stored in the binding based on the supplied
/// remap array (Array[old vertex index] = new vertex index). Store an index
/// of 0xFFFFFFFF to indicate that the vertex was removed, or use an index
/// greater than the remap array size to indicate the new location of a split
/// vertex.
/// </summary>
//-----------------------------------------------------------------------------
void SkinBindData::remapVertices(const UInt32Array & Remap)
{
	// Iterate through all bone information and remap vertex indices.
	for (size_t i = 0; i < _bones.size(); ++i)
	{
		VertexInfluenceArray NewInfluences;
		VertexInfluenceArray &Influences = _bones[i].influences;
		NewInfluences.reserve(Influences.size());
		for (size_t j = 0; j < Influences.size(); ++j)
		{
			std::uint32_t new_index = Remap[Influences[j].vertex_index];
			if (new_index != 0xFFFFFFFF)
			{
				// Insert an influence at the new index
				NewInfluences.push_back(VertexInfluence(new_index, Influences[j].weight));

				// If the vertex was split into two, we want to retain an
				// influence to the original index too.
				if (new_index >= Remap.size())
					NewInfluences.push_back(VertexInfluence(Influences[j].vertex_index, Influences[j].weight));

			} // End if !removed

		} // Next source influence
		_bones[i].influences = NewInfluences;

	} // Next bone
}

//-----------------------------------------------------------------------------
//  Name : buildVertexTable()
/// <summary>
/// Construct a list of bone influences and weights for each vertex based 
/// on the binding data provided.
/// </summary>
//-----------------------------------------------------------------------------
void SkinBindData::build_vertex_table(std::uint32_t nVertexCount, const UInt32Array & VertexRemap, BindVertexArray& Table)
{
	std::uint32_t nVertex;

	// Initialize the vertex table with the required number of vertices.
	Table.reserve(nVertexCount);
	for (nVertex = 0; nVertex < nVertexCount; ++nVertex)
	{
		VertexData data;
		data.palette = -1;
		data.original_vertex = nVertex;
		Table.push_back(data);

	} // Next Vertex

	  // Iterate through all bone information and populate the above array.
	for (size_t i = 0; i < _bones.size(); ++i)
	{
		VertexInfluenceArray& Influences = _bones[i].influences;
		for (size_t j = 0; j < Influences.size(); ++j)
		{

			// Vertex data has been remapped?
			if (VertexRemap.size() > 0)
			{
				nVertex = VertexRemap[Influences[j].vertex_index];
				if (nVertex == 0xFFFFFFFF) continue;
				auto& data = Table[nVertex];
				// Push influence data.
				data.influences.push_back((std::int32_t)i);
				data.weights.push_back(Influences[j].weight);
			} // End if remap
			else
			{
				auto& data = Table[Influences[j].vertex_index];
				// Push influence data.
				data.influences.push_back((std::int32_t)i);
				data.weights.push_back(Influences[j].weight);
			}

		} // Next Influence

	} // Next Bone
}

//-----------------------------------------------------------------------------
//  Name : getBones ()
/// <summary>
/// Retrieve a list of all bones that influence the skin in some way.
/// </summary>
//-----------------------------------------------------------------------------
const SkinBindData::BoneArray & SkinBindData::get_bones() const
{
	return _bones;
}

//-----------------------------------------------------------------------------
//  Name : getBones ()
/// <summary>
/// Retrieve a list of all bones that influence the skin in some way.
/// </summary>
//-----------------------------------------------------------------------------
SkinBindData::BoneArray & SkinBindData::get_bones()
{
	return _bones;
}

bool SkinBindData::has_bones() const
{
	return !get_bones().empty();
}

const SkinBindData::BoneInfluence* SkinBindData::find_bone_by_id(const std::string& name) const
{
	auto it = std::find_if(std::begin(_bones), std::end(_bones),
		[name](const auto& bone)
	{
		return name == bone.bone_id;
	});
	if (it != std::end(_bones))
	{
		return &(*it);
	}


	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
// BonePalette Member Definitions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : BonePalette() (Constructor)
/// <summary>
/// Class constructor.
/// </summary>
//-----------------------------------------------------------------------------
BonePalette::BonePalette(std::uint32_t palette_size)
{
	// Initialize variables to sensible defaults
	_data_group_id = 0;
	_maximum_size = palette_size;
	_maximum_blend_index = -1;
}

//-----------------------------------------------------------------------------
//  Name : BonePalette() (Copy Constructor)
/// <summary>
/// Class copy constructor.
/// </summary>
//-----------------------------------------------------------------------------
BonePalette::BonePalette(const BonePalette & Init)
{
	_bones_lut = Init._bones_lut;
	_bones = Init._bones;
	_faces = Init._faces;
	_data_group_id = Init._data_group_id;
	_maximum_size = Init._maximum_size;
	_maximum_blend_index = Init._maximum_blend_index;
}

//-----------------------------------------------------------------------------
//  Name : ~BonePalette() (Destructor)
/// <summary>
/// Clean up any resources being used.
/// </summary>
//-----------------------------------------------------------------------------
BonePalette::~BonePalette()
{
}

//-----------------------------------------------------------------------------
//  Name : apply()
/// <summary>
/// Apply the bone / palette information to the render driver ready for
/// drawing the skinned mesh.
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::apply(const std::vector<math::transform_t> & boneTransforms, const SkinBindData * const pBindData, bool bComputeInverseTranspose)
{
	// Retrieve the main list of bones from the skin bind data that will
	// be referenced by the palette's bone index list.
	const SkinBindData::BoneArray & BindList = pBindData->get_bones();
	if (boneTransforms.empty())
		return;

	// 	RenderDriver& driver = Singleton<RenderDriver>::getInstance();
	// 	const std::uint32_t maxBlendTransforms = driver.getCapabilities()->getMaxBlendTransforms();
	// 	Matrix4x4::Array Transforms;
	// 	Transforms.resize(maxBlendTransforms);
	// 	// Compute transformation matrix for each bone in the palette
	// 	if (bComputeInverseTranspose)
	// 	{
	// 		Matrix4x4::Array ITTransforms;
	// 		ITTransforms.resize(maxBlendTransforms);
	// 
	// 		for (size_t i = 0; i < mBones.size(); ++i)
	// 		{
	// 			auto& pBoneTransform = boneTransforms[mBones[i]];
	// 			const SkinBindData::BoneInfluence* pBoneData = BindList[mBones[i]];
	// 
	// 			Transforms[i] = (pBoneData->bindPoseTransform * pBoneTransform).matrix();
	// 
	// 			// Compute inverse transpose
	// 			ITTransforms[i] = Transforms[i];
	// 			(Vector4&)ITTransforms[i]._41 = Vector4(0, 0, 0, 1);
	// 			ITTransforms[i] = Matrix4x4::inverse(ITTransforms[i]);
	// 			ITTransforms[i] = Matrix4x4::transpose(ITTransforms[i]);
	// 
	// 		} // Next Bone
	// 
	// 		  // Set to the device
	// 		driver.setVertexBlendData(&Transforms[0], &ITTransforms[0], (std::uint32_t)mBones.size(), mMaximumBlendIndex);
	// 
	// 	} // End if bComputeInverseTranspose
	// 	else
	// 	{
	// 
	// 		for (size_t i = 0; i < mBones.size(); ++i)
	// 		{
	// 			auto& pBoneTransform = boneTransforms[mBones[i]];
	// 			const SkinBindData::BoneInfluence* pBoneData = BindList[mBones[i]];
	// 
	// 			Transforms[i] = (pBoneData->bindPoseTransform * pBoneTransform).matrix();
	// 
	// 		} // Next Bone
	// 
	// 		  // Set to the device
	// 		driver.setVertexBlendData(&Transforms[0], nullptr, (std::uint32_t)mBones.size(), mMaximumBlendIndex);
	// 
	// 	} // End if !bComputeInverseTranspose

}

//-----------------------------------------------------------------------------
//  Name : assignBones()
/// <summary>
/// Assign the specified bones (and faces) to this bone palette.
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::assign_bones(BoneIndexMap & Bones, UInt32Array & faces)
{
	BoneIndexMap::iterator itBone, itBone2;

	// Iterate through newly specified input bones and add any unique ones to the palette.
	for (itBone = Bones.begin(); itBone != Bones.end(); ++itBone)
	{
		itBone2 = _bones_lut.find(itBone->first);
		if (itBone2 == _bones_lut.end())
		{
			_bones_lut[itBone->first] = (std::uint32_t)_bones.size();
			_bones.push_back(itBone->first);

		} // End if not already added

	} // Next Bone

	  // Merge the new face list with ours.
	_faces.insert(_faces.end(), faces.begin(), faces.end());
}

//-----------------------------------------------------------------------------
//  Name : assignBones()
/// <summary>
/// Assign the specified bones to this bone palette.
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::assign_bones(const UInt32Array & Bones)
{
	BoneIndexMap::iterator itBone;

	// Clear out prior data.
	_bones.clear();
	_bones_lut.clear();

	// Iterate through newly specified input bones and add any unique ones to the palette.
	for (size_t i = 0; i < Bones.size(); ++i)
	{
		itBone = _bones_lut.find(Bones[i]);
		if (itBone == _bones_lut.end())
		{
			_bones_lut[Bones[i]] = (std::uint32_t)_bones.size();
			_bones.push_back(Bones[i]);

		} // End if not already added

	} // Next Bone
}

//-----------------------------------------------------------------------------
//  Name : computePaletteFit()
/// <summary>
/// Determine the relevant "fit" information that can be used to 
/// discover if and how the specified combination of bones will fit into 
/// this palette.
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::compute_palette_fit(BoneIndexMap & Input, std::int32_t & nCurrentSpace, std::int32_t & common_bones, std::int32_t & additional_bones)
{
	// Reset values
	nCurrentSpace = _maximum_size - (std::int32_t)_bones.size();
	common_bones = 0;
	additional_bones = 0;

	// Early out if possible
	if (_bones.size() == 0)
	{
		additional_bones = (int)Input.size();
		return;

	} // End if no bones stored
	else if (Input.size() == 0)
	{
		return;

	} // End if no bones input

	  // Iterate through newly specified input bones and see how many
	  // indices it has in common with our existing set.
	BoneIndexMap::iterator itBone, itBone2;
	for (itBone = Input.begin(); itBone != Input.end(); ++itBone)
	{
		itBone2 = _bones_lut.find(itBone->first);
		if (itBone2 != _bones_lut.end())
			common_bones++;
		else
			additional_bones++;

	} // Next Bone
}

//-----------------------------------------------------------------------------
//  Name : translateBoneToPalette ()
/// <summary>
/// Translate the specified bone index into its associated position in 
/// the palette. If it does not exist, a value of -1 will be returned.
/// </summary>
//-----------------------------------------------------------------------------
std::uint32_t BonePalette::translate_bone_to_palette(std::uint32_t nBoneIndex) const
{
	auto itBone = _bones_lut.find(nBoneIndex);
	if (itBone == _bones_lut.end())
		return 0xFFFFFFFF;
	return itBone->second;
}

//-----------------------------------------------------------------------------
//  Name : getDataGroup ()
/// <summary>
/// Retrieve the identifier of the data group assigned to the subset of the
/// mesh reserved for this bone palette.
/// </summary>
//-----------------------------------------------------------------------------
std::uint32_t BonePalette::get_data_group() const
{
	return _data_group_id;
}

//-----------------------------------------------------------------------------
//  Name : setDataGroup ()
/// <summary>
/// Set the identifier of the data group assigned to the subset of the mesh 
/// reserved for this bone palette.
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::set_data_group(std::uint32_t nGroup)
{
	_data_group_id = nGroup;
}

//-----------------------------------------------------------------------------
//  Name : getMaximumBlendIndex ()
/// <summary>
/// Retrieve the maximum vertex blend index for this palette (i.e. if 
/// every vertex was only influenced by one bone, this variable would 
/// contain a value of 0).
/// </summary>
//-----------------------------------------------------------------------------
std::int32_t BonePalette::get_maximum_blend_index() const
{
	return _maximum_blend_index;
}

//-----------------------------------------------------------------------------
//  Name : setMaximumBlendIndex ()
/// <summary>
/// Set the maximum vertex blend index for this palette (i.e. if every 
/// vertex was only influenced by one bone, this variable would contain a
/// value of 0).
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::set_maximum_blend_index(int nIndex)
{
	_maximum_blend_index = nIndex;
}

//-----------------------------------------------------------------------------
//  Name : getMaximumSize ()
/// <summary>
/// Retrieve the maximum size of the palette -- the maximum number of bones
/// capable of being referenced.
/// </summary>
//-----------------------------------------------------------------------------
std::uint32_t BonePalette::get_maximum_size() const
{
	return _maximum_size;
}

//-----------------------------------------------------------------------------
//  Name : getInfluencedFaces ()
/// <summary>
/// Retrieve the list of faces assigned to this palette.
/// </summary>
//-----------------------------------------------------------------------------
UInt32Array & BonePalette::get_influenced_faces()
{
	return _faces;
}

//-----------------------------------------------------------------------------
//  Name : clearInfluencedFaces ()
/// <summary>
/// Clear out the temporary face influences array.
/// </summary>
//-----------------------------------------------------------------------------
void BonePalette::clear_influenced_faces()
{
	_faces.clear();
}

//-----------------------------------------------------------------------------
//  Name : getBones ()
/// <summary>
/// Retrieve the indices of the bones referenced by this palette.
/// </summary>
//-----------------------------------------------------------------------------
const UInt32Array & BonePalette::get_bones() const
{
	return _bones;
}


///////////////////////////////////////////////////////////////////////////////
// Global Operator Definitions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : operator < () (AdjacentEdgeKey&, AdjacentEdgeKey&)
/// <summary>
/// Perform less than comparison on the AdjacentEdgeKey structure.
/// </summary>
//-----------------------------------------------------------------------------
bool operator < (const Mesh::AdjacentEdgeKey& Key1, const Mesh::AdjacentEdgeKey& Key2)
{
	float fDifference = 0;

	// Test vertex positions.
	if (math::epsilonNotEqual(Key1.vertex1->x, Key2.vertex1->x, math::epsilon<float>()))
		return (Key2.vertex1->x < Key1.vertex1->x);
	if (math::epsilonNotEqual(Key1.vertex1->y, Key2.vertex1->y, math::epsilon<float>()))
		return (Key2.vertex1->y < Key1.vertex1->y);
	if (math::epsilonNotEqual(Key1.vertex1->z, Key2.vertex1->z, math::epsilon<float>()))
		return (Key2.vertex1->z < Key1.vertex1->z);

	if (math::epsilonNotEqual(Key1.vertex2->x, Key2.vertex2->x, math::epsilon<float>()))
		return (Key2.vertex2->x < Key1.vertex2->x);
	if (math::epsilonNotEqual(Key1.vertex2->y, Key2.vertex2->y, math::epsilon<float>()))
		return (Key2.vertex2->y < Key1.vertex2->y);
	if (math::epsilonNotEqual(Key1.vertex2->z, Key2.vertex2->z, math::epsilon<float>()))
		return (Key2.vertex2->z < Key1.vertex2->z);

	// Exactly equal
	return false;
}

//-----------------------------------------------------------------------------
//  Name : operator < () (MeshSubsetKey&, MeshSubsetKey&)
/// <summary>
/// Perform less than comparison on the MeshSubsetKey structure.
/// </summary>
//-----------------------------------------------------------------------------
bool operator < (const Mesh::MeshSubsetKey& Key1, const Mesh::MeshSubsetKey& Key2)
{
	std::int32_t nDifference = 0;

	nDifference = (std::int32_t)Key1.data_group_id - (std::int32_t)Key2.data_group_id;
	if (nDifference != 0) return (nDifference < 0);

	// Exactly equal
	return false;
}
//-----------------------------------------------------------------------------
//  Name : operator < () (WeldKey&, WeldKey&)
/// <summary>
/// Perform less than comparison on the WeldKey structure.
/// </summary>
//-----------------------------------------------------------------------------
bool operator < (const Mesh::WeldKey& Key1, const Mesh::WeldKey& Key2)
{
	float pos1[4];
	gfx::vertexUnpack(pos1, gfx::Attrib::Position, Key1.format, Key1.vertex);
	math::vec3 v1(pos1[0], pos1[1], pos1[2]);

	float pos2[4];
	gfx::vertexUnpack(pos1, gfx::Attrib::Position, Key2.format, Key2.vertex);
	math::vec3 v2(pos2[0], pos2[1], pos2[2]);
	float tolerance = Key1.tolerance * Key2.tolerance;

	return math::distance2(v1, v2) > tolerance;
}


//-----------------------------------------------------------------------------
//  Name : operator < () (BoneCombinationKey&, BoneCombinationKey&)
/// <summary>
/// Perform less than comparison on the BoneCombinationKey structure.
/// </summary>
//-----------------------------------------------------------------------------
bool operator < (const Mesh::BoneCombinationKey& Key1, const Mesh::BoneCombinationKey& Key2)
{
	int nDifference;
	const Mesh::FaceInfluences * p1 = Key1.influences;
	const Mesh::FaceInfluences * p2 = Key2.influences;


	// The bone count must match.
	nDifference = (int)p1->bones.size() - (int)p2->bones.size();
	if (nDifference != 0) return (nDifference < 0);

	// Compare the bone indices in each list
	BonePalette::BoneIndexMap::const_iterator itBone1 = p1->bones.begin();
	BonePalette::BoneIndexMap::const_iterator itBone2 = p2->bones.begin();
	for (; itBone1 != p1->bones.end() && itBone2 != p2->bones.end(); ++itBone1, ++itBone2)
	{
		nDifference = (int)itBone1->first - (int)itBone2->first;
		if (nDifference != 0) return (nDifference < 0);

	} // Next Bone

	// Exact match (for the purposes of combining influences)
	return false;
}