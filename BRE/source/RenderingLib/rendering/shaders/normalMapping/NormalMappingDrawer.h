#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/normalMapping/ps/NormalMappingPsData.h>
#include <rendering/shaders/normalMapping/vs/NormalMappingVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace YAML {
	class Node;
}

namespace BRE {
	class NormalMappingDrawer {
	public:
		static void Create(const YAML::Node& node, std::vector<NormalMappingDrawer>& drawers);
		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

	private:
		NormalMappingVertexShaderData mVertexShaderData;
		NormalMappingPixelShaderData mPixelShaderData;

		DirectX::XMFLOAT4X4 mWorld;
	};
}