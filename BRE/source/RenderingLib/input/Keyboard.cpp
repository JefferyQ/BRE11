#include "Keyboard.h"

#include <utils/Assert.h>

namespace BRE {
	Keyboard* Keyboard::gInstance = nullptr;

	Keyboard::Keyboard(IDirectInput8& directInput, const HWND windowHandle)
		: mDirectInput(directInput)
	{
		ZeroMemory(mCurrentState, sizeof(mCurrentState));
		ZeroMemory(mLastState, sizeof(mLastState));
		ASSERT_HR(mDirectInput.CreateDevice(GUID_SysKeyboard, &mDevice, nullptr));
		ASSERT_PTR(mDevice);
		ASSERT_HR(mDevice->SetDataFormat(&c_dfDIKeyboard));
		ASSERT_HR(mDevice->SetCooperativeLevel(windowHandle, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
		mDevice->Acquire();
	}

	Keyboard::~Keyboard() {
		mDevice->Unacquire();
		mDevice->Release();
	}

	void Keyboard::Update() {
		ASSERT_PTR(mDevice);
		memcpy(mLastState, mCurrentState, sizeof(mCurrentState));
		if (FAILED(mDevice->GetDeviceState(sizeof(mCurrentState), reinterpret_cast<LPVOID>(mCurrentState))) && SUCCEEDED(mDevice->Acquire())) {
			mDevice->GetDeviceState(sizeof(mCurrentState), reinterpret_cast<LPVOID>(mCurrentState));
		}
	}
}