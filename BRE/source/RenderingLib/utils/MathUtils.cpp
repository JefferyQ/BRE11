#include "MathUtils.h"

#include <Windows.h>

#include <rendering/models/Mesh.h>
#include <utils/Assert.h>

using namespace DirectX;

namespace BRE {
	namespace Utils {
		float RandomFloat(const float min, const float max) {
			BRE_ASSERT(max > min);
			const float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			// generate (in your case) a float between 0 and (4.5-.78)
			// then add .78, giving you a float between .78 and 4.5
			const float range = max - min;
			return (random * range) + min;
		}

		void CalculateTangentArray(Mesh& mesh, std::vector<XMFLOAT3>& tangents) {
			const size_t vertexCount = mesh.Vertices().size();
			const size_t triangleCount = mesh.FaceCount();
			const std::vector<XMFLOAT3>& texCoords = mesh.TextureCoordinates();
			tangents.resize(vertexCount);
			XMFLOAT3 *tan1 = new XMFLOAT3[vertexCount * 2];
			XMFLOAT3 *tan2 = tan1 + vertexCount;
			ZeroMemory(tan1, vertexCount * sizeof(XMFLOAT3) * 2);
			size_t baseIndex = 0;
			for (long a = 0; a < triangleCount; ++a) {
				long i1 = mesh.Indices()[baseIndex];
				long i2 = mesh.Indices()[baseIndex + 1];
				long i3 = mesh.Indices()[baseIndex + 2];

				const XMFLOAT3& v1 = mesh.Vertices()[i1];
				const XMFLOAT3& v2 = mesh.Vertices()[i2];
				const XMFLOAT3& v3 = mesh.Vertices()[i3];

				const XMFLOAT3& w1 = texCoords[i1];
				const XMFLOAT3& w2 = texCoords[i2];
				const XMFLOAT3& w3 = texCoords[i3];

				float x1 = v2.x - v1.x;
				float x2 = v3.x - v1.x;
				float y1 = v2.y - v1.y;
				float y2 = v3.y - v1.y;
				float z1 = v2.z - v1.z;
				float z2 = v3.z - v1.z;

				float s1 = w2.x - w1.x;
				float s2 = w3.x - w1.x;
				float t1 = w2.y - w1.y;
				float t2 = w3.y - w1.y;

				float r = 1.0F / (s1 * t2 - s2 * t1);
				XMFLOAT3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
				XMFLOAT3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

				XMStoreFloat3(&tan1[i1], XMVectorAdd(XMLoadFloat3(&tan1[i1]), XMLoadFloat3(&sdir)));
				XMStoreFloat3(&tan1[i2], XMVectorAdd(XMLoadFloat3(&tan1[i2]), XMLoadFloat3(&sdir)));
				XMStoreFloat3(&tan1[i3], XMVectorAdd(XMLoadFloat3(&tan1[i3]), XMLoadFloat3(&sdir)));

				XMStoreFloat3(&tan2[i1], XMVectorAdd(XMLoadFloat3(&tan2[i1]), XMLoadFloat3(&tdir)));
				XMStoreFloat3(&tan2[i2], XMVectorAdd(XMLoadFloat3(&tan2[i2]), XMLoadFloat3(&tdir)));
				XMStoreFloat3(&tan2[i3], XMVectorAdd(XMLoadFloat3(&tan2[i3]), XMLoadFloat3(&tdir)));

				baseIndex += 3;
			}
			for (long a = 0; a < vertexCount; ++a) {
				const XMFLOAT3& n = mesh.Normals()[a];
				const XMFLOAT3& t = tan1[a];
				// Gram-Schmidt orthogonalize
				XMStoreFloat3(&tangents[a], XMVectorSubtract(XMLoadFloat3(&t), XMLoadFloat3(&n)));
				const float dot = n.x * t.x + n.y * t.y + n.z * t.z;
				XMStoreFloat3(&tangents[a], XMVectorScale(XMLoadFloat3(&tangents[a]), dot));
				XMStoreFloat3(&tangents[a], XMVector3Length(XMLoadFloat3(&tangents[a])));
			}
			delete[] tan1;
		}
	}
}