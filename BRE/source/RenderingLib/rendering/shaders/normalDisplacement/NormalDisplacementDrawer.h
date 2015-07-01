#pragma once

#include <DirectXMath.h>

#include <rendering/shaders/normalDisplacement/ds/NormalDisplacementDsData.h>
#include <rendering/shaders/normalDisplacement/hs/NormalDisplacementHsData.h>
#include <rendering/shaders/normalDisplacement/ps/NormalDisplacementPsData.h>
#include <rendering/shaders/normalDisplacement/vs/NormalDisplacementVsData.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11RenderTargetView;

namespace YAML {
	class Node;
}

namespace BRE {
	class NormalDisplacementVsData;

	class NormalDisplacementDrawer {
	public:
		static void Create(const YAML::Node& node, std::vector<NormalDisplacementDrawer>& drawers);
		void Draw(ID3D11Device1& device, ID3D11DeviceContext1& context, ID3D11RenderTargetView* *geometryBuffersRTVs, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj);

	private:
		NormalDisplacementVsData mVsData;
		NormalDisplacementHsData mHsData;
		NormalDisplacementDsData mDsData;
		NormalDisplacementPsData mPsData;

		DirectX::XMFLOAT4X4 mWorld;
	};
}