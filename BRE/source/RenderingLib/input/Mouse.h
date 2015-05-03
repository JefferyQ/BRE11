#pragma once

#include <dinput.h>

namespace BRE {
	class Mouse {
	public:
		// Global Instance
		static Mouse* gInstance;

		const Mouse& operator=(const Mouse& rhs) = delete;

		enum MouseButtons {
			MouseButtonsLeft = 0,
			MouseButtonsRight = 1,
			MouseButtonsMiddle = 2,
			MouseButtonsX1 = 3
		};

		Mouse(IDirectInput8& directInput, const HWND windowHandle);
		~Mouse();

		void Update();

		const DIMOUSESTATE& CurrentState() { return mCurrentState; }
		const DIMOUSESTATE& LastState() { return mLastState; }
		long X() const { return mX; }
		long Y() const { return mY; }
		long Wheel() const { return mWheel; }
		bool IsButtonUp(const MouseButtons button) const { return ((mCurrentState.rgbButtons[button] & 0x80) == 0); }
		bool IsButtonDown(const MouseButtons button) const { return ((mCurrentState.rgbButtons[button] & 0x80) != 0); }
		bool WasButtonUp(const MouseButtons button) const { return ((mLastState.rgbButtons[button] & 0x80) == 0); }
		bool WasButtonDown(const MouseButtons button) const { return ((mLastState.rgbButtons[button] & 0x80) != 0); }
		bool WasButtonPressedThisFrame(const MouseButtons button) const { return (IsButtonDown(button) && WasButtonUp(button)); }
		bool WasButtonReleasedThisFrame(const MouseButtons button) const { return (IsButtonUp(button) && WasButtonDown(button)); }
		bool IsButtonHeldDown(const MouseButtons button) const { return (IsButtonDown(button) && WasButtonDown(button)); }

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