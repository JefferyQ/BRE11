#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>
#include <rendering/shaders/LightsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class DirLightPixelShaderData {
	public:
		DirLightPixelShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[6]);
		void PostDraw(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT3& CameraPosVS() { return mCBuffer.mData.mCameraPosVS; }
		DirectionalLightData& Light() { return mCBuffer.mData.mLight; }
		ID3D11SamplerState* & SamplerState() { return mSampler; }

	private:
		void InitializeCBuffers();

		ID3D11PixelShader* mShader;

		struct CBufferPerFrameData {
			DirectionalLightData mLight;
			DirectX::XMFLOAT3 mCameraPosVS;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11SamplerState* mSampler;
	};
}