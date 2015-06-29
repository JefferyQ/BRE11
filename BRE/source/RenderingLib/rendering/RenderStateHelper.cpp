#include "RenderStateHelper.h"

#include <cstdint>
#include <d3d11_1.h>

namespace BRE {
	RenderStateHelper* RenderStateHelper::gInstance = nullptr;

	RenderStateHelper::RenderStateHelper(ID3D11DeviceContext1& context)
		: mContext(context)
		, mRasterizerState(nullptr)
		, mBlendState(nullptr)
		, mSampleMask(UINT32_MAX)
		, mDepthStencilState(nullptr)
		, mStencilRef(UINT32_MAX)
	{
	}

	RenderStateHelper::~RenderStateHelper() {
		if (mRasterizerState) mRasterizerState->Release();
		if (mBlendState) mBlendState->Release();
		if (mDepthStencilState) mDepthStencilState->Release();
	}

	void RenderStateHelper::SaveRasterizerState() {
		if (mRasterizerState) mRasterizerState->Release();
		mContext.RSGetState(&mRasterizerState);
	}

	void RenderStateHelper::ResetAll() {
		mContext.RSSetState(nullptr);
		mContext.OMSetBlendState(nullptr, nullptr, UINT32_MAX);
		mContext.OMSetDepthStencilState(nullptr, UINT32_MAX);
	}

	void RenderStateHelper::SaveAll() {
		SaveRasterizerState();
		SaveBlendState();
		SaveDepthStencilState();
	}

	void RenderStateHelper::RestoreAll() const {
		RestoreRasterizerState();
		RestoreBlendState();
		RestoreDepthStencilState();
	}

	void RenderStateHelper::RestoreRasterizerState() const {
		mContext.RSSetState(mRasterizerState);
	}

	void RenderStateHelper::SaveBlendState() {
		if (mBlendState) mBlendState->Release();
		mContext.OMGetBlendState(&mBlendState, mBlendFactor, &mSampleMask);
	}

	void RenderStateHelper::RestoreBlendState() const {
		mContext.OMSetBlendState(mBlendState, mBlendFactor, mSampleMask);
	}

	void RenderStateHelper::SaveDepthStencilState() {
		if (mDepthStencilState) mDepthStencilState->Release();
		mContext.OMGetDepthStencilState(&mDepthStencilState, &mStencilRef);
	}

	void RenderStateHelper::RestoreDepthStencilState() const {
		mContext.OMSetDepthStencilState(mDepthStencilState, mStencilRef);
	}
}