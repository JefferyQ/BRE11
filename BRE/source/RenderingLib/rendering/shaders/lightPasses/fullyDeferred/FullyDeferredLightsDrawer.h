#pragma once

#include <vector>

#include <rendering/shaders/FullscreenVsData.h>
#include <rendering/shaders/lightPasses/fullyDeferred/FullyDeferredDirLightPsData.h>
#include <rendering/shaders/lightPasses/fullyDeferred/QuadCulledPointLightGsData.h>
#include <rendering/shaders/lightPasses/fullyDeferred/QuadCulledPointLightPsData.h>
#include <rendering/shaders/lightPasses/fullyDeferred/QuadCulledPointLightVsData.h>

struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11ShaderResourceView;

namespace BRE {
	namespace FullyDeferred {
		class LightsDrawer {
		public:
			struct QuadCulledPointLightData {
				QuadCulledPointLightVertexShaderData mQuadCulledPointLightVsData;
				QuadCulledPointLightGeometryShaderData mQuadCulledPointLightGsData;
				QuadCulledPointLightPixelShaderData mQuadCulledPointLightPsData;
			};

			struct DirLightData {
				FullscreenVertexShaderData mVertexShaderData;
				DirLightPixelShaderData mPixelShaderData;
			};

			LightsDrawer() { InitStates(); }

			std::vector<DirLightData>& DirLightDataVec() { return mDirLightDataVec; }
			std::vector<QuadCulledPointLightData>& GetQuadCulledPointLightDataVec() { return mQuadCulledPointLightDataVec; }

			void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[4]);

		private:
			void InitStates();

			ID3D11BlendState* mDefaultBS;
			ID3D11DepthStencilState* mDisableDepthTestDSS;

			std::vector<DirLightData> mDirLightDataVec;
			std::vector<QuadCulledPointLightData> mQuadCulledPointLightDataVec;
		};
	}
}