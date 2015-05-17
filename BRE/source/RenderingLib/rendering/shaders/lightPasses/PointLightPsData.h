#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/LightsData.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11PixelShader;
struct ID3D11SamplerState;
struct ID3D11ShaderResourceView;

namespace BRE {
	class PointLightPixelShaderData {
	public:
		PointLightPixelShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[4]);
		void PostDraw(ID3D11DeviceContext1& context);

		DirectX::XMFLOAT3& CameraPosVS() { return mCBufferPerFrameData.mCameraPosVS; }
		ID3D11SamplerState* &SamplerState() { return mSampler; }
		DirectX::XMFLOAT4X4& ViewMatrix() { return mCBufferPerFrameData.mView; }

	private:
		void InitializeCBuffers();

		ID3D11PixelShader* mShader;

		struct CBufferPerFrameData {
			DirectX::XMFLOAT4X4 mView;
			DirectX::XMFLOAT3 mCameraPosVS;
		} mCBufferPerFrameData;

		ID3D11Buffer* mCBufferPerFrame;

		ID3D11SamplerState* mSampler;
	};
}