#include "Mesh.h"

#include <d3d11_1.h>
#include <memory>
#include <assimp/scene.h>

#include <general/Application.h>
#include <rendering/models/Model.h>
#include <utils/Assert.h>

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
		mVertices.reserve(mesh.mNumVertices);
		for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
			mVertices.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mVertices[i])));
		}

		// Normals
		if (mesh.HasNormals()) {
			mNormals.reserve(mesh.mNumVertices);
			for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
				mNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mNormals[i])));
			}
		}

		// Texture Coordinates
		const unsigned int uvChannelCount = mesh.GetNumUVChannels();
		for (unsigned int i = 0; i < uvChannelCount; ++i) {
			std::vector<XMFLOAT3>* textureCoordinates = new std::vector<XMFLOAT3>();
			textureCoordinates->reserve(mesh.mNumVertices);
			mTextureCoordinates.push_back(textureCoordinates);

			const aiVector3D* aiTextureCoordinates = mesh.mTextureCoords[i];
			ASSERT_PTR(aiTextureCoordinates);
			for (unsigned int j = 0; j < mesh.mNumVertices; j++) {
				textureCoordinates->push_back(XMFLOAT3(reinterpret_cast<const float*>(&aiTextureCoordinates[j])));
			}
		}

		// Vertex Colors
		const unsigned int colorChannelCount = mesh.GetNumColorChannels();
		for (unsigned int i = 0; i < colorChannelCount; ++i) {
			std::vector<XMFLOAT4>* vertexColors = new std::vector<XMFLOAT4>();
			vertexColors->reserve(mesh.mNumVertices);
			mVertexColors.push_back(vertexColors);

			const aiColor4D* aiVertexColors = mesh.mColors[i];
			ASSERT_PTR(aiVertexColors);
			for (unsigned int j = 0; j < mesh.mNumVertices; j++) {
				vertexColors->push_back(XMFLOAT4(reinterpret_cast<const float*>(&aiVertexColors[j])));
			}
		}

		// Faces (note: could pre-reserve if we limit primitive types)
		if (mesh.HasFaces()) {
			ASSERT_PTR(mesh.mFaces);
			mFaceCount = mesh.mNumFaces;
			for (unsigned int i = 0; i < mFaceCount; ++i) {
				const aiFace* face = &mesh.mFaces[i];
				ASSERT_PTR(face);
				for (unsigned int j = 0; j < face->mNumIndices; ++j) {
					mIndices.push_back(face->mIndices[j]);
				}
			}
		}

		// Tangents and Binormals
		ASSERT_COND(mesh.HasTangentsAndBitangents());
		mTangents.reserve(mesh.mNumVertices);
		mBiNormals.reserve(mesh.mNumVertices);
		for (unsigned int i = 0; i < mesh.mNumVertices; ++i) {
			mTangents.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mTangents[i])));
			mBiNormals.push_back(XMFLOAT3(reinterpret_cast<const float*>(&mesh.mBitangents[i])));
		}
	}

	Mesh::~Mesh() {
		for (std::vector<XMFLOAT3>* textureCoordinates : mTextureCoordinates) {
			delete textureCoordinates;
		}

		for (std::vector<XMFLOAT4>* vertexColors : mVertexColors) {
			delete vertexColors;
		}
	}
}