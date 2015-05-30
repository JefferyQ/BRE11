#include "VertexType.h"

#include <d3d11_1.h>

#include <managers/ModelManager.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/models/Mesh.h>
#include <rendering/models/Model.h>
#include <utils/Assert.h>
#include <utils/Hash.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace BRE {
	NormalMappingVertexData::NormalMappingVertexData() {
	}

	NormalMappingVertexData::NormalMappingVertexData(const XMFLOAT4& posL, const XMFLOAT2& texC, const XMFLOAT3& normalL, const XMFLOAT3& tangentL)
		: mPosL(posL)
		, mTexC(texC)
		, mNormalL(normalL)
		, mTangentL(tangentL)
	{
	}

	size_t NormalMappingVertexData::CreateVertexBuffer(const size_t modelId) {
		// Check if there is already a buffer for current model
		// and current vertex type.
		ASSERT_PTR(ModelManager::gInstance->GetModel(modelId));
		const Model& model = *ModelManager::gInstance->GetModel(modelId);
		std::string bufferName = model.Filename();
		bufferName += "_";
		bufferName += "NormalMappingVertexData";
		bufferName += "_vertexBuffer";
		const size_t bufferId = Hash(bufferName.c_str());
		if (ShaderResourcesManager::gInstance->Buffer(bufferId)) {
			return bufferId;
		}

		// Create buffer
		ASSERT_COND(model.Meshes().size() == 1);
		BRE::Mesh& mesh = *model.Meshes()[0];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const std::vector<XMFLOAT3>& textureCoordinates = mesh.TextureCoordinates();
		ASSERT_COND(textureCoordinates.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		ASSERT_COND(normals.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& tangents = mesh.Tangents();
		ASSERT_COND(tangents.size() == sourceVertices.size());
		const size_t numVerts = sourceVertices.size();
		std::vector<NormalMappingVertexData> vertices;
		vertices.reserve(numVerts);
		for (size_t i = 0; i < numVerts; i++) {
			const XMFLOAT3& posL = sourceVertices[i];
			const XMFLOAT3& uv = textureCoordinates[i];
			const XMFLOAT3& normalL = normals[i];
			const XMFLOAT3& tangentL = tangents[i];
			vertices.push_back(NormalMappingVertexData(XMFLOAT4(posL.x, posL.y, posL.z, 1.0f), XMFLOAT2(uv.x, uv.y), normalL, tangentL));
		}
		const unsigned int bufferSize = static_cast<unsigned int> (vertices.size() * sizeof(NormalMappingVertexData));
		Utility::CreateInitializedBuffer(bufferName.c_str(), &vertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);

		return bufferId;
	}

	ColorVertexData::ColorVertexData() {
	}

	ColorVertexData::ColorVertexData(const XMFLOAT4& posL, const XMFLOAT4& color, const XMFLOAT3& normalL)
		: mPosL(posL)
		, mColor(color)
		, mNormalL(normalL)
	{
	}

	size_t ColorVertexData::CreateVertexBuffer(const size_t modelId) {
		// Check if there is already a buffer for current model
		// and current vertex type.
		ASSERT_PTR(ModelManager::gInstance->GetModel(modelId));
		const Model& model = *ModelManager::gInstance->GetModel(modelId);
		std::string bufferName = model.Filename();
		bufferName += "_";
		bufferName += "ColorVertexData";
		bufferName += "_vertexBuffer";
		const size_t bufferId = Hash(bufferName.c_str());
		if (ShaderResourcesManager::gInstance->Buffer(bufferId)) {
			return bufferId;
		}

		// Create buffer
		ASSERT_COND(model.Meshes().size() == 1);
		BRE::Mesh& mesh = *model.Meshes()[0];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const std::vector<XMFLOAT4>& colors = mesh.Colors();
		ASSERT_COND(colors.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		ASSERT_COND(normals.size() == sourceVertices.size());
		const size_t numVerts = sourceVertices.size();
		std::vector<ColorVertexData> vertices;
		vertices.reserve(numVerts);
		for (size_t i = 0; i < numVerts; i++) {
			const XMFLOAT3& posL = sourceVertices[i];
			const XMFLOAT4& color = colors[i];
			const XMFLOAT3& normalL = normals[i];
			vertices.push_back(ColorVertexData(XMFLOAT4(posL.x, posL.y, posL.z, 1.0f), color, normalL));
		}
		const unsigned int bufferSize = static_cast<unsigned int> (vertices.size() * sizeof(ColorVertexData));
		Utility::CreateInitializedBuffer(bufferName.c_str(), &vertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);

		return bufferId;
	}

	BasicVertexData::BasicVertexData() {
	}

	BasicVertexData::BasicVertexData(const XMFLOAT4& posL, const XMFLOAT3& normalL)
		: mPosL(posL)
		, mNormalL(normalL)
	{
	}

	size_t BasicVertexData::CreateVertexBuffer(const size_t modelId) {
		// Check if there is already a buffer for current model
		// and current vertex type.
		ASSERT_PTR(ModelManager::gInstance->GetModel(modelId));
		const Model& model = *ModelManager::gInstance->GetModel(modelId);
		std::string bufferName = model.Filename();
		bufferName += "_";
		bufferName += "BasicVertexData";
		bufferName += "_vertexBuffer";
		const size_t bufferId = Hash(bufferName.c_str());
		if (ShaderResourcesManager::gInstance->Buffer(bufferId)) {
			return bufferId;
		}

		// Create buffer
		ASSERT_COND(model.Meshes().size() == 1);
		BRE::Mesh& mesh = *model.Meshes()[0];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		ASSERT_COND(normals.size() == sourceVertices.size());
		const size_t numVerts = sourceVertices.size();
		std::vector<BasicVertexData> vertices;
		vertices.reserve(numVerts);
		for (size_t i = 0; i < numVerts; i++) {
			const XMFLOAT3& posL = sourceVertices[i];
			const XMFLOAT3& normalL = normals[i];
			vertices.push_back(BasicVertexData(XMFLOAT4(posL.x, posL.y, posL.z, 1.0f), normalL));
		}
		const unsigned int bufferSize = static_cast<unsigned int> (vertices.size() * sizeof(BasicVertexData));
		Utility::CreateInitializedBuffer(bufferName.c_str(), &vertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);

		return bufferId;
	}
}