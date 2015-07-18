#pragma once
#pragma once

#include <DirectXMath.h>
#include <vector>

namespace BRE {
	class Mesh;
	namespace Utils {
		float RandomFloat(const float min, const float max);
		void CalculateTangentArray(Mesh& mesh, std::vector<DirectX::XMFLOAT3>& tangents);
	};
}