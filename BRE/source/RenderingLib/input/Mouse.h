#pragma once

#include <dinput.h>

namespace BRE {
	class Mouse {
	public:
		static Mouse* gInstance;

		const Mouse& operator=(const Mouse& rhs) = delete;

		enum MouseButton {
			MouseButtonsLeft = 0,
			MouseButtonsRight,
			MouseButtonsMiddle,
			MouseButtonsX1
		};

		Mouse(IDirectInput8& directInput, const HWND windowHandle);
		~Mouse();

		void Update();

		const DIMOUSESTATE& CurrentState() { return mCurrentState; }
		const DIMOUSESTATE& LastState() { return mLastState; }
		long X() const { return mX; }
		long Y() const { return mY; }
		long Wheel() const { return mWheel; }
		bool IsButtonUp(const MouseButton b) const { return (mCurrentState.rgbButtons[b] & 0x80) == 0; }
		bool IsButtonDown(const MouseButton b) const { return (mCurrentState.rgbButtons[b] & 0x80) != 0; }
		bool WasButtonUp(const MouseButton b) const { return (mLastState.rgbButtons[b] & 0x80) == 0; }
		bool WasButtonDown(const MouseButton b) const { return (mLastState.rgbButtons[b] & 0x80) != 0; }
		bool WasButtonPressedThisFrame(const MouseButton b) const { return IsButtonDown(b) && WasButtonUp(b); }
		bool WasButtonReleasedThisFrame(const MouseButton b) const { return IsButtonUp(b) && WasButtonDown(b); }
		bool IsButtonHeldDown(const MouseButton b) const { return IsButtonDown(b) && WasButtonDown(b); }

	private:
		IDirectInput8& mDirectInput;
		LPDIRECTINPUTDEVICE8 mDevice;
		DIMOUSESTATE mCurrentState;
		DIMOUSESTATE mLastState;
		long mX;
		long mY;
		long mWheel;
	};
}