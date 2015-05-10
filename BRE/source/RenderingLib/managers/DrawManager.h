#pragma once

#include <DirectXMath.h>
#include <vector>

#include <rendering/StringDrawer.h>
#include <rendering/shaders/lightPasses/LightsDrawer.h>
#include <rendering/shaders/lightPasses/fullyDeferred/FullyDeferredLightsDrawer.h>
#include <rendering/shaders/normalMapping/NormalMappingDrawer.h>
#include <rendering/shaders/normalDisplacement/fullyDeferred/FullyDeferredNormalDisplacementDrawer.h>
#include <rendering/shaders/normalMapping/fullyDeferred/FullyDeferredNormalMappingDrawer.h>

struct ID3D11DepthStencilView;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;
struct IDXGISwapChain1;

namespace BRE {
	namespace FullyDeferred {
		class QuadCulledPointLightVertexShaderData;
		class QuadCulledPointLightGeometryShaderData;
		class QuadCulledPointLightPixelShaderData;
	}

	class DrawManager {
	public:
		static DrawManager* gInstance;

		DrawManager(ID3D11Device1& device, ID3D11DeviceContext1& context, const unsigned int screenWidth, const unsigned int screenHeight);

		void LoadModels(const char* filepath);

		void DrawAll(ID3D11Device1& device, ID3D11DeviceContext1& context, IDXGISwapChain1& swapChain, ID3D11RenderTargetView& backBufferRTV, ID3D11DepthStencilView& depthStencilView);

		const DirectX::XMFLOAT4X4& ViewProjection() const { return mViewProjection; }

		std::vector<FullyDeferred::NormalDisplacementDrawer>& FullyDeferredNormalDisplacementDrawerVec() { return mDrawers0; }
		std::vector<FullyDeferred::NormalMappingDrawer>& FullyDeferredNormalMappingDrawerVec() { return mDrawers1; }
		std::vector<FullyDeferred::LightsDrawer::DirLightData>& DirLightDataVec() { return mLightsDrawer.DirLightDataVec(); }
		std::vector<FullyDeferred::LightsDrawer::QuadCulledPointLightData>& GetQuadCulledPointLightDataVec() { return mLightsDrawer.GetQuadCulledPointLightDataVec(); }

		std::vector<NormalMappingDrawer>& GetNormalMappingDrawerVec() { return mNormalMappingDrawer; }
		LightsDrawer& LightPass() { return mLightPass; }

		StringDrawer& FrameRateDrawer() { return mFrameRateDrawer; }

	private:
		void InitFullyDeferredResources(const unsigned int screenWidth, const unsigned int screenHeight);
		void InitPostProcessResources(const unsigned int screenWidth, const unsigned int screenHeight);
		void InitResources(const unsigned int screenWidth, const unsigned int screenHeight);

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

		// Render target views and shader resources views
		// for surface properties
		// [0] -> Normal
		// [1] -> Position
		ID3D11RenderTargetView* mSurfacePropsRTVs[2];
		ID3D11ShaderResourceView* mSurfacePropsSRVs[2];

		ID3D11RenderTargetView* mLightBufferRTV;
		ID3D11ShaderResourceView* mLightBufferSRV;

		// Render target views and shader resource views of
		// textures used for postprocessing purposes
		ID3D11RenderTargetView* mPostprocess1RTV;
		ID3D11ShaderResourceView* mPostprocess1SRV;
		ID3D11RenderTargetView* mPostprocess2RTV;
		ID3D11ShaderResourceView* mPostprocess2SRV;

		std::vector<FullyDeferred::NormalDisplacementDrawer> mDrawers0;
		std::vector<FullyDeferred::NormalMappingDrawer> mDrawers1;
		FullyDeferred::LightsDrawer mLightsDrawer;
		StringDrawer mFrameRateDrawer;

		std::vector<NormalMappingDrawer> mNormalMappingDrawer;
		LightsDrawer mLightPass;
	};
}