#pragma once

#include <DirectXMath.h>
#include <vector>

struct aiMesh;
struct ID3D11Buffer;
struct ID3D11Device1;

namespace BRE {
	class Material;
	class ModelMaterial;

	class Mesh {
		friend class Model;

	public:
		Mesh(const Mesh& rhs) = delete;
		Mesh& operator=(const Mesh& rhs) = delete;
		~Mesh();

		const Model& GetModel() const { return mModel; }
		const ModelMaterial& GetMaterial() const { return *mMaterial; }
		const std::string& Name() const { return mName; }
		const std::vector<DirectX::XMFLOAT3>& Vertices() const { return mVertices; }
		const std::vector<DirectX::XMFLOAT3>& Normals() const { return mNormals; }
		const std::vector<DirectX::XMFLOAT3>& Tangents() const { return mTangents; }
		const std::vector<DirectX::XMFLOAT3>& BiNormals() const { return mBiNormals; }
		const std::vector<DirectX::XMFLOAT3>& TextureCoordinates() const { return mTextureCoordinates; }
		unsigned int FaceCount() const { return mFaceCount; }
		const std::vector<unsigned int>& Indices() const { return mIndices; }

	private:
		Mesh(Model& model, const aiMesh& mesh);

		Model& mModel;
		const ModelMaterial* mMaterial;
		std::string mName;
		std::vector<DirectX::XMFLOAT3> mVertices;
		std::vector<DirectX::XMFLOAT3> mNormals;
		std::vector<DirectX::XMFLOAT3> mTangents;
		std::vector<DirectX::XMFLOAT3> mBiNormals;
		std::vector<DirectX::XMFLOAT3> mTextureCoordinates;
		unsigned int mFaceCount;
		std::vector<unsigned int> mIndices;
	};
}