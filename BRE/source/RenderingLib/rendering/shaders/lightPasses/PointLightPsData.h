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
	class PointLightPixelShaderData {
	public:
		PointLightPixelShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* *geometryBuffersSRVs, ID3D11ShaderResourceView& depthStencilSRV);
		void PostDraw(ID3D11DeviceContext1& context);

		ID3D11SamplerState* &SamplerState() { return mSampler; }
		float& ProjectionA() { return mCBuffer.mData.mProjectionConstants[0]; }
		float& ProjectionB() { return mCBuffer.mData.mProjectionConstants[1]; }

	private:
		void InitializeCBuffers();

		ID3D11PixelShader* mShader;

		struct CBufferPerFrameData {
			float mProjectionConstants[2]; // 0 -> Far clip distance / (Far clip distance / near clip distance)
										   // 1 -> (- Far clip distance * Near clip distance) / (Far clip distance - near clip distance)
		};
		Buffer<CBufferPerFrameData> mCBuffer;

		ID3D11SamplerState* mSampler;
	};
}