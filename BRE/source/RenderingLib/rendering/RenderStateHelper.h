#pragma once

//////////////////////////////////////////////////////////////////////////
//
// Class that has the ability to save/restore render states
// like rasterizer, blend and depth stencil states.
//
//////////////////////////////////////////////////////////////////////////

struct ID3D11BlendState;
struct ID3D11DepthStencilState;
struct ID3D11DeviceContext1;
struct ID3D11RasterizerState;

namespace BRE {
	class RenderStateHelper {
	public:
		// Global instance
		static RenderStateHelper* gInstance;

		RenderStateHelper(ID3D11DeviceContext1& context);
		~RenderStateHelper();
		const RenderStateHelper& operator=(const RenderStateHelper& rhs) = delete;

		void SaveRasterizerState();
		void RestoreRasterizerState() const;

		void SaveBlendState();
		void RestoreBlendState() const;

		void SaveDepthStencilState();
		void RestoreDepthStencilState() const;

		void ResetAll();
		void SaveAll();
		void RestoreAll() const;

	private:
		ID3D11DeviceContext1& mContext;

		ID3D11RasterizerState* mRasterizerState;
		ID3D11BlendState* mBlendState;
		float mBlendFactor[4];
		unsigned int mSampleMask;
		ID3D11DepthStencilState* mDepthStencilState;
		unsigned int mStencilRef;
	};
}
