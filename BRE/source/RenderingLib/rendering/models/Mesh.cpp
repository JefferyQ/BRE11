#include "Mesh.h"

#include <d3d11_1.h>
#include <memory>
#include <assimp/scene.h>

#include <general/Application.h>
#include <rendering/models/Model.h>
#include <utils/Assert.h>
#include <utils\Utility.h>

using namespace DirectX;

namespace BRE {
	Mesh::Mesh(Model& model, const aiMesh& mesh)
		: mModel(model)
		, mMaterial(nullptr)
		, mName(mesh.mName.C_Str())
		, mFaceCount(0)
	{
		mMaterial = mModel.Materials()[mesh.mMaterialIndex];
		ASSERT_PTR(mMaterial);

		// Vertices
		ASSERT_COND(mesh.mNumVertices > 0);
		mVertices.reserve(mesh.mNumVertices);
		for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
			mVertices.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mVertices[i])));
		}

		// Normals
		ASSERT_COND(mesh.HasNormals());
		mNormals.reserve(mesh.mNumVertices);
		for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
			mNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mNormals[i])));
		}
		

		// Texture Coordinates
		if (mesh.HasTextureCoords(0)) {
			ASSERT_COND(mesh.GetNumUVChannels() == 1);
			mTextureCoordinates.reserve(mesh.mNumVertices);
			const aiVector3D* aiTextureCoordinates = mesh.mTextureCoords[0];
			ASSERT_PTR(aiTextureCoordinates);
			for (unsigned int j = 0; j < mesh.mNumVertices; j++) {
				mTextureCoordinates.push_back(XMFLOAT3(reinterpret_cast<const float*>(&aiTextureCoordinates[j])));
			}
		}

		// Colors
		if (mesh.HasVertexColors(0)) {
			ASSERT_COND(mesh.GetNumColorChannels() == 1);
			const aiColor4D* aiColors = mesh.mColors[0];
			for (unsigned int j = 0; j < mesh.mNumVertices; j++) {
				mColors.push_back(XMFLOAT4(reinterpret_cast<const float*>(&aiColors[j])));
			}
		}		

		// We only allow triangles
		ASSERT_COND(mesh.HasFaces());
		mFaceCount = mesh.mNumFaces;
		for (unsigned int i = 0; i < mFaceCount; ++i) {
			const aiFace* face = &mesh.mFaces[i];
			ASSERT_PTR(face);
			ASSERT_COND(face->mNumIndices == 3);
			mIndices.push_back(face->mIndices[0]);
			mIndices.push_back(face->mIndices[1]);
			mIndices.push_back(face->mIndices[2]);
		}
		

		// Tangents and Binormals
		mTangents.reserve(mesh.mNumVertices);
		mBiNormals.reserve(mesh.mNumVertices);
		if (mesh.HasTangentsAndBitangents()) {
			for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
				mTangents.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mTangents[i])));
				mBiNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mBitangents[i])));
			}
		}
		/*else {
			Utility::CalculateTangentArray(*this, mTangents);
		}*/

	}

	Mesh::~Mesh() {
	}
}