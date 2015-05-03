#pragma once
#include <DirectXMath.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>

struct ID3D11Device1;
struct ID3D11DeviceContext1;

namespace BRE {
	class StringDrawer {
	public:
		StringDrawer(ID3D11Device1& device, ID3D11DeviceContext1& context);
		DirectX::XMFLOAT2& TextPosition() { return mTextPosition; }
		std::wstring& Text() { return mString; }

		void Draw();

	private:
		DirectX::SpriteBatch mSpriteBatch;
		DirectX::SpriteFont mSpriteFont;
		DirectX::XMFLOAT2 mTextPosition;
		std::wstring mString;
	};
}