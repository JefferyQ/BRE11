#pragma once

#include <windows.h>

namespace BRE {
	class Clock {
	public:
		void Reset();
		double Frequency() const;
		void UpdateTime();

		const LARGE_INTEGER& StartTime() const { return mStartTime; }
		const LARGE_INTEGER& CurrentTime() const { return mCurrentTime; }
		const LARGE_INTEGER& LastTime() const { return mLastTime; }
		float TotalTime() const { return static_cast<float>(mTotalTime); }
		float ElapsedTime() const { return static_cast<float>(mElapsedTime); }
		void GetTime(LARGE_INTEGER& time) const { QueryPerformanceCounter(&time); }
		unsigned int FrameRate() { return mFrameRate; }

	private:
		LARGE_INTEGER mStartTime;
		LARGE_INTEGER mCurrentTime;
		LARGE_INTEGER mLastTime;
		double mFrequency;
		double mTotalTime;
		double mElapsedTime;
		unsigned int mFrameCount;
		unsigned int mFrameRate;
		double mLastTotalElapsedTime;
	};
}
