#pragma once

#include <vector>

#include <rendering/shaders/filters/FiltersVsData.h>
#include <rendering/shaders/filters/toneMapping/ToneMappingPsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11ShaderResourceView;

namespace BRE {
	class PostProcessDrawer {
	public:
		PostProcessDrawer(ID3D11Device1& device);

		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11ShaderResourceView* postProcessSRV);

	private:
		FiltersVertexShaderData mFiltersVsData;
		ToneMappingPixelShaderData mToneMappingPsData;
	};
}