#pragma once

#include <rendering/shaders/Buffer.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;
struct ID3D11HullShader;

namespace BRE {
	class NormalDisplacementHullShaderData {
	public:
		NormalDisplacementHullShaderData();

		void PreDraw(ID3D11Device1& device, ID3D11DeviceContext1& context);
		void PostDraw(ID3D11DeviceContext1& context);

		float& TessellationFactor() { return mCBuffer.mData.mTessellationFactor; }

	private:
		void InitializeCBuffers();

		ID3D11HullShader* mShader;

		struct CBufferPerFrameData {
			float mTessellationFactor;
		};
		Buffer<CBufferPerFrameData> mCBuffer;
	};
}