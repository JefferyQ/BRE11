#pragma once

#include <DirectXMath.h>
#include <vector>

#include <rendering/StringDrawer.h>
#include <rendering/shaders/basic/BasicDrawer.h>
#include <rendering/shaders/filters/PostProcessDrawer.h>
#include <rendering/shaders/lightPasses/LightsDrawer.h>
#include <rendering/shaders/normalDisplacement/NormalDisplacementDrawer.h>
#include <rendering/shaders/normalMapping/NormalMappingDrawer.h>

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;
struct IDXGISwapChain1;

namespace BRE {
	class PointLightVertexShaderData;
	class PointLightGeometryShaderData;
	class PointLightPixelShaderData;

	class DrawManager {
	public:
		static DrawManager* gInstance;

		DrawManager(ID3D11Device1& device, ID3D11DeviceContext1& context, const unsigned int screenWidth, const unsigned int screenHeight);

		void LoadModels(const char* filepath);

		void DrawAll(ID3D11Device1& device, ID3D11DeviceContext1& context, IDXGISwapChain1& swapChain, ID3D11RenderTargetView& backBufferRTV, ID3D11DepthStencilView& depthStencilView, ID3D11ShaderResourceView& depthStencilSRV);

		std::vector<LightsDrawer::DirLightData>& DirLightDataVec() { return mLightsDrawer.DirLightDataVec(); }
		std::vector<LightsDrawer::PointLightData>& PointLightDataVec() { return mLightsDrawer.PointLightDataVec(); }
		StringDrawer& FrameRateDrawer() { return mFrameRateDrawer; }

	private:
		void InitPostProcessResources(const unsigned int screenWidth, const unsigned int screenHeight);
		void InitGBuffers(const unsigned int screenWidth, const unsigned int screenHeight);

		// Render target views and shader resources views
		// for fully deferred rendering purposes
		// [0] -> Normal
		// [1] -> Base Color
		// [2] -> Smoothness_MetalMask_Curvature
		ID3D11RenderTargetView* mGBuffersRTVs[3];
		ID3D11ShaderResourceView* mGBuffersSRVs[3];

		// Render target views and shader resource views of
		// textures used for postprocessing purposes
		ID3D11RenderTargetView* mPostprocess1RTV;
		ID3D11ShaderResourceView* mPostprocess1SRV;
		ID3D11RenderTargetView* mPostprocess2RTV;
		ID3D11ShaderResourceView* mPostprocess2SRV;

		std::vector<NormalDisplacementDrawer> mNormalDisplacementDrawers;
		std::vector<NormalMappingDrawer> mNormalMappingDrawers;
		std::vector<BasicDrawer> mBasicDrawers;
		LightsDrawer mLightsDrawer;
		PostProcessDrawer mPostProcessDrawer;
		StringDrawer mFrameRateDrawer;		
	};
}