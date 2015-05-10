#pragma once

#include <DirectXMath.h>
#include <vector>

#include <rendering/StringDrawer.h>
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
	class dPointLightPixelShaderData;

	class DrawManager {
	public:
		static DrawManager* gInstance;

		DrawManager(ID3D11Device1& device, ID3D11DeviceContext1& context, const unsigned int screenWidth, const unsigned int screenHeight);

		void LoadModels(const char* filepath);

		void DrawAll(ID3D11Device1& device, ID3D11DeviceContext1& context, IDXGISwapChain1& swapChain, ID3D11RenderTargetView& backBufferRTV, ID3D11DepthStencilView& depthStencilView);

		const DirectX::XMFLOAT4X4& ViewProjection() const { return mViewProjection; }

		std::vector<NormalDisplacementDrawer>& NormalDisplacementDrawerVec() { return mDrawers0; }
		std::vector<NormalMappingDrawer>& NormalMappingDrawerVec() { return mDrawers1; }
		std::vector<LightsDrawer::DirLightData>& DirLightDataVec() { return mLightsDrawer.DirLightDataVec(); }
		std::vector<LightsDrawer::PointLightData>& PointLightDataVec() { return mLightsDrawer.PointLightDataVec(); }
		StringDrawer& FrameRateDrawer() { return mFrameRateDrawer; }

	private:
		void InitFullyDeferredResources(const unsigned int screenWidth, const unsigned int screenHeight);
		void InitPostProcessResources(const unsigned int screenWidth, const unsigned int screenHeight);

		// Each time we draw, view projection matrix is calculated
		// and consumed by drawers that need them.
		DirectX::XMFLOAT4X4 mViewProjection;

		// Render target views and shader resources views
		// for fully deferred rendering purposes
		// [0] -> Normal
		// [1] -> Diffuse albedo
		// [2] -> Specular albedo
		// [3] -> Position
		ID3D11RenderTargetView* mGeometryBuffersRTVs[4];
		ID3D11ShaderResourceView* mGeometryBuffersSRVs[4];

		// Render target views and shader resource views of
		// textures used for postprocessing purposes
		ID3D11RenderTargetView* mPostprocess1RTV;
		ID3D11ShaderResourceView* mPostprocess1SRV;
		ID3D11RenderTargetView* mPostprocess2RTV;
		ID3D11ShaderResourceView* mPostprocess2SRV;

		std::vector<NormalDisplacementDrawer> mDrawers0;
		std::vector<NormalMappingDrawer> mDrawers1;
		LightsDrawer mLightsDrawer;
		StringDrawer mFrameRateDrawer;
	};
}