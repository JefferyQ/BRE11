#include "VertexType.h"

#include <d3d11_1.h>
#include <sstream>

#include <managers/ModelManager.h>
#include <managers/ShaderResourcesManager.h>
#include <rendering/models/Mesh.h>
#include <rendering/models/Model.h>
#include <utils/Assert.h>
#include <utils/Hash.h>
#include <utils/DXUtils.h>

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

	size_t NormalMappingVertexData::CreateVertexBuffer(const size_t modelId, const size_t meshIndex) {
		// Check if there is already a buffer for current model
		// and current vertex type.
		BRE_ASSERT(ModelManager::gInstance->GetModel(modelId));
		const Model& model = *ModelManager::gInstance->GetModel(modelId);
		BRE_ASSERT(meshIndex < model.Meshes().size());
		std::string bufferName = model.Filename();
		bufferName += "_";
		bufferName += "NormalMappingVertexData";
		bufferName += "_vertexBuffer";
		std::stringstream stream;
		stream << meshIndex;
		bufferName += stream.str();
		const size_t bufferId = Utils::Hash(bufferName.c_str());
		if (ShaderResourcesManager::gInstance->Buffer(bufferId)) {
			return bufferId;
		}

		// Create buffer
		BRE::Mesh& mesh = *model.Meshes()[meshIndex];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const std::vector<XMFLOAT3>& textureCoordinates = mesh.TextureCoordinates();
		BRE_ASSERT(textureCoordinates.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		BRE_ASSERT(normals.size() == sourceVertices.size());
		const std::vector<XMFLOAT3>& tangents = mesh.Tangents();
		BRE_ASSERT(tangents.size() == sourceVertices.size());
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
		Utils::CreateInitializedBuffer(bufferName.c_str(), &vertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);

		return bufferId;
	}
	
	BasicVertexData::BasicVertexData() {
	}

	BasicVertexData::BasicVertexData(const XMFLOAT4& posL, const XMFLOAT3& normalL)
		: mPosL(posL)
		, mNormalL(normalL)
	{
	}

	size_t BasicVertexData::CreateVertexBuffer(const size_t modelId, const size_t meshIndex) {
		// Check if there is already a buffer for current model
		// and current vertex type.
		BRE_ASSERT(ModelManager::gInstance->GetModel(modelId));
		const Model& model = *ModelManager::gInstance->GetModel(modelId);
		BRE_ASSERT(meshIndex < model.Meshes().size());
		std::string bufferName = model.Filename();
		bufferName += "_";
		bufferName += "BasicVertexData";
		bufferName += "_vertexBuffer";
		std::stringstream stream;
		stream << meshIndex;
		bufferName += stream.str();
		const size_t bufferId = Utils::Hash(bufferName.c_str());
		if (ShaderResourcesManager::gInstance->Buffer(bufferId)) {
			return bufferId;
		}

		// Create buffer
		BRE::Mesh& mesh = *model.Meshes()[meshIndex];
		const std::vector<XMFLOAT3>& sourceVertices = mesh.Vertices();
		const std::vector<XMFLOAT3>& normals = mesh.Normals();
		BRE_ASSERT(normals.size() == sourceVertices.size());
		const size_t numVerts = sourceVertices.size();
		std::vector<BasicVertexData> vertices;
		vertices.reserve(numVerts);
		for (size_t i = 0; i < numVerts; i++) {
			const XMFLOAT3& posL = sourceVertices[i];
			const XMFLOAT3& normalL = normals[i];
			vertices.push_back(BasicVertexData(XMFLOAT4(posL.x, posL.y, posL.z, 1.0f), normalL));
		}
		const unsigned int bufferSize = static_cast<unsigned int> (vertices.size() * sizeof(BasicVertexData));
		Utils::CreateInitializedBuffer(bufferName.c_str(), &vertices[0], bufferSize, D3D11_USAGE_IMMUTABLE, D3D11_BIND_VERTEX_BUFFER);

		return bufferId;
	}
}