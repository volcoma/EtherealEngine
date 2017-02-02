#include "mesh.h"
#include "vertex_buffer.h"
#include "index_buffer.h"
#include "core/memory/checked_delete.h"
#include <algorithm>
bool Mesh::is_valid() const
{
	if (groups.empty())
	{
		return false;
	}

	for (auto& group : groups)
	{
		if (!group.vertex_buffer)
			return false;

		if (!group.index_buffer)
			return false;
	}
	return true;
}


namespace experimental
{
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
		mBoundingBox.reset();
		mPrepareStatus = MeshStatus::NotPrepared;
		mFaceCount = 0;
		mVertexCount = 0;
		mHardwareMesh = false;
		mOptimizedMesh = false;
		mDisableFinalSort = false;
		mPrepareData.vertexSource = nullptr;
		mPrepareData.ownsSource = false;
		mPrepareData.triangleCount = 0;
		mPrepareData.vertexCount = 0;
		mPrepareData.computeNormals = false;
		mPrepareData.computeBinormals = false;
		mPrepareData.computeTangents = false;
		mSystemVB = nullptr;
		mSystemIB = nullptr;
		mForceTangentGen = false;
		mForceNormalGen = false;
	}

	Mesh::~Mesh()
	{
		dispose();
	}

	void Mesh::dispose()
	{
		// Iterate through the different subsets in the mesh and clean up
		SubsetArray::iterator itSubset;
		for (itSubset = mMeshSubsets.begin(); itSubset != mMeshSubsets.end(); ++itSubset)
		{
			// Just perform a standard 'disconnect' in the 
			// regular unload case.
			checked_delete(*itSubset);

		} // Next Subset
		mMeshSubsets.clear();
		mSubsetLookup.clear();
		mDataGroups.clear();

		// Clean up preparation data.
		if (mPrepareData.ownsSource == true)
			checked_array_delete(mPrepareData.vertexSource);
		mPrepareData.vertexSource = nullptr;
		mPrepareData.sourceFormat = {};
		mPrepareData.ownsSource = false;
		mPrepareData.vertexData.clear();
		mPrepareData.vertexFlags.clear();
		mPrepareData.vertexRecords.clear();
		mPrepareData.triangleData.clear();

		// Release mesh data memory
		checked_array_delete(mSystemVB);
		checked_array_delete(mSystemIB);

		mTriangleData.clear();

		// Release resources
		mHardwareVB.reset();
		mHardwareIB.reset();

		// Clear variables
		mPrepareData.vertexSource = nullptr;
		mPrepareData.ownsSource = false;
		mPrepareData.sourceFormat = {};
		mPrepareData.triangleCount = 0;
		mPrepareData.vertexCount = 0;
		mPrepareData.computeNormals = false;
		mPrepareData.computeBinormals = false;
		mPrepareData.computeTangents = false;
		mPrepareStatus = MeshStatus::NotPrepared;
		mFaceCount = 0;
		mVertexCount = 0;
		mSystemVB = nullptr;
		mVertexFormat = {};
		mSystemIB = nullptr;
		mForceTangentGen = false;
		mForceNormalGen = false;


		// Reset structures
		mBoundingBox.reset();
	}

	bool Mesh::prepareMesh(const gfx::VertexDecl& pVertexFormat, bool bRollBackPrepare /* = false */)
	{
		// If we are already in the process of preparing, this is a no-op.
		if (mPrepareStatus == MeshStatus::Preparing)
			return false;

		// Should we roll back an earlier call to 'endPrepare' ?
		if (mPrepareStatus == MeshStatus::Prepared && bRollBackPrepare == true)
		{
			// Reset required values.
			mPrepareData.triangleCount = 0;
			mPrepareData.triangleData.clear();
			mPrepareData.vertexCount = 0;
			mPrepareData.vertexData.clear();
			mPrepareData.vertexFlags.clear();
			mPrepareData.vertexRecords.clear();
			mPrepareData.computeNormals = false;
			mPrepareData.computeBinormals = false;
			mPrepareData.computeTangents = false;

			// We can release the prior prepared triangle data early as this information
			// will be reconstructed from the existing mesh subset table.
			mTriangleData.clear();

			// Release prior hardware buffers if they were constructed.
			mHardwareVB.reset();
			mHardwareIB.reset();

			// Set the size of the preparation buffer so that we can add
			// the existing buffer data to it.
			auto nNewStride = pVertexFormat.getStride();
			mPrepareData.vertexData.resize(mVertexCount * nNewStride);
			mPrepareData.vertexCount = mVertexCount;

			// Create enough space in our triangle data array for 
			// the number of faces that existed in the prior final buffer.
			mPrepareData.triangleData.resize(mFaceCount);

			// Copy all of the vertex data back into the preparation 
			// structures, converting if necessary.
			if (pVertexFormat.m_hash == mVertexFormat.m_hash)
				memcpy(&mPrepareData.vertexData[0], mSystemVB, mPrepareData.vertexData.size());
			else
				gfx::vertexConvert(pVertexFormat, &mPrepareData.vertexData[0], mVertexFormat, mSystemVB, mVertexCount);

			// Clear out the vertex buffer
			checked_array_delete(mSystemVB);
			mVertexCount = 0;

			// Iterate through each subset and extract triangle data.
			SubsetArray::iterator itSubset;
			for (itSubset = mMeshSubsets.begin(); itSubset != mMeshSubsets.end(); ++itSubset)
			{
				MeshSubset * pSubset = *itSubset;

				// Iterate through each face in the subset
				std::uint32_t * pCurrentIndex = &mSystemIB[(pSubset->faceStart * 3)];
				for (std::int32_t i = 0; i < pSubset->faceCount; ++i, pCurrentIndex += 3)
				{
					// Generate winding data
					Triangle & Data = mPrepareData.triangleData[mPrepareData.triangleCount++];
					Data.dataGroupId = pSubset->dataGroupId;
					memcpy(Data.indices, pCurrentIndex, 3 * sizeof(std::uint32_t));

				} // Next Face

			} // Next subset

			  // Release additional memory
			checked_array_delete(mSystemIB);
			mFaceCount = 0;

			// Determine which components the original vertex data actually contained.
			bool bSourceHasNormal = mVertexFormat.has(gfx::Attrib::Normal);
			bool bSourceHasBinormal = mVertexFormat.has(gfx::Attrib::Bitangent);
			bool bSourceHasTangent = mVertexFormat.has(gfx::Attrib::Tangent);

			// The 'PreparationData::vertexFlags' array contains a record of the above for each vertex
			// that currently exists in the preparation buffer. This is required when performing processes 
			// such as the generation of vertex normals, etc.
			std::uint8_t nVertexFlags = 0;
			if (bSourceHasNormal)
				nVertexFlags |= PreparationData::SourceContainsNormal;
			if (bSourceHasBinormal)
				nVertexFlags |= PreparationData::SourceContainsBinormal;
			if (bSourceHasTangent)
				nVertexFlags |= PreparationData::SourceContainsTangent;

			// Record the information.
			mPrepareData.vertexFlags.resize(mPrepareData.vertexCount);
			for (std::uint32_t i = 0; i < mPrepareData.vertexCount; ++i)
				mPrepareData.vertexFlags[i] = nVertexFlags;

			// Clean up heap allocated subset structures
			for (itSubset = mMeshSubsets.begin(); itSubset != mMeshSubsets.end(); ++itSubset)
				checked_delete(*itSubset);

			// Reset prepared data arrays and variables.
			mDataGroups.clear();
			mMeshSubsets.clear();
			mSubsetLookup.clear();
			mHardwareMesh = false;
			mOptimizedMesh = false;

		} // End if roll back an earlier prepare
		else if ((mPrepareStatus != MeshStatus::Preparing && bRollBackPrepare) || !bRollBackPrepare)
		{
			// Clear out anything which is currently loaded in the mesh.
			dispose();

		} // End if not rolling back or no need to roll back

		  // We are in the process of preparing the mesh
		mPrepareStatus = MeshStatus::Preparing;
		mVertexFormat = pVertexFormat;

		return true;
	}

	//-----------------------------------------------------------------------------
	//  Name : prepareMesh ()
	/// <summary>
	/// Prepare the mesh immediately with the specified data.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::prepareMesh(const gfx::VertexDecl& pVertexFormat, void * pVertices, std::uint32_t nVertexCount, const TriangleArray & Faces, bool bHardwareCopy /* = true */, bool bWeld /* = true */, bool bOptimize /* = true */)
	{
		// Clear out anything which is currently loaded in the mesh.
		dispose();

		// We are in the process of preparing the mesh

		mPrepareStatus = MeshStatus::Preparing;
		mVertexFormat = pVertexFormat;

		// Populate preparation structures.
		mPrepareData.triangleCount = (std::uint32_t)Faces.size();
		mPrepareData.triangleData = Faces;
		mPrepareData.vertexCount = nVertexCount;


		// Copy vertex data.
		mPrepareData.vertexData.resize(nVertexCount * pVertexFormat.getStride());
		mPrepareData.vertexFlags.resize(nVertexCount);
		memset(&mPrepareData.vertexFlags[0], 0, nVertexCount);
		memcpy(&mPrepareData.vertexData[0], pVertices, nVertexCount * pVertexFormat.getStride());

		// Generate the bounding box data for the new geometry.
		std::int32_t nPositionOffset = pVertexFormat.getOffset(gfx::Attrib::Position);
		std::int32_t nStride = (std::int32_t)pVertexFormat.getStride();
		if (nPositionOffset >= 0)
		{
			std::uint8_t * pSrc = ((std::uint8_t*)pVertices) + nPositionOffset;
			for (std::uint32_t i = 0; i < nVertexCount; ++i, pSrc += nStride)
				mBoundingBox.add_point(*((math::vec3*)pSrc));



		} // End if has position

		  // Finish up
		return endPrepare(bHardwareCopy, bWeld, bOptimize);
	}

	//-----------------------------------------------------------------------------
	//  Name : setVertexSource ()
	/// <summary>
	/// While preparing the mesh, this function should be called in order to
	/// specify the source of the vertex buffer to pull data from.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::setVertexSource(void * pSource, std::uint32_t nVertexCount, const gfx::VertexDecl& pSourceFormat)
	{
		// We can only do this if we are in the process of preparing the mesh
		if (mPrepareStatus != MeshStatus::Preparing)
		{
			//AppLog::write(AppLog::Debug | AppLog::Error, _T("Attempting to set a mesh vertex source without first calling 'prepareMesh' is not allowed.\n"));
			return false;

		} // End if not preparing

		  // Clear any existing source information.
		if (mPrepareData.ownsSource == true)
			checked_array_delete(mPrepareData.vertexSource);
		mPrepareData.vertexSource = nullptr;
		mPrepareData.sourceFormat = {};
		mPrepareData.ownsSource = false;
		mPrepareData.vertexRecords.clear();

		// If specifying nullptr (i.e. to clear) then we're done.
		if (pSource == nullptr)
			return true;

		// Validate requirements
		if (nVertexCount == 0)
			return false;

		// If source format matches the format we're using to prepare
		// then just store the pointer for this vertex source. Otherwise
		// we need to allocate a temporary buffer and convert the data.
		mPrepareData.sourceFormat = pSourceFormat;
		if (pSourceFormat.m_hash == mVertexFormat.m_hash)
		{
			mPrepareData.vertexSource = (std::uint8_t*)pSource;

		} // End if matching
		else
		{
			mPrepareData.vertexSource = new std::uint8_t[nVertexCount * mVertexFormat.getStride()];
			mPrepareData.ownsSource = true;
			gfx::vertexConvert(mVertexFormat, mPrepareData.vertexSource, pSourceFormat, (std::uint8_t*)pSource, nVertexCount);
		} // End if !matching

		  // Allocate the vertex records for the new vertex buffer
		mPrepareData.vertexRecords.clear();
		mPrepareData.vertexRecords.resize(nVertexCount);

		// Fill with 0xFFFFFFFF initially to indicate that no vertex
		// originally in this location has yet been inserted into the
		// final vertex list.
		memset(&mPrepareData.vertexRecords[0], 0xFF, nVertexCount * sizeof(std::uint32_t));

		// Some data needs computing? These variables are essentially 'toggles'
		// that are set largely so that we can early out if it was NEVER necessary 
		// to generate these components (i.e. not one single vertex needed it).
		if (!pSourceFormat.has(gfx::Attrib::Normal) && mVertexFormat.has(gfx::Attrib::Normal))
			mPrepareData.computeNormals = true;
		if (!pSourceFormat.has(gfx::Attrib::Bitangent) && mVertexFormat.has(gfx::Attrib::Bitangent))
			mPrepareData.computeBinormals = true;
		if (!pSourceFormat.has(gfx::Attrib::Tangent) && mVertexFormat.has(gfx::Attrib::Tangent))
			mPrepareData.computeTangents = true;

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
	bool Mesh::addPrimitives(const TriangleArray & aTriangles)
	{
		std::uint32_t nOrigIndex, nIndex;

		// We can only do this if we are in the process of preparing the mesh
		if (mPrepareStatus != MeshStatus::Preparing)
		{
			//AppLog::write(AppLog::Debug | AppLog::Error, _T("Attempting to add primitives to a mesh without first calling 'prepareMesh' is not allowed.\n"));
			return false;

		} // End if not preparing

		  // Determine the correct offset to any relevant elements in the vertex
		std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
		std::uint16_t nNormalOffset = mVertexFormat.getOffset(gfx::Attrib::Normal);
		std::uint16_t nVertexStride = mVertexFormat.getStride();

		// During the construction process we test to see if any specified
		// vertex normal contains invalid data. If the original source vertex
		// data did not contain a normal, we can optimize and skip this step.
		bool bSourceHasNormal = mPrepareData.sourceFormat.has(gfx::Attrib::Normal);
		bool bSourceHasBinormal = mPrepareData.sourceFormat.has(gfx::Attrib::Bitangent);
		bool bSourceHasTangent = mPrepareData.sourceFormat.has(gfx::Attrib::Tangent);

		// In addition, we also record which of the required components each
		// vertex actually contained based on the following information.
		std::uint8_t nVertexFlags = 0;
		if (bSourceHasNormal == true)
			nVertexFlags |= PreparationData::SourceContainsNormal;
		if (bSourceHasBinormal == true)
			nVertexFlags |= PreparationData::SourceContainsBinormal;
		if (bSourceHasTangent == true)
			nVertexFlags |= PreparationData::SourceContainsTangent;

		// Loop through the specified faces and process them.
		std::uint8_t * pSrcVertices = mPrepareData.vertexSource;
		std::uint32_t nFaceCount = (std::uint32_t)aTriangles.size();
		for (std::uint32_t i = 0; i < nFaceCount; ++i)
		{
			const Triangle & SrcTri = aTriangles[i];

			// Retrieve vertex positions (if there are any) so that we can perform degenerate testing.
			if (nPositionOffset >= 0)
			{
				math::vec3 * pVertex1 = (math::vec3*)(pSrcVertices + (SrcTri.indices[0] * nVertexStride) + nPositionOffset);
				math::vec3 * pVertex2 = (math::vec3*)(pSrcVertices + (SrcTri.indices[1] * nVertexStride) + nPositionOffset);
				math::vec3 * pVertex3 = (math::vec3*)(pSrcVertices + (SrcTri.indices[2] * nVertexStride) + nPositionOffset);

				// Skip triangle if it is degenerate.
				if ((math::epsilonEqual(*pVertex1, *pVertex2, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}) ||
					(math::epsilonEqual(*pVertex1, *pVertex3, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}) ||
					(math::epsilonEqual(*pVertex2, *pVertex3, math::epsilon<float>()) == math::tvec3<bool>{true, true, true}))
					continue;
			} // End if has position.

			  // Prepare a triangle structure ready for population
			mPrepareData.triangleCount++;
			mPrepareData.triangleData.resize(mPrepareData.triangleCount);
			Triangle & TriangleData = mPrepareData.triangleData[mPrepareData.triangleCount - 1];

			// Set triangle's subset information.
			TriangleData.dataGroupId = SrcTri.dataGroupId;

			// For each index in the face
			for (std::uint32_t j = 0; j < 3; ++j)
			{
				// Extract the original index from the specified index buffer
				nOrigIndex = SrcTri.indices[j];

				// Retrieve the vertex record for the original vertex
				nIndex = mPrepareData.vertexRecords[nOrigIndex];

				// Have we inserted this vertex into the vertex buffer previously?
				if (nIndex == 0xFFFFFFFF)
				{
					// Vertex does not yet exist in the vertex buffer we are preparing
					// so copy the vertex in and record the index mapping for this vertex.
					nIndex = mPrepareData.vertexCount++;
					mPrepareData.vertexRecords[nOrigIndex] = nIndex;

					// Resize the output vertex buffer ready to hold this new data.
					size_t nInitialSize = mPrepareData.vertexData.size();
					mPrepareData.vertexData.resize(nInitialSize + nVertexStride);

					// Copy the data in.
					std::uint8_t * pSrc = pSrcVertices + (nOrigIndex * nVertexStride);
					std::uint8_t * pDst = &mPrepareData.vertexData[nInitialSize];
					memcpy(pDst, pSrc, nVertexStride);

					// Also record other pertenant details about this vertex.
					mPrepareData.vertexFlags.push_back(nVertexFlags);

					// Clear any invalid normals (completely messes up HDR if ANY NaNs make it this far)
					if (nNormalOffset > 0 && bSourceHasNormal == true)
					{
						math::vec3 * pNormal = (math::vec3*)(pDst + nNormalOffset);
						if (_isnan(pNormal->x) || _isnan(pNormal->y) || _isnan(pNormal->z))
							*pNormal = math::vec3(0, 0, 0);

					} // End if have normal

					  // Grow the size of the bounding box
					if (nPositionOffset >= 0)
						mBoundingBox.add_point(*((math::vec3*)(pDst + nPositionOffset)));

				} // End if vertex not recorded in this buffer yet

				  // Copy the index in
				TriangleData.indices[j] = nIndex;

			} // Next Index

		} // Next Face

		  // Success!
		return true;
	}

	//-----------------------------------------------------------------------------
	//  Name : createBox ()
	/// <summary>
	/// Create box geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::createBox(const gfx::VertexDecl& pFormat, float fWidth, float fHeight, float fDepth, std::uint32_t nWidthSegs, std::uint32_t nHeightSegs, std::uint32_t nDepthSegs, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
	{
		return createBox(pFormat, fWidth, fHeight, fDepth, nWidthSegs, nHeightSegs, nDepthSegs, 1.0f, 1.0f, bInverted, Origin, bHardwareCopy);
	}

	//-----------------------------------------------------------------------------
	//  Name : createBox ()
	/// <summary>
	/// Create box geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::createBox(const gfx::VertexDecl& pFormat, float fWidth, float fHeight, float fDepth, std::uint32_t nWidthSegs, std::uint32_t nHeightSegs, std::uint32_t nDepthSegs, float fTexUScale, float fTexVScale, bool bInverted, MeshCreateOrigin::E Origin, bool bHardwareCopy /* = true */)
	{
		std::uint32_t  nXCount, nYCount, nCounter;
		math::vec3 vCurrentPos, vDeltaPosX, vDeltaPosY, vNormal;
		math::vec2 vCurrentTex, vDeltaTex;

		// Clear out old data.
		dispose();

		// We are in the process of preparing.
		mPrepareStatus = MeshStatus::Preparing;
		mVertexFormat = pFormat;

		// Determine the correct offset to any relevant elements in the vertex
		std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
		std::uint16_t nTexCoordOffset = mVertexFormat.getOffset(gfx::Attrib::TexCoord0);
		std::uint16_t nNormalOffset = mVertexFormat.getOffset(gfx::Attrib::Normal);
		std::uint16_t nVertexStride = mVertexFormat.getStride();

		// Compute the number of faces and vertices that will be required for this box
		mPrepareData.triangleCount = (4 * (nWidthSegs * nDepthSegs)) + (4 * (nWidthSegs * nHeightSegs)) + (4 * (nHeightSegs * nDepthSegs));
		mPrepareData.vertexCount = (2 * ((nWidthSegs + 1) * (nDepthSegs + 1))) + (2 * ((nWidthSegs + 1) * (nHeightSegs + 1))) + (2 * ((nHeightSegs + 1) * (nDepthSegs + 1)));

		// Allocate enough space for the new vertex and triangle data
		mPrepareData.vertexData.resize(mPrepareData.vertexCount * nVertexStride);
		mPrepareData.vertexFlags.resize(mPrepareData.vertexCount);
		mPrepareData.triangleData.resize(mPrepareData.triangleCount);

		// Ensure width and depth are absolute (prevent inverting on those axes)
		fWidth = fabsf(fWidth);
		fDepth = fabsf(fDepth);

		// Generate faces
		std::uint8_t * pCurrentVertex = &mPrepareData.vertexData[0];
		std::uint8_t * pCurrentFlags = &mPrepareData.vertexFlags[0];
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
				vCurrentPos = math::vec3(fHalfWidth, fHeight, -fHalfDepth);
				vNormal = math::vec3(1, 0, 0);
				break;

			case 1: // +Y
				nXCount = nWidthSegs + 1;
				nYCount = nDepthSegs + 1;
				vDeltaPosX = math::vec3(fWidth / (float)nWidthSegs, 0, 0);
				vDeltaPosY = math::vec3(0, 0, -fDepth / (float)nDepthSegs);
				vCurrentPos = math::vec3(-fHalfWidth, fHeight, fHalfDepth);
				vNormal = (fHeight > 0.0f) ? math::vec3(0, 1, 0) : math::vec3(0, -1, 0);
				break;

			case 2: // +Z
				nXCount = nWidthSegs + 1;
				nYCount = nHeightSegs + 1;
				vDeltaPosX = math::vec3(-fWidth / (float)nWidthSegs, 0, 0);
				vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
				vCurrentPos = math::vec3(fHalfWidth, fHeight, fHalfDepth);
				vNormal = math::vec3(0, 0, 1);
				break;

			case 3: // -X
				nXCount = nDepthSegs + 1;
				nYCount = nHeightSegs + 1;
				vDeltaPosX = math::vec3(0, 0, -fDepth / (float)nDepthSegs);
				vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
				vCurrentPos = math::vec3(-fHalfWidth, fHeight, fHalfDepth);
				vNormal = math::vec3(-1, 0, 0);
				break;

			case 4: // -Y
				nXCount = nWidthSegs + 1;
				nYCount = nDepthSegs + 1;
				vDeltaPosX = math::vec3(fWidth / (float)nWidthSegs, 0, 0);
				vDeltaPosY = math::vec3(0, 0, fDepth / (float)nDepthSegs);
				vCurrentPos = math::vec3(-fHalfWidth, 0, -fHalfDepth);
				vNormal = (fHeight > 0.0f) ? math::vec3(0, -1, 0) : math::vec3(0, 1, 0);
				break;

			case 5: // -Z
				nXCount = nWidthSegs + 1;
				nYCount = nHeightSegs + 1;
				vDeltaPosX = math::vec3(fWidth / (float)nWidthSegs, 0, 0);
				vDeltaPosY = math::vec3(0, -fHeight / (float)nHeightSegs, 0);
				vCurrentPos = math::vec3(-fHalfWidth, fHeight, -fHalfDepth);
				vNormal = math::vec3(0, 0, -1);
				break;

			} // End Face Switch

			  // Should we invert the vertex normal
			if (bInverted == true)
				vNormal = -vNormal;

			// Add faces
			vCurrentTex = math::vec2(0, 0);
			vDeltaTex = math::vec2(1.0f / (float)(nXCount - 1), 1.0f / (float)(nYCount - 1));
			for (std::uint32_t y = 0; y < nYCount; ++y)
			{
				for (std::uint32_t x = 0; x < nXCount; ++x)
				{
					math::vec3 vOutputPos = vCurrentPos;
					if (Origin == MeshCreateOrigin::Center)
						vOutputPos.y -= fHeight * 0.5f;
					else if (Origin == MeshCreateOrigin::Top)
						vOutputPos.y -= fHeight;

					// Store vertex components
					if (nPositionOffset >= 0)
						gfx::vertexPack(&vOutputPos[0], false, gfx::Attrib::Position, pFormat, pCurrentVertex);
					//*((math::vec3*)(pCurrentVertex + nPositionOffset)) = vOutputPos;
					if (nNormalOffset > 0)
						gfx::vertexPack(&vNormal[0], true, gfx::Attrib::Normal, pFormat, pCurrentVertex);
					//*((math::vec3*)(pCurrentVertex + nNormalOffset)) = vNormal;
					if (nTexCoordOffset > 0)
						gfx::vertexPack(&math::vec2(vCurrentTex.x * fTexUScale, vCurrentTex.y * fTexVScale)[0], true, gfx::Attrib::TexCoord0, pFormat, pCurrentVertex);
					//*((math::vec2*)(pCurrentVertex + nTexCoordOffset)) = math::vec2(vCurrentTex.x * fTexUScale, vCurrentTex.y * fTexVScale);

				// Set flags for this vertex (we want to generate tangents 
				// and binormals if we need them).
					*pCurrentFlags++ = PreparationData::SourceContainsNormal;

					// Grow the object space bounding box for this mesh
					// by including the computed position.
					mBoundingBox.add_point(vOutputPos);

					// Move to next vertex position
					vCurrentPos += vDeltaPosX;
					vCurrentTex.x += vDeltaTex.x;
					pCurrentVertex += nVertexStride;

				} // Next Column

				  // Move to next row
				vCurrentPos += vDeltaPosY;
				vCurrentPos -= vDeltaPosX * (float)nXCount;
				vCurrentTex.x = 0.0f;
				vCurrentTex.y += vDeltaTex.y;

			} // Next Row

		} // Next Face


		  // Now generate indices. For each box face.
		nCounter = 0;
		Triangle * pCurrentTriangle = &mPrepareData.triangleData[0];
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
						pCurrentTriangle->dataGroupId = 0;
						pCurrentTriangle->indices[0] = x + 1 + ((y + 1) * nXCount) + nCounter;
						pCurrentTriangle->indices[1] = x + 1 + (y * nXCount) + nCounter;
						pCurrentTriangle->indices[2] = x + (y * nXCount) + nCounter;
						pCurrentTriangle++;

						pCurrentTriangle->dataGroupId = 0;
						pCurrentTriangle->indices[0] = x + ((y + 1) * nXCount) + nCounter;
						pCurrentTriangle->indices[1] = x + 1 + ((y + 1) * nXCount) + nCounter;
						pCurrentTriangle->indices[2] = x + (y * nXCount) + nCounter;
						pCurrentTriangle++;

					} // End if inverted
					else
					{
						pCurrentTriangle->dataGroupId = 0;
						pCurrentTriangle->indices[0] = x + (y * nXCount) + nCounter;
						pCurrentTriangle->indices[1] = x + 1 + (y * nXCount) + nCounter;
						pCurrentTriangle->indices[2] = x + 1 + ((y + 1) * nXCount) + nCounter;
						pCurrentTriangle++;

						pCurrentTriangle->dataGroupId = 0;
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
		mPrepareData.computeNormals = true;
		mPrepareData.computeBinormals = mVertexFormat.has(gfx::Attrib::Bitangent);
		mPrepareData.computeTangents = mVertexFormat.has(gfx::Attrib::Tangent);

		// Finish up
		return endPrepare(bHardwareCopy, false, false);
	}

	//-----------------------------------------------------------------------------
	//  Name : subsetSortPredicate () (Private, Static)
	/// <summary>
	/// Predicate function that allows us to sort mesh subset lists by
	/// material and datagroup.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::subsetSortPredicate(const MeshSubset * lhs, const MeshSubset * rhs)
	{
		if ((lhs->dataGroupId < rhs->dataGroupId))
			return true;
		return false;
	}

	//-----------------------------------------------------------------------------
	//  Name : endPrepare ()
	/// <summary>
	/// All data has been added to the mesh and we should now build the
	/// renderable data for the mesh.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::endPrepare(bool bHardwareCopy /* = true */, bool bWeld /* = true */, bool bOptimize /* = true */)
	{
		// Were we previously preparing?
		if (mPrepareStatus != MeshStatus::Preparing)
		{
			//AppLog::write(AppLog::Debug | AppLog::Error, _T("Attempting to call 'endPrepare' on a mesh without first calling 'prepareMesh' is not allowed.\n"));
			return false;

		} // End if previously preparing

		  // Clear out old data that is no longer necessary in order to preserve memory
		gfx::setVertexBuffer(gfx::VertexBufferHandle{ gfx::invalidHandle });
		// Scan the preparation data for degenerate triangles.
		std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
		std::uint16_t nVertexStride = mVertexFormat.getStride();
		std::uint8_t * pSrcVertices = &mPrepareData.vertexData[0] + nPositionOffset;
		for (std::uint32_t i = 0; i < mPrepareData.triangleCount; ++i)
		{
			Triangle & Tri = mPrepareData.triangleData[i];
			math::vec3 v1;
			float vf1[4];
			gfx::vertexUnpack(vf1, gfx::Attrib::Position, mVertexFormat, pSrcVertices, Tri.indices[0]);
			math::vec3 v2;
			float vf2[4];
			gfx::vertexUnpack(vf2, gfx::Attrib::Position, mVertexFormat, pSrcVertices, Tri.indices[1]);
			math::vec3 v3;
			float vf3[4];
			gfx::vertexUnpack(vf3, gfx::Attrib::Position, mVertexFormat, pSrcVertices, Tri.indices[2]);
			memcpy(&v1[0], vf1, 3 * sizeof(float));
			memcpy(&v2[0], vf2, 3 * sizeof(float));
			memcpy(&v3[0], vf3, 3 * sizeof(float));
			//const math::vec3 & v1 = *(math::vec3*)(pSrcVertices + (Tri.indices[0] * nVertexStride));
			//const math::vec3 & v2 = *(math::vec3*)(pSrcVertices + (Tri.indices[1] * nVertexStride));
			//const math::vec3 & v3 = *(math::vec3*)(pSrcVertices + (Tri.indices[2] * nVertexStride));	

			math::vec3 c = math::cross(v2 - v1, v3 - v1);
			if (math::length2(c) < (4.0f * 0.000001f * 0.000001f))
				Tri.flags |= TriangleFlags::Degenerate;

		} // Next triangle

		  // Process the vertex data in order to generate any additional components that may be necessary
		  // (i.e. Normal, Binormal and Tangent)
		if (generateVertexComponents(bWeld) == false)
			return false;

		// Allocate the system memory vertex buffer ready for population.
		mVertexCount = (std::uint32_t)mPrepareData.vertexCount;
		mSystemVB = new std::uint8_t[mVertexCount * mVertexFormat.getStride()];

		// Copy vertex data into the new buffer and dispose of the temporary data.
		memcpy(mSystemVB, &mPrepareData.vertexData[0], mVertexCount * mVertexFormat.getStride());
		mPrepareData.vertexData.clear();
		mPrepareData.vertexFlags.clear();
		mPrepareData.vertexCount = 0;

		// Vertex data has been updated and potentially needs to be serialized.


		// Allocate the memory for our system memory index buffer
		mFaceCount = mPrepareData.triangleCount;
		mSystemIB = new std::uint32_t[mFaceCount * 3];

		// Transform triangle indices, material and data group information
		// to the final triangle data arrays. We keep the latter two handy so
		// that we know precisely which subset each triangle belongs to.
		mTriangleData.resize(mFaceCount);
		std::uint32_t * pDstIndices = mSystemIB;
		for (std::uint32_t i = 0; i < mFaceCount; ++i)
		{
			// Copy indices.
			const Triangle & TriIn = mPrepareData.triangleData[i];
			*pDstIndices++ = TriIn.indices[0];
			*pDstIndices++ = TriIn.indices[1];
			*pDstIndices++ = TriIn.indices[2];

			// Copy triangle subset information.
			MeshSubsetKey & TriOut = mTriangleData[i];
			TriOut.dataGroupId = TriIn.dataGroupId;

		} // Next triangle
		mPrepareData.triangleCount = 0;
		mPrepareData.triangleData.clear();

		// Index data has been updated and potentially needs to be serialized.


		// A video memory copy of the mesh was requested?
		if (bHardwareCopy)
		{
			// Calculate the required size of the vertex buffer
			std::uint32_t nBufferSize = mVertexCount * mVertexFormat.getStride();

			const gfx::Memory* mem = gfx::copy(mSystemVB, static_cast<std::uint32_t>(nBufferSize));
			mHardwareVB = std::make_shared<VertexBuffer>();
			mHardwareVB->populate(mem, mVertexFormat);

		} // End if video memory vertex buffer required

		  // Skin binding data has potentially been updated and needs to be serialized.


		  // Finally perform the final sort of the mesh data in order
		  // to build the index buffer and subset tables.
		if (!sortMeshData(bOptimize, bHardwareCopy))
			return false;

		// The mesh is now prepared
		mPrepareStatus = MeshStatus::Prepared;
		mHardwareMesh = bHardwareCopy;
		mOptimizedMesh = bOptimize;

		// Success!
		return true;
	}

	//-----------------------------------------------------------------------------
	// Name : sortMeshData() (Protected)
	/// <summary>
	/// Sort the data in the mesh into material & datagroup order.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::sortMeshData(bool bOptimize, bool bBuildHardwareBuffers)
	{
		std::map<MeshSubsetKey, std::uint32_t> SubsetSizes;
		std::map<MeshSubsetKey, std::uint32_t>::iterator itSubsetSize;
		DataGroupSubsetMap::iterator itDataGroup;
		std::uint32_t i, j;

		// Clear out any old data EXCEPT the old subset index
		// We'll need this in order to understand how to update
		// the material reference counting later on.
		mDataGroups.clear();
		mSubsetLookup.clear();

		// Our first job is to collate all the various subsets and also
		// to determine how many triangles should exist in each.
		for (i = 0; i < mFaceCount; ++i)
		{
			const MeshSubsetKey & SubsetKey = mTriangleData[i];

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
			MeshSubset * pSubset = new MeshSubset();
			pSubset->dataGroupId = Key.dataGroupId;
			pSubset->faceStart = nCounter;
			nCounter += itSubsetSize->second;

			// Ensure that "FaceCount" defaults to zero at this point
			// so that we can keep a running total during the final buffer 
			// construction.
			pSubset->faceCount = 0;

			// Also reset vertex values as appropriate (will grow
			// using standard 'bounding' value insert).
			pSubset->vertexStart = 0x7FFFFFFF;
			pSubset->vertexCount = 0;

			// Add to list for fast linear access, and lookup table
			// for sorted search.
			NewSubsets.push_back(pSubset);
			mSubsetLookup[Key] = pSubset;

			// Add to data group lookup table
			itDataGroup = mDataGroups.find(pSubset->dataGroupId);
			if (itDataGroup == mDataGroups.end())
				mDataGroups[pSubset->dataGroupId].push_back(pSubset);
			else
				itDataGroup->second.push_back(pSubset);


		} // Next Subset

		  // Allocate space for new sorted index buffer and face re-map information
		std::uint32_t * pSrcIndices = mSystemIB;
		std::uint32_t * pDstIndices = new std::uint32_t[mFaceCount * 3];
		std::uint32_t * pFaceRemap = new std::uint32_t[mFaceCount];

		// Start building new indices
		std::int32_t  nIndex;
		std::uint32_t nIndexStart = 0;
		for (i = 0; i < mFaceCount; ++i)
		{
			// Find a matching subset for this triangle
			MeshSubset * pSubset = mSubsetLookup[mTriangleData[i]];

			// Copy index data over to new buffer, taking care to record the correct
			// vertex values as required. We'll temporarily use VertexStart and VertexCount
			// as a MathUtility::minValue/max record that we'll come round and correct later.
			nIndexStart = (pSubset->faceStart + pSubset->faceCount) * 3;

			// Index[0]
			nIndex = (std::int32_t)(*pSrcIndices++);
			if (nIndex < pSubset->vertexStart)
				pSubset->vertexStart = nIndex;
			if (nIndex > pSubset->vertexCount)
				pSubset->vertexCount = nIndex;
			pDstIndices[nIndexStart++] = nIndex;

			// Index[1]
			nIndex = (std::int32_t)(*pSrcIndices++);
			if (nIndex < pSubset->vertexStart)
				pSubset->vertexStart = nIndex;
			if (nIndex > pSubset->vertexCount)
				pSubset->vertexCount = nIndex;
			pDstIndices[nIndexStart++] = nIndex;

			// Index[2]
			nIndex = (std::int32_t)(*pSrcIndices++);
			if (nIndex < pSubset->vertexStart)
				pSubset->vertexStart = nIndex;
			if (nIndex > pSubset->vertexCount)
				pSubset->vertexCount = nIndex;
			pDstIndices[nIndexStart++] = nIndex;

			// Store face re-map information so that we can remap data as required
			pFaceRemap[i] = pSubset->faceStart + pSubset->faceCount;

			// We have now recorded a triangle in this subset
			pSubset->faceCount++;

		} // Next Triangle

		  // Sort the subset list in order to ensure that all subsets with the same 
		  // materials and data groups are added next to one another in the final
		  // index buffer. This ensures that we can batch draw all subsets that share 
		  // common properties.
		std::sort(NewSubsets.begin(), NewSubsets.end(), subsetSortPredicate);

		// Perform the same sort on the data group and material mapped lists.
		// Also take the time to build the final list of materials used by this mesh
		// (render control batching system requires that we cache this information in a 
		// specific format).
		for (itDataGroup = mDataGroups.begin(); itDataGroup != mDataGroups.end(); ++itDataGroup)
			std::sort(itDataGroup->second.begin(), itDataGroup->second.end(), subsetSortPredicate);

		// Optimize the faces as we transfer to the final destination index buffer
		// if requested. Otherwise, just copy them over directly.
		pSrcIndices = pDstIndices;
		pDstIndices = mSystemIB;

		for (nCounter = 0, i = 0; i < (size_t)NewSubsets.size(); ++i)
		{
			MeshSubset * pSubset = NewSubsets[i];

			// Note: Remember that at this stage, the subset's 'vertexCount' member still describes
			// a 'max' vertex (not a count)... We're correcting this later.
			if (bOptimize == true)
				buildOptimizedIndexBuffer(pSubset, pSrcIndices + (pSubset->faceStart * 3), pDstIndices, pSubset->vertexStart, pSubset->vertexCount);
			else
				memcpy(pDstIndices, pSrcIndices + (pSubset->faceStart * 3), pSubset->faceCount * 3 * sizeof(std::uint32_t));

			// This subset's starting face now refers to its location 
			// in the final destination buffer rather than the temporary one.
			pSubset->faceStart = nCounter;
			nCounter += pSubset->faceCount;

			// Move on to output next sorted subset.
			pDstIndices += pSubset->faceCount * 3;

		} // Next Subset

		  // Clean up.
		checked_array_delete(pSrcIndices);

		// Rebuild the additional triangle data based on the newly sorted
		// subset data, and also convert the previously recorded maximum
		// vertex value (stored in "VertexCount") into its final form
		for (i = 0; i < (std::uint32_t)NewSubsets.size(); ++i)
		{
			// Convert vertex "Max" to "Count"
			MeshSubset * pSubset = NewSubsets[i];
			pSubset->vertexCount = (pSubset->vertexCount - pSubset->vertexStart) + 1;

			// Update additional triangle data array.
			for (j = pSubset->faceStart; j < ((std::uint32_t)pSubset->faceStart + (std::uint32_t)pSubset->faceCount); ++j)
			{
				mTriangleData[j].dataGroupId = pSubset->dataGroupId;

			} // Next Triangle

		} // Next Subset

		  // We're done with the remap data.
		  // TODO: Note - we don't actually use the face remap information at
		  // the moment, but it could be useful?
		checked_array_delete(pFaceRemap);

		// Hardware versions of the final buffer were required?
		if (bBuildHardwareBuffers)
		{
			// Calculate the required size of the index buffer
			std::uint32_t nBufferSize = mFaceCount * 3 * sizeof(std::uint32_t);

			// Allocate hardware buffer if required (i.e. it does not already exist).
			if (!mHardwareIB || (mHardwareIB && !mHardwareIB->is_valid()))
			{
				const gfx::Memory* mem = gfx::copy(mSystemIB, static_cast<std::uint32_t>(nBufferSize));
				mHardwareIB = std::make_shared<IndexBuffer>();
				mHardwareIB->populate(mem, BGFX_BUFFER_INDEX32);
			} // End if not allocated

		} // End if hardware buffer required

		  // Index data and subsets have been updated and potentially need to be serialized.

		  // Destroy old subset data.
		for (i = 0; i < mMeshSubsets.size(); ++i)
			checked_delete(mMeshSubsets[i]);
		mMeshSubsets.clear();

		// Use the new subset data.
		mMeshSubsets = NewSubsets;

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
	float Mesh::findVertexOptimizerScore(const OptimizerVertexInfo * pVertexInfo)
	{
		float Score = 0.0f;

		// Do any remaining triangles use this vertex?
		if (pVertexInfo->unusedTriangleReferences == 0)
			return -1.0f;

		std::int32_t CachePosition = pVertexInfo->cachePosition;
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
		float ValenceBoost = math::pow((float)pVertexInfo->unusedTriangleReferences, -MeshOptimizer::ValenceBoostPower);
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
	void Mesh::buildOptimizedIndexBuffer(const MeshSubset * pSubset, std::uint32_t * pSrcBuffer, std::uint32_t * pDestBuffer, std::uint32_t nMinVertex, std::uint32_t nMaxVertex)
	{
		OptimizerVertexInfo   * pVertexInfo = nullptr, *pVert;
		OptimizerTriangleInfo * pTriangleInfo = nullptr, *pTri;
		std::uint32_t                i, j, k, nVertexCount;
		float                 fBestScore = 0.0f, fScore;
		std::int32_t                 nBestTriangle = -1;
		std::uint32_t                nVertexCacheSize = 0;
		std::uint32_t                nIndex, nTriangleIndex, nTemp;

		// Declare vertex cache storage (plus one to allow them to drop "off the end")
		std::uint32_t pVertexCache[MeshOptimizer::MaxVertexCacheSize + 1];

		// First allocate enough room for the optimization information for each vertex and triangle
		nVertexCount = (nMaxVertex - nMinVertex) + 1;
		pVertexInfo = new OptimizerVertexInfo[nVertexCount];
		pTriangleInfo = new OptimizerTriangleInfo[pSubset->faceCount];

		// The first pass is to initialize the vertex information with information about the
		// faces which reference them.
		for (i = 0; i < (unsigned)pSubset->faceCount; ++i)
		{
			nIndex = pSrcBuffer[i * 3] - nMinVertex;
			pVertexInfo[nIndex].unusedTriangleReferences++;
			pVertexInfo[nIndex].triangleReferences.push_back(i);
			nIndex = pSrcBuffer[(i * 3) + 1] - nMinVertex;
			pVertexInfo[nIndex].unusedTriangleReferences++;
			pVertexInfo[nIndex].triangleReferences.push_back(i);
			nIndex = pSrcBuffer[(i * 3) + 2] - nMinVertex;
			pVertexInfo[nIndex].unusedTriangleReferences++;
			pVertexInfo[nIndex].triangleReferences.push_back(i);

		} // Next Triangle

		  // Initialize vertex scores
		for (i = 0; i < nVertexCount; ++i)
			pVertexInfo[i].vertexScore = findVertexOptimizerScore(&pVertexInfo[i]);

		// Compute the score for each triangle, and record the triangle with the best score
		for (i = 0; i < (unsigned)pSubset->faceCount; ++i)
		{
			// The triangle score is the sum of the scores of each of
			// its three vertices.
			nIndex = pSrcBuffer[i * 3] - nMinVertex;
			fScore = pVertexInfo[nIndex].vertexScore;
			nIndex = pSrcBuffer[(i * 3) + 1] - nMinVertex;
			fScore += pVertexInfo[nIndex].vertexScore;
			nIndex = pSrcBuffer[(i * 3) + 2] - nMinVertex;
			fScore += pVertexInfo[nIndex].vertexScore;
			pTriangleInfo[i].triangleScore = fScore;

			// Record the triangle with the highest score
			if (fScore > fBestScore)
			{
				fBestScore = fScore;
				nBestTriangle = (signed)i;

			} // End if better than previous score

		} // Next Triangle

		  // Now we can start adding triangles, beginning with the previous highest scoring triangle.
		for (i = 0; i < (unsigned)pSubset->faceCount; ++i)
		{
			// If we don't know the best triangle, for whatever reason, find it
			if (nBestTriangle < 0)
			{
				nBestTriangle = -1;
				fBestScore = 0.0f;

				// Iterate through the entire list of un-added faces
				for (j = 0; j < (unsigned)pSubset->faceCount; ++j)
				{
					if (pTriangleInfo[j].added == false)
					{
						fScore = pTriangleInfo[j].triangleScore;

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
				nIndex = pSrcBuffer[(nTriangleIndex * 3) + j];
				*pDestBuffer++ = nIndex;

				// Adjust the index so that it points into our info buffer
				// rather than the actual source vertex itself.
				nIndex = nIndex - nMinVertex;

				// Retrieve the referenced vertex information
				pVert = &pVertexInfo[nIndex];

				// Reduce the 'valence' of this vertex (one less triangle is now referencing)
				pVert->unusedTriangleReferences--;

				// Remove this triangle from the list of references in the vertex
				UInt32Array::iterator itReference = std::find(pVert->triangleReferences.begin(), pVert->triangleReferences.end(), nTriangleIndex);
				if (itReference != pVert->triangleReferences.end())
					pVert->triangleReferences.erase(itReference);

				// Now we must update the vertex cache to include this vertex. If it was
				// already in the cache, it should be moved to the head, otherwise it should
				// be inserted (pushing one off the end).
				if (pVert->cachePosition == -1)
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
						pVertexInfo[pVertexCache[nVertexCacheSize]].cachePosition = -1;

					} // End if no more room

					  // Overwrite the first entry
					pVertexCache[0] = nIndex;

				} // End if not in cache
				else if (pVert->cachePosition > 0)
				{
					// Already in the vertex cache, move it to the head.
					// Note : If the cache position is already 0, we just ignore
					// it... hence the above 'else if' rather than just 'else'.
					if (pVert->cachePosition == 1)
					{
						// We were in the second slot, just swap the two
						nTemp = pVertexCache[0];
						pVertexCache[0] = nIndex;
						pVertexCache[1] = nTemp;

					} // End if simple swap
					else
					{
						// Shuffle EVERYONE up who came before us.
						memmove(&pVertexCache[1], &pVertexCache[0], pVert->cachePosition * sizeof(std::uint32_t));

						// Insert this vertex at the head
						pVertexCache[0] = nIndex;

					} // End if memory move required

				} // End if already in cache

				  // Update the cache position records for all vertices in the cache
				for (k = 0; k < nVertexCacheSize; ++k)
					pVertexInfo[pVertexCache[k]].cachePosition = k;

			} // Next Index

			  // Recalculate the of all vertices contained in the cache
			for (j = 0; j < nVertexCacheSize; ++j)
			{
				pVert = &pVertexInfo[pVertexCache[j]];
				pVert->vertexScore = findVertexOptimizerScore(pVert);

			} // Next entry in the vertex cache

			  // Update the score of the triangles which reference this vertex
			  // and record the highest scoring.
			for (j = 0; j < nVertexCacheSize; ++j)
			{
				pVert = &pVertexInfo[pVertexCache[j]];

				// For each triangle referenced
				for (k = 0; k < pVert->unusedTriangleReferences; ++k)
				{
					nTriangleIndex = pVert->triangleReferences[k];
					pTri = &pTriangleInfo[nTriangleIndex];
					fScore = pVertexInfo[pSrcBuffer[(nTriangleIndex * 3)] - nMinVertex].vertexScore;
					fScore += pVertexInfo[pSrcBuffer[(nTriangleIndex * 3) + 1] - nMinVertex].vertexScore;
					fScore += pVertexInfo[pSrcBuffer[(nTriangleIndex * 3) + 2] - nMinVertex].vertexScore;
					pTri->triangleScore = fScore;

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
		std::int32_t nFaceStart = 0, nFaceCount = 0, nVertexStart = 0, nVertexCount = 0;

		for (size_t i = 0; i < mMeshSubsets.size(); ++i)
		{
			drawSubset(std::uint32_t(i));
		}

	}


	//-----------------------------------------------------------------------------
	//  Name : drawSubset ()
	/// <summary>
	/// Draw an individual subset of the mesh based on the material AND
	/// data group specified.
	/// </summary>
	//-----------------------------------------------------------------------------
	void Mesh::drawSubset(std::uint32_t nDataGroupId)
	{
		// Attempt to find a matching subset.
		SubsetKeyMap::iterator itSubset = mSubsetLookup.find(MeshSubsetKey(nDataGroupId));
		if (itSubset == mSubsetLookup.end())
			return;

		// Process and draw all subsets of the mesh that use the specified material.
		std::int32_t nSubsetVertStart, nSubsetVertEnd;
		std::int32_t nFaceStart = 0, nFaceCount = 0, nVertexStart = 0, nVertexEnd = 0, nVertexCount = 0;

		MeshSubset * pSubset = itSubset->second;
		nFaceStart = pSubset->faceStart;
		nFaceCount = pSubset->faceCount;
		nVertexStart = pSubset->vertexStart;
		nVertexEnd = nVertexStart + pSubset->vertexCount - 1;

		// Vertex start/end is a little more complex, but can be computed
		// using a containment style test. First precompute some values to
		// make the tests a little simpler.
		nSubsetVertStart = pSubset->vertexStart;
		nSubsetVertEnd = nSubsetVertStart + pSubset->vertexCount - 1;

		// Perform the containment tests
		if (nSubsetVertStart < nVertexStart) nVertexStart = nSubsetVertStart;
		if (nSubsetVertStart > nVertexEnd) nVertexEnd = nSubsetVertStart;
		if (nSubsetVertEnd < nVertexStart) nVertexStart = nSubsetVertEnd;
		if (nSubsetVertEnd > nVertexEnd) nVertexEnd = nSubsetVertEnd;



		// Compute the final vertex count.
		nVertexCount = (nVertexEnd - nVertexStart) + 1;

		// Render any batched data.
		if (nFaceCount > 0)
		{
			// Set vertex and index buffer source streams
			gfx::setVertexBuffer(mHardwareVB->handle, (std::uint32_t)nVertexStart, (std::uint32_t)nVertexCount);
			gfx::setIndexBuffer(mHardwareIB->handle, (std::uint32_t)nFaceStart * 3, (std::uint32_t)nFaceCount * 3);

		}
	}


	//-----------------------------------------------------------------------------
	//  Name : draw ()
	/// <summary>
	/// Draw the mesh (face count = nNumFaces).
	/// Note : Used during volume rendering.
	/// </summary>
	//-----------------------------------------------------------------------------
	void Mesh::draw(std::uint32_t nNumFaces)
	{
		// Set vertex and index buffer source streams
		gfx::setVertexBuffer(mHardwareVB->handle, 0, (std::uint32_t)mVertexCount);
		gfx::setIndexBuffer(mHardwareIB->handle, 0, (std::uint32_t)nNumFaces * 3);
	}


	//-----------------------------------------------------------------------------
	//  Name : generateVertexComponents () (Private)
	/// <summary>
	/// Some vertex components potentially need to be generated. This may
	/// include vertex normals, binormals or tangents. This function will
	/// generate any such components which were not provided.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::generateVertexComponents(bool bWeld)
	{
		// Vertex normals were requested (and at least some were not yet provided?)
		if (mForceNormalGen || mPrepareData.computeNormals)
		{
			// Generate the adjacency information for vertex normal computation
			UInt32Array adjacency;
			if (!generateAdjacency(adjacency))
			{
				//AppLog::write(AppLog::Error, _T("Failed to generate adjacency buffer for mesh containing %i faces.\n"), mPrepareData.triangleCount);
				return false;

			} // End if failed to generate

			  // Generate any vertex normals that have not been provided
			if (!generateVertexNormals(&adjacency.front()))
			{
				//AppLog::write(AppLog::Error, _T("Failed to generate vertex normals for mesh containing %i faces.\n"), mPrepareData.triangleCount);
				return false;

			} // End if failed to generate

		} // End if compute

		  // Weld vertices at this point
		if (bWeld)
		{
			if (!weldVertices())
			{
				//AppLog::write(AppLog::Error, _T("Failed to weld vertices for mesh containing %i faces.\n"), mPrepareData.triangleCount);
				return false;

			} // End if failed to weld

		} // End if optional weld

		  // Binormals and / or tangents were requested (and at least some where not yet provided?)
		if (mForceTangentGen || mPrepareData.computeBinormals || mPrepareData.computeTangents)
		{
			// Requires normals
			if (mVertexFormat.has(gfx::Attrib::Normal))
			{
				// Generate any vertex tangents that have not been provided
				if (!generateVertexTangents())
				{
					//AppLog::write(AppLog::Error, _T("Failed to generate vertex tangents for mesh containing %i faces.\n"), mPrepareData.triangleCount);
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
	bool Mesh::generateVertexNormals(std::uint32_t * pAdjacency, UInt32Array * pRemapArray /* = nullptr */)
	{
		std::uint32_t        nStartTri, nPreviousTri, nCurrentTri;
		math::vec3       vecEdge1, vecEdge2, vecNormal;
		std::uint32_t        i, j, k, nIndex;

		// Get access to useful data offset information.
		std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
		std::uint16_t nNormalOffset = mVertexFormat.getOffset(gfx::Attrib::Normal);
		std::uint16_t nVertexStride = mVertexFormat.getStride();

		// Final format requests vertex normals?
		if (nNormalOffset < 0)
			return true;

		// Size the remap array accordingly and populate it with the default mapping.
		std::uint32_t nOriginalVertexCount = mPrepareData.vertexCount;
		if (pRemapArray)
		{
			pRemapArray->resize(mPrepareData.vertexCount);
			for (i = 0; i < mPrepareData.vertexCount; ++i)
				(*pRemapArray)[i] = i;

		} // End if supplied

		  // Pre-compute surface normals for each triangle
		std::uint8_t * pSrcVertices = &mPrepareData.vertexData[0];
		math::vec3 * pNormals = new math::vec3[mPrepareData.triangleCount];
		memset(pNormals, 0, mPrepareData.triangleCount * sizeof(math::vec3));
		for (i = 0; i < mPrepareData.triangleCount; ++i)
		{
			// Retrieve positions of each referenced vertex.
			const Triangle & Tri = mPrepareData.triangleData[i];
			const math::vec3 * v1 = (math::vec3*)(pSrcVertices + (Tri.indices[0] * nVertexStride) + nPositionOffset);
			const math::vec3 * v2 = (math::vec3*)(pSrcVertices + (Tri.indices[1] * nVertexStride) + nPositionOffset);
			const math::vec3 * v3 = (math::vec3*)(pSrcVertices + (Tri.indices[2] * nVertexStride) + nPositionOffset);

			// Compute the two edge vectors required for generating our normal
			// We normalize here to prevent problems when the triangles are very small.
			vecEdge1 = math::normalize(*v2 - *v1);
			vecEdge2 = math::normalize(*v3 - *v1);

			// Generate the normal
			vecNormal = math::cross(vecEdge1, vecEdge2);
			pNormals[i] = math::normalize(vecNormal);

		} // Next Face

		  // Now compute the actual VERTEX normals using face adjacency information
		for (i = 0; i < mPrepareData.triangleCount; ++i)
		{
			Triangle & Tri = mPrepareData.triangleData[i];
			if (Tri.flags & TriangleFlags::Degenerate)
				continue;

			// Process each vertex in the face
			for (j = 0; j < 3; ++j)
			{
				// Retrieve the index for this vertex.
				nIndex = Tri.indices[j];

				// Skip this vertex if normal information was already provided.
				if (!mForceNormalGen && (mPrepareData.vertexFlags[nIndex] & PreparationData::SourceContainsNormal))
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
					vecNormal = pNormals[nStartTri];
					for (; ; )
					{
						// Stop walking if we reach the starting triangle again, or if there
						// is no connectivity out of this edge
						if (nCurrentTri == nStartTri || nCurrentTri == 0xFFFFFFFF)
							break;

						// Add this normal.
						vecNormal += pNormals[nCurrentTri];

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
				vecNormal = math::normalize(vecNormal);

				// If the normal we are about to store is significantly different from any normal
				// already stored in this vertex (excepting the case where it is <0,0,0>), we need
				// to split the vertex into two.
				math::vec3 * pRefNormal = (math::vec3*)(pSrcVertices + (nIndex * nVertexStride) + nNormalOffset);
				if (pRefNormal->x == 0.0f && pRefNormal->y == 0.0f && pRefNormal->z == 0.0f)
				{
					*pRefNormal = vecNormal;

				} // End if no normal stored here yet
				else
				{
					// Split and store in a new vertex if it is different (enough)
					if (math::abs(pRefNormal->x - vecNormal.x) >= 1e-3f ||
						math::abs(pRefNormal->y - vecNormal.y) >= 1e-3f ||
						math::abs(pRefNormal->z - vecNormal.z) >= 1e-3f)
					{
						// Make room for new vertex data.
						mPrepareData.vertexData.resize(mPrepareData.vertexData.size() + nVertexStride);

						// Ensure that we update the 'pSrcVertices' pointer (used throughout the
						// loop). The internal buffer wrapped by the resized vertex data vector
						// may have been re-allocated.
						pSrcVertices = &mPrepareData.vertexData[0];

						// Duplicate the vertex at the end of the buffer
						memcpy(pSrcVertices + (mPrepareData.vertexCount * nVertexStride),
							pSrcVertices + (nIndex * nVertexStride),
							nVertexStride);

						// Duplicate any other remaining information.
						mPrepareData.vertexFlags.push_back(mPrepareData.vertexFlags[nIndex]);

						// Record the split
						if (pRemapArray)
							(*pRemapArray)[nIndex] = mPrepareData.vertexCount;

						// Store the new normal and finally record the fact that we have
						// added a new vertex.
						nIndex = mPrepareData.vertexCount++;
						pRefNormal = (math::vec3*)(pSrcVertices + (nIndex * nVertexStride) + nNormalOffset);
						*pRefNormal = vecNormal;

						// Update the index
						Tri.indices[j] = nIndex;

					} // End if normal is different

				} // End if normal already stored here

			} // Next Vertex

		} // Next Face

		  // We're done with the surface normals
		checked_array_delete(pNormals);

		// If no new vertices were introduced, then it is not necessary
		// for the caller to remap anything.
		if (pRemapArray && nOriginalVertexCount == mPrepareData.vertexCount)
			pRemapArray->clear();

		// Success!
		return true;
	}

	//-----------------------------------------------------------------------------
	//  Name : generateVertexTangents () 
	/// <summary>
	/// Builds the tangent space vectors for this polygon. 
	/// Credit to Terathon Software - http://www.terathon.com/code/tangent.html 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool Mesh::generateVertexTangents()
	{
		math::vec3     * pTangents = nullptr, *pBinormals = nullptr;
		std::uint32_t        i, i1, i2, i3, nNumFaces, nNumVerts;
		math::vec3       P, Q, T, B, vCross, vNormal;
		float         s1, t1, s2, t2, r;
		math::plane         Plane;

		// Get access to useful data offset information.
		std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
		std::uint16_t nNormalOffset = mVertexFormat.getOffset(gfx::Attrib::Normal);
		std::uint16_t nBinormalOffset = mVertexFormat.getOffset(gfx::Attrib::Bitangent);
		std::uint16_t nTangentOffset = mVertexFormat.getOffset(gfx::Attrib::Tangent);
		std::uint16_t nTexCoordOffset = mVertexFormat.getOffset(gfx::Attrib::TexCoord0);
		std::uint16_t nVertexStride = mVertexFormat.getStride();

		// Final format requests tangents?
		bool bRequiresBinormal = (nBinormalOffset > 0);
		bool bRequiresTangent = (nTangentOffset > 0);
		if (!mForceTangentGen && !bRequiresBinormal && !bRequiresTangent)
			return true;

		// This will fail if we don't already have normals however.
		if (nNormalOffset < 0)
			return false;

		// Allocate storage space for the tangent and binormal vectors
		// that we will effectively need to average for shared vertices.
		nNumFaces = mPrepareData.triangleCount;
		nNumVerts = mPrepareData.vertexCount;
		pTangents = new math::vec3[nNumVerts];
		pBinormals = new math::vec3[nNumVerts];
		memset(pTangents, 0, sizeof(math::vec3) * nNumVerts);
		memset(pBinormals, 0, sizeof(math::vec3) * nNumVerts);

		// Iterate through each triangle in the mesh
		std::uint8_t * pSrcVertices = &mPrepareData.vertexData[0];
		for (i = 0; i < nNumFaces; ++i)
		{
			Triangle & Tri = mPrepareData.triangleData[i];

			// Compute the three indices for the triangle
			i1 = Tri.indices[0];
			i2 = Tri.indices[1];
			i3 = Tri.indices[2];

			// Retrieve references to the positions of the three vertices in the triangle.
			math::vec3 E;
			float fE[4];
			gfx::vertexUnpack(fE, gfx::Attrib::Position, mVertexFormat, pSrcVertices, i1);
			math::vec3 F;
			float fF[4];
			gfx::vertexUnpack(fF, gfx::Attrib::Position, mVertexFormat, pSrcVertices, i2);
			math::vec3 G;
			float fG[4];
			gfx::vertexUnpack(fG, gfx::Attrib::Position, mVertexFormat, pSrcVertices, i3);
			memcpy(&E[0], fE, 3 * sizeof(float));
			memcpy(&F[0], fF, 3 * sizeof(float));
			memcpy(&G[0], fG, 3 * sizeof(float));
			//const math::vec3 & E = *(math::vec3*)(pSrcVertices + (i1 * nVertexStride) + nPositionOffset);
			//const math::vec3 & F = *(math::vec3*)(pSrcVertices + (i2 * nVertexStride) + nPositionOffset);
			//const math::vec3 & G = *(math::vec3*)(pSrcVertices + (i3 * nVertexStride) + nPositionOffset);

			// Retrieve references to the base texture coordinates of the three vertices in the triangle.
			// TODO: Allow customization of which tex coordinates to generate from.
			math::vec2 Et;
			float fEt[4];
			gfx::vertexUnpack(&fEt[0], gfx::Attrib::TexCoord0, mVertexFormat, pSrcVertices, i1);
			math::vec2 Ft;
			float fFt[4];
			gfx::vertexUnpack(&fFt[0], gfx::Attrib::TexCoord0, mVertexFormat, pSrcVertices, i2);
			math::vec2 Gt;
			float fGt[4];
			gfx::vertexUnpack(&fGt[0], gfx::Attrib::TexCoord0, mVertexFormat, pSrcVertices, i3);
			memcpy(&Et[0], fEt, 2 * sizeof(float));
			memcpy(&Ft[0], fFt, 2 * sizeof(float));
			memcpy(&Gt[0], fGt, 2 * sizeof(float));
			//const math::vec2 & Et = *(math::vec2*)(pSrcVertices + (i1 * nVertexStride) + nTexCoordOffset);
			//const math::vec2 & Ft = *(math::vec2*)(pSrcVertices + (i2 * nVertexStride) + nTexCoordOffset);
			//const math::vec2 & Gt = *(math::vec2*)(pSrcVertices + (i3 * nVertexStride) + nTexCoordOffset);

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
			pTangents[i1] += T;
			pTangents[i2] += T;
			pTangents[i3] += T;
			pBinormals[i1] += B;
			pBinormals[i2] += B;
			pBinormals[i3] += B;

		} // Next Triangle 

		  // Generate final tangent vectors 
		for (i = 0; i < nNumVerts; i++, pSrcVertices += nVertexStride)
		{
			// Skip if the original imported data already provided a binormal / tangent.
			bool bHasBinormal = ((mPrepareData.vertexFlags[i] & PreparationData::SourceContainsBinormal) != 0);
			bool bHasTangent = ((mPrepareData.vertexFlags[i] & PreparationData::SourceContainsTangent) != 0);
			if (!mForceTangentGen && bHasBinormal && bHasTangent)
				continue;

			// Retrieve the normal vector from the vertex and the computed
			// tangent vector.
			float normal[4];
			gfx::vertexUnpack(normal, gfx::Attrib::Normal, mVertexFormat, pSrcVertices);
			memcpy(&vNormal[0], normal, 3 * sizeof(float));
			//vNormal = *(math::vec3*)(pSrcVertices + nNormalOffset);
			T = pTangents[i];

			// GramSchmidt orthogonalize
			T = T - (vNormal * math::dot(vNormal, T));
			T = math::normalize(T);

			// Store tangent if required
			if (mForceTangentGen || (!bHasTangent && bRequiresTangent))
				gfx::vertexPack(&math::vec4(T, 1.0f)[0], true, gfx::Attrib::Tangent, mVertexFormat, pSrcVertices);

			// Compute and store binormal if required
			if (mForceTangentGen || (!bHasBinormal && bRequiresBinormal))
			{
				// Calculate the new orthogonal binormal
				B = math::cross(vNormal, T);
				B = math::normalize(B);

				// Compute the "handedness" of the tangent and binormal. This
				// ensures the inverted / mirrored texture coordinates still have
				// an accurate matrix.
				vCross = math::cross(vNormal, T);
				if (math::dot(vCross, pBinormals[i]) < 0.0f)
				{
					// Flip the binormal
					B = -B;

				} // End if coordinates inverted

				  // Store.
				gfx::vertexPack(&math::vec4(B, 1.0f)[0], true, gfx::Attrib::Bitangent, mVertexFormat, pSrcVertices);
				//*(math::vec3*)(pSrcVertices + nBinormalOffset) = B;

			} // End if requires binormal   

		} // Next vertex

		  // Cleanup 
		checked_array_delete(pTangents);
		checked_array_delete(pBinormals);

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
	bool Mesh::generateAdjacency(UInt32Array & adjacency)
	{
		std::map< AdjacentEdgeKey, std::uint32_t > EdgeTree;
		std::map< AdjacentEdgeKey, std::uint32_t >::iterator itEdge;

		// What is the status of the mesh?
		if (mPrepareStatus != MeshStatus::Prepared)
		{
			// Validate requirements
			if (mPrepareData.triangleCount == 0)
				return false;

			// Retrieve useful data offset information.
			std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
			std::uint16_t nVertexStride = mVertexFormat.getStride();

			// Insert all edges into the edge tree
			std::uint8_t * pSrcVertices = &mPrepareData.vertexData[0] + nPositionOffset;
			for (std::uint32_t i = 0; i < mPrepareData.triangleCount; ++i)
			{
				AdjacentEdgeKey Edge;

				// Degenerate triangles cannot participate.
				const Triangle & Tri = mPrepareData.triangleData[i];
				if (Tri.flags & TriangleFlags::Degenerate)
					continue;

				// Retrieve positions of each referenced vertex.
				const math::vec3 * v1 = (math::vec3*)(pSrcVertices + (Tri.indices[0] * nVertexStride));
				const math::vec3 * v2 = (math::vec3*)(pSrcVertices + (Tri.indices[1] * nVertexStride));
				const math::vec3 * v3 = (math::vec3*)(pSrcVertices + (Tri.indices[2] * nVertexStride));

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
			adjacency.resize(mPrepareData.triangleCount * 3, 0xFFFFFFFF);

			// Now, find any adjacent edges for each triangle edge
			for (std::uint32_t i = 0; i < mPrepareData.triangleCount; ++i)
			{
				AdjacentEdgeKey Edge;

				// Degenerate triangles cannot participate.
				const Triangle & Tri = mPrepareData.triangleData[i];
				if (Tri.flags & TriangleFlags::Degenerate)
					continue;

				// Retrieve positions of each referenced vertex.
				const math::vec3 * v1 = (math::vec3*)(pSrcVertices + (Tri.indices[0] * nVertexStride));
				const math::vec3 * v2 = (math::vec3*)(pSrcVertices + (Tri.indices[1] * nVertexStride));
				const math::vec3 * v3 = (math::vec3*)(pSrcVertices + (Tri.indices[2] * nVertexStride));

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
			if (mFaceCount == 0)
				return false;

			// Retrieve useful data offset information.
			std::uint16_t nPositionOffset = mVertexFormat.getOffset(gfx::Attrib::Position);
			std::uint16_t nVertexStride = mVertexFormat.getStride();

			// Insert all edges into the edge tree
			std::uint8_t * pSrcVertices = mSystemVB + nPositionOffset;
			std::uint32_t * pSrcIndices = mSystemIB;
			for (std::uint32_t i = 0; i < mFaceCount; ++i, pSrcIndices += 3)
			{
				AdjacentEdgeKey Edge;

				// Retrieve positions of each referenced vertex.
				const math::vec3 * v1 = (math::vec3*)(pSrcVertices + (pSrcIndices[0] * nVertexStride));
				const math::vec3 * v2 = (math::vec3*)(pSrcVertices + (pSrcIndices[1] * nVertexStride));
				const math::vec3 * v3 = (math::vec3*)(pSrcVertices + (pSrcIndices[2] * nVertexStride));

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
			adjacency.resize(mFaceCount * 3, 0xFFFFFFFF);

			// Now, find any adjacent edges for each triangle edge
			pSrcIndices = mSystemIB;
			for (std::uint32_t i = 0; i < mFaceCount; ++i, pSrcIndices += 3)
			{
				AdjacentEdgeKey Edge;

				// Retrieve positions of each referenced vertex.
				const math::vec3 * v1 = (math::vec3*)(pSrcVertices + (pSrcIndices[0] * nVertexStride));
				const math::vec3 * v2 = (math::vec3*)(pSrcVertices + (pSrcIndices[1] * nVertexStride));
				const math::vec3 * v3 = (math::vec3*)(pSrcVertices + (pSrcIndices[2] * nVertexStride));

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
	bool Mesh::weldVertices(UInt32Array * pVertexRemap /* = nullptr */)
	{
		//		gfx::weldVertices()
		// 		WeldKey Key;
		// 		std::map< WeldKey, std::uint32_t > VertexTree;
		// 		std::map< WeldKey, std::uint32_t >::const_iterator itKey;
		// 		ByteArray NewVertexData, NewVertexFlags;
		// 		std::uint32_t nNewVertexCount = 0;
		// 
		// 		// Allocate enough space to build the remap array for the existing vertices
		// 		if (pVertexRemap)
		// 			pVertexRemap->resize(mPrepareData.vertexCount);
		// 		std::uint32_t * pCollapseMap = new std::uint32_t[mPrepareData.vertexCount];
		// 
		// 		// Retrieve useful data offset information.
		// 		std::uint16_t nVertexStride = mVertexFormat.getStride();
		// 
		// 		// For each vertex to be welded.
		// 		for (std::uint32_t i = 0; i < mPrepareData.vertexCount; ++i)
		// 		{
		// 			// Build a new key structure for inserting
		// 			Key.vertex = (&mPrepareData.vertexData[0]) + (i * nVertexStride);
		// 			Key.format = mVertexFormat;
		// 			Key.tolerance = 0.000001f;
		// 
		// 			// Does a vertex with matching details already exist in the tree.
		// 			itKey = VertexTree.find(Key);
		// 			if (itKey == VertexTree.end())
		// 			{
		// 				// No matching vertex. Insert into the tree (value = NEW index of vertex).
		// 				VertexTree[Key] = nNewVertexCount;
		// 				pCollapseMap[i] = nNewVertexCount;
		// 				if (pVertexRemap)
		// 					(*pVertexRemap)[i] = nNewVertexCount;
		// 
		// 				// Store the vertex in the new buffer
		// 				NewVertexData.resize((nNewVertexCount + 1) * nVertexStride);
		// 				memcpy(&NewVertexData[nNewVertexCount * nVertexStride], Key.vertex, nVertexStride);
		// 				NewVertexFlags.push_back(mPrepareData.vertexFlags[i]);
		// 				nNewVertexCount++;
		// 
		// 			} // End if no matching vertex
		// 			else
		// 			{
		// 				// A vertex already existed at this location.
		// 				// Just mark the 'collapsed' index for this vertex in the remap array.
		// 				pCollapseMap[i] = itKey->second;
		// 				if (pVertexRemap)
		// 					(*pVertexRemap)[i] = 0xFFFFFFFF;
		// 
		// 			} // End if vertex already existed
		// 
		// 		} // Next Vertex
		// 
		// 		  // If nothing was welded, just bail
		// 		if (mPrepareData.vertexCount == nNewVertexCount)
		// 		{
		// 			checked_array_delete(pCollapseMap);
		// 
		// 			if (pVertexRemap)
		// 				pVertexRemap->clear();
		// 			return true;
		// 
		// 		} // End if nothing to do
		// 
		// 		  // Otherwise, replace the old preparation vertices and remap
		// 		mPrepareData.vertexData.clear();
		// 		mPrepareData.vertexData.resize(NewVertexData.size());
		// 		memcpy(&mPrepareData.vertexData[0], &NewVertexData[0], NewVertexData.size());
		// 		mPrepareData.vertexFlags.clear();
		// 		mPrepareData.vertexFlags.resize(NewVertexFlags.size());
		// 		memcpy(&mPrepareData.vertexFlags[0], &NewVertexFlags[0], NewVertexFlags.size());
		// 		mPrepareData.vertexCount = nNewVertexCount;
		// 
		// 		// Now remap all the triangle indices
		// 		for (std::uint32_t i = 0; i < mPrepareData.triangleCount; ++i)
		// 		{
		// 			Triangle & Tri = mPrepareData.triangleData[i];
		// 			Tri.indices[0] = pCollapseMap[Tri.indices[0]];
		// 			Tri.indices[1] = pCollapseMap[Tri.indices[1]];
		// 			Tri.indices[2] = pCollapseMap[Tri.indices[2]];
		// 
		// 		} // Next Triangle
		// 
		// 		  // Clean up
		// 		checked_array_delete(pCollapseMap);

				// Success!
		return true;
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

		nDifference = (std::int32_t)Key1.dataGroupId - (std::int32_t)Key2.dataGroupId;
		if (nDifference != 0) return (nDifference < 0);

		// Exactly equal
		return false;
	}

}