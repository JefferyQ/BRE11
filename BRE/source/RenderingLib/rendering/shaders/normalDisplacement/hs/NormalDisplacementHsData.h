#pragma once

struct ID3D11Buffer;
struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11HullShader;

namespace BRE {
	class NormalDisplacementHsData {
	public:
		NormalDisplacementHsData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		float* TessellationFactors() { return mCBufferPerFrameData.mTessellationFactors; }

	private:
		void InitializeCBuffers();

		ID3D11HullShader* mShader;

		struct CBufferPerFrameData {
			// 3 Tessellation edge & 1 inside factors
			float mTessellationFactors[4];
		} mCBufferPerFrameData;

		ID3D11Buffer* mCBufferPerFrame;
	};
}