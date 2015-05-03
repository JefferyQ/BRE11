#include "QuadCulledPointLightGsData.h"

#include <d3d11_1.h>
#include <memory>

#include <managers/ShadersManager.h>
#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>
#include <utils/Utility.h>

using namespace DirectX;

namespace {
	const char* sShaderFile = "content\\shaders\\lightPasses\\fullyDeferred\\QuadCulledPointLightGS.cso";
}

namespace BRE {
	namespace FullyDeferred {
		QuadCulledPointLightGeometryShaderData::QuadCulledPointLightGeometryShaderData()
			: mShader(ShadersManager::gInstance->LoadGeometryShader(sShaderFile))
		{
			ASSERT_PTR(mShader);
			InitializeCBuffers();
		}

		void QuadCulledPointLightGeometryShaderData::InitializeCBuffers() {
			// Initialize constant buffer
			ASSERT_COND(mCBufferPerFrame == nullptr);
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.ByteWidth = static_cast<unsigned int> (sizeof(CBufferPerFrameData));
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;

			mCBufferPerFrame = ShaderResourcesManager::gInstance->AddBuffer(reinterpret_cast<size_t>(this), bufferDesc, nullptr);
			ASSERT_PTR(mCBufferPerFrame);
		}

		void QuadCulledPointLightGeometryShaderData::PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context) {
			// Set shader
			ASSERT_PTR(mShader);
			context.GSSetShader(mShader, nullptr, 0);

			// Set constant buffers
			ASSERT_PTR(mCBufferPerFrame);
			Utility::CopyData(device, &mCBufferPerFrameData, sizeof(CBufferPerFrameData), *mCBufferPerFrame);
			ID3D11Buffer* const cBuffers[] = { mCBufferPerFrame };
			context.GSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
		}

		void QuadCulledPointLightGeometryShaderData::PostDraw(ID3D11DeviceContext1& context) {
			// Set shader
			context.GSSetShader(nullptr, nullptr, 0);

			// Set constant buffers
			ID3D11Buffer* const cBuffers[] = { nullptr };
			context.GSSetConstantBuffers(0, ARRAYSIZE(cBuffers), cBuffers);
		}
	}
}