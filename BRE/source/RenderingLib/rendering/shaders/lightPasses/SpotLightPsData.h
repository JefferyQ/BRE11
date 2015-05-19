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
	class SpotLightPixelShaderData {
	public:
		SpotLightPixelShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[4]);
		void PostDraw(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT3& CameraPosW() { return mCBuffer.mData.mCameraPosW; }
		SpotLightData& Light() { return mCBuffer.mData.mLight; }
		ID3D11SamplerState* &SamplerState() { return mSampler; }

	private:
		void InitializeCBuffers();

		ID3D11PixelShader* mShader;

		struct CBufferPerFrameData {
			SpotLightData mLight;
			DirectX::XMFLOAT3 mCameraPosW;
			float mPad;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11SamplerState* mSampler;
	};
}