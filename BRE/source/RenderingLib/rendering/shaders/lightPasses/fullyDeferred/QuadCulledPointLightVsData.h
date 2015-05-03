#pragma once

#include <DirectXMath.h>

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11VertexShader;

namespace BRE {
	namespace FullyDeferred {
		class QuadCulledPointLightVertexShaderData {
			// Max number of point lights allowed by shader
			static const unsigned int sMaxLights = 512;

		public:
			QuadCulledPointLightVertexShaderData();

			void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
			void PostDraw(ID3D11DeviceContext1& context);
			void Draw(ID3D11DeviceContext1& context);

			DirectX::XMFLOAT4& LightPosAndRadius(const unsigned int index);
			DirectX::XMFLOAT4& LightColor(const unsigned int index);

		private:
			void InitializeCBuffers();

			ID3D11VertexShader* mShader;

			struct CBufferPerFrameData {
				DirectX::XMFLOAT4 mLightPosAndRadius[sMaxLights];
				DirectX::XMFLOAT4 mLightColor[sMaxLights];
			} mCBufferPerFrameData;

			ID3D11Buffer* mCBufferPerFrame;
		};
	}
}