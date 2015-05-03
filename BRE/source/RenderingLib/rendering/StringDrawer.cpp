#include "StringDrawer.h"

#include <rendering/RenderStateHelper.h>

namespace BRE {
	StringDrawer::StringDrawer(ID3D11Device1& device, ID3D11DeviceContext1& context)
		: mSpriteBatch(&context)
		, mSpriteFont(&device, L"content\\fonts\\Arial_14_Regular.spritefont")
		, mTextPosition(0.0f, 0.0f)
	{
	}

	void StringDrawer::Draw() {
		mSpriteBatch.Begin();
		mSpriteFont.DrawString(&mSpriteBatch, mString.c_str(), mTextPosition);
		mSpriteBatch.End();
	}
}