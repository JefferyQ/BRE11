#pragma once

#include <DirectXMath.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11GeometryShader;

namespace BRE {
	namespace FullyDeferred {
		class QuadCulledPointLightGeometryShaderData {
		public:
			QuadCulledPointLightGeometryShaderData();

			void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
			void PostDraw(ID3D11DeviceContext1& context);
			DirectX::XMFLOAT4X4& ViewMatrix() { return mCBufferPerFrameData.mView; }
			DirectX::XMFLOAT4X4& ProjectionMatrix() { return mCBufferPerFrameData.mProj; }

		private:
			void InitializeCBuffers();

			ID3D11GeometryShader* mShader;

			struct CBufferPerFrameData {
				DirectX::XMFLOAT4X4 mView;
				DirectX::XMFLOAT4X4 mProj;
			} mCBufferPerFrameData;

			ID3D11Buffer* mCBufferPerFrame;
		};
	}
}