#pragma once

#include <dinput.h>

namespace BRE {
	class Keyboard {
	public:
		// Global instance
		static Keyboard* gInstance;

		const Keyboard& operator=(const Keyboard& rhs) = delete;

		Keyboard(IDirectInput8& directInput, const HWND windowHandle);
		~Keyboard();

		void Update();

		const byte* const CurrentState() const { return mCurrentState; }
		const byte* const LastState() const { return mLastState; }
		bool IsKeyUp(const byte key) const { return ((mCurrentState[key] & 0x80) == 0); }
		bool IsKeyDown(const byte key) const { return ((mCurrentState[key] & 0x80) != 0); }
		bool WasKeyUp(const byte key) const { return ((mLastState[key] & 0x80) == 0); }
		bool WasKeyDown(const byte key) const { return ((mLastState[key] & 0x80) != 0); }
		bool WasKeyPressedThisFrame(const byte key) const { return (IsKeyDown(key) && WasKeyUp(key)); }
		bool WasKeyReleasedThisFrame(const byte key) const { return (IsKeyUp(key) && WasKeyDown(key)); }
		bool IsKeyHeldDown(const byte key) const { return (IsKeyDown(key) && WasKeyDown(key)); }

	private:
		IDirectInput8& mDirectInput;
		LPDIRECTINPUTDEVICE8 mDevice;
		byte mCurrentState[256];
		byte mLastState[256];
	};
}