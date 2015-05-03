#include "RenderStateHelper.h"

#include <cstdint>
#include <d3d11_1.h>

#include <utils/Memory.h>

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
		RELEASE_OBJECT(mRasterizerState);
		RELEASE_OBJECT(mBlendState);
		RELEASE_OBJECT(mDepthStencilState);
	}

	void RenderStateHelper::SaveRasterizerState() {
		RELEASE_OBJECT(mRasterizerState);
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
		RELEASE_OBJECT(mBlendState);
		mContext.OMGetBlendState(&mBlendState, mBlendFactor, &mSampleMask);
	}

	void RenderStateHelper::RestoreBlendState() const {
		mContext.OMSetBlendState(mBlendState, mBlendFactor, mSampleMask);
	}

	void RenderStateHelper::SaveDepthStencilState() {
		RELEASE_OBJECT(mDepthStencilState);
		mContext.OMGetDepthStencilState(&mDepthStencilState, &mStencilRef);
	}

	void RenderStateHelper::RestoreDepthStencilState() const {
		mContext.OMSetDepthStencilState(mDepthStencilState, mStencilRef);
	}
}