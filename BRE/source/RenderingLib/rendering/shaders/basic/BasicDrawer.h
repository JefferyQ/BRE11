#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/basic/ps/BasicPsData.h>
#include <rendering/shaders/basic/vs/BasicVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace YAML {
	class Node;
}

namespace BRE {
	class BasicDrawer {
	public:
		static void Create(const YAML::Node& node, std::vector<BasicDrawer>& drawers);
		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

	private:
		BasicVertexShaderData mVertexShaderData;
		BasicPixelShaderData mPixelShaderData;
		DirectX::XMFLOAT4X4 mWorld;
	};
}