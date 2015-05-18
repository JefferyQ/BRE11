#include "GlobalResources.h"

#include <d3d11_1.h>

#include <managers/ShaderResourcesManager.h>
#include <utils/Assert.h>

namespace BRE {
	GlobalResources* GlobalResources::gInstance = nullptr;

	GlobalResources::GlobalResources() {
		// Create D3D11_FILTER_MIN_MAG_MIP_POINT sampler state
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MipLODBias = 0;
		desc.MaxAnisotropy = 0;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.BorderColor[0] = 0.0f;
		desc.BorderColor[1] = 0.0f;
		desc.BorderColor[2] = 0.0f;
		desc.BorderColor[3] = 0.0f;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = 0.0f;
		ShaderResourcesManager::gInstance->AddSamplerState("D3D11_FILTER_MIN_MAG_MIP_POINT_sampler_state", desc, &mMinMagMipPointSS);
		ASSERT_PTR(mMinMagMipPointSS);
	}
}