#pragma once

#include <vector>

#include <rendering/shaders/FullscreenVsData.h>
#include <rendering/shaders/lightPasses/DirLightPsData.h>
#include <rendering/shaders/lightPasses/PointLightGsData.h>
#include <rendering/shaders/lightPasses/PointLightPsData.h>
#include <rendering/shaders/lightPasses/PointLightVsData.h>

struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11ShaderResourceView;

namespace BRE {
	class LightsDrawer {
	public:
		struct PointLightData {
			PointLightVertexShaderData mPointLightVsData;
			PointLightGeometryShaderData mPointLightGsData;
			PointLightPixelShaderData mPointLightPsData;
		};

		struct DirLightData {
			FullscreenVertexShaderData mVertexShaderData;
			DirLightPixelShaderData mPixelShaderData;
		};

		LightsDrawer() { InitStates(); }

		std::vector<DirLightData>& DirLightDataVec() { return mDirLightDataVec; }
		std::vector<PointLightData>& PointLightDataVec() { return mPointLightDataVec; }

		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* geometryBuffersSRVs[5]);

	private:
		void InitStates();

		ID3D11BlendState* mDefaultBS;
		ID3D11DepthStencilState* mDisableDepthTestDSS;

		std::vector<DirLightData> mDirLightDataVec;
		std::vector<PointLightData> mPointLightDataVec;
	};
}