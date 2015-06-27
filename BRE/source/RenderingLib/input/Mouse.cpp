#include "Mouse.h"

#include <utils/Assert.h>

namespace BRE {
	Mouse* Mouse::gInstance = nullptr;

	Mouse::Mouse(IDirectInput8& directInput, const HWND windowHandle)
		: mDirectInput(directInput)
	{
		ZeroMemory(&mCurrentState, sizeof(mCurrentState));
		ZeroMemory(&mLastState, sizeof(mLastState));
		ASSERT_HR(mDirectInput.CreateDevice(GUID_SysMouse, &mDevice, nullptr));
		ASSERT_PTR(mDevice);
		ASSERT_HR(mDevice->SetDataFormat(&c_dfDIMouse));
		ASSERT_HR(mDevice->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
		mDevice->Acquire();
	}

	Mouse::~Mouse() {
		mDevice->Unacquire();
		mDevice->Release();
	}

	void Mouse::Update() {
		ASSERT_PTR(mDevice);
		memcpy(&mLastState, &mCurrentState, sizeof(mCurrentState));
		if (FAILED(mDevice->GetDeviceState(sizeof(mCurrentState), &mCurrentState)) && SUCCEEDED(mDevice->Acquire()) && FAILED(mDevice->GetDeviceState(sizeof(mCurrentState), &mCurrentState))) {
			return;
		}
		mX += mCurrentState.lX;
		mY += mCurrentState.lY;
		mWheel += mCurrentState.lZ;
	}
}