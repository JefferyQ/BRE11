#include "PostProcessDrawer.h"

#include <d3d11_1.h>
#include <DirectXMath.h>

#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>

#include <rendering/GlobalResources.h>

using namespace DirectX;

namespace BRE {
	PostProcessDrawer::PostProcessDrawer(ID3D11Device1& device)
		: mFiltersVsData(device)
	{

	}

	void PostProcessDrawer::Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* postProcessSRV) {
		mToneMappingPsData.TextureToFilterSRV() = postProcessSRV;
		mToneMappingPsData.SamplerState() = GlobalResources::gInstance->MinMagMipPointSampler();

		mFiltersVsData.PreDraw(device, context);
		mToneMappingPsData.PreDraw(device, context);
		mFiltersVsData.DrawIndexed(context);
		mFiltersVsData.PostDraw(context);
		mToneMappingPsData.PostDraw(context);		
	}
}