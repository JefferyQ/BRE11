#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/Buffer.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11GeometryShader;

namespace BRE {
	class PointLightGeometryShaderData {
	public:
		PointLightGeometryShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);
		DirectX::XMFLOAT4X4& ProjectionMatrix() { return mCBuffer.mData.mProj; }

	private:
		void InitializeCBuffers();

		ID3D11GeometryShader* mShader;

		struct CBufferPerFrameData {
			DirectX::XMFLOAT4X4 mProj;
		};
		Buffer<CBufferPerFrameData> mCBuffer;

	};
}