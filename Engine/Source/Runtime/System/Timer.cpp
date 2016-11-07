#include "Timer.h"
//-----------------------------------------------------------------------------
// Local Function Definitions
//-----------------------------------------------------------------------------
namespace
{
	int FrameTimeSort(const void * pKey1, const void * pKey2)
	{
		if (*((Timer::Real*)pKey1) == *((Timer::Real*)pKey2))
			return 0;
		return (*((Timer::Real*)pKey1) < *((Timer::Real*)pKey2)) ? -1 : 1;
	}
};

Timer::Timer()
{
	// Query performance hardware and setup time scaling values
	mLastTime = clock::now();
	mStartTime = mLastTime;
	mCurrentTime = mLastTime;

	// Calculate elapsed time in seconds
	mCurrentTimeSeconds = duration_cast<rSeconds>(mCurrentTime - mStartTime).count();
	// Clear any needed values
	mUseSmoothing = false;
	mTimeElapsed = Real(0.0);
	mPrevTimeElapsed = Real(0.0);
	mSimulationSpeed = Real(1.0);
	mSimulationTimeElapsed = Real(0.0);
	mTimeDebt = Real(0.0);
	mRateLimit = Real(0.0);
	mFrameRate = 0;
	mFPSFrameCount = 0;
	mFPSTimeElapsed = Real(0.0);
	mFrameCounter = 0;
	memset(mFrameTime, 0, sizeof(mFrameTime));
}

Timer::~Timer()
{
}

void Timer::tick()
{
	tick(Real(0.0));
}
void Timer::tick(Real fLockFPS)
{
	mCurrentTime = clock::now();
	// Calculate elapsed time in seconds
	Real fTimeElapsed = duration_cast<rSeconds>(mCurrentTime - mLastTime).count();

	// Smoothly ramp up frame rate to prevent jittering
//    if ( fLockFPS == 0.0 && mTimeElapsed > 0 ) fLockFPS = (1.0 / mTimeElapsed) + 20.0;

	// Any permanent rate limit applied?
	if (fLockFPS <= Real(0.0))
		fLockFPS = mRateLimit;

	// Should we lock the frame rate ?
	if (fLockFPS > Real(0.0))
	{
		while (fTimeElapsed < (Real(1.0) / fLockFPS))
		{
			mCurrentTime = clock::now();
			// Calculate elapsed time in seconds
			fTimeElapsed = duration_cast<rSeconds>(mCurrentTime - mLastTime).count();
		} // End While

	} // End If

	// Save current frame time
	mLastTime = mCurrentTime;
	// Use the exact frame time in all samples for at least the first 11 frames.
	// Otherwise, start wrapping the elapsed time sample buffer.
	if (mFrameCounter <= 11)
	{
		for (int i = 0; i < 11; ++i)
			mFrameTime[i] = fTimeElapsed;
		mPrevTimeElapsed = fTimeElapsed;

	} // End if <= 11
	else
	{
		//for ( Int i = 10; i > 0; --i )
			//mFrameTime[i] = mFrameTime[i-1];
		memmove(&mFrameTime[1], mFrameTime, 10 * sizeof(Real));
		mFrameTime[0] = fTimeElapsed;

	} // End if > 11

	// Calculate Frame Rate
	mFPSFrameCount++;
	mFPSTimeElapsed += fTimeElapsed;
	if (mFPSTimeElapsed > Real(1.0))
	{
		mFrameRate = mFPSFrameCount;
		mFPSFrameCount = 0;
		mFPSTimeElapsed = Real(0.0);

	} // End If Second Elapsed

	// If smoothing is enabled, calculate the mean of all 11 samples in the 
	// frame time buffer (minus the two highest and two lowest outliers). 
	// Otherwise, just use the measured time as-is.
	if (mUseSmoothing)
	{
		// Compute the mean of the frame time, discarding the two highest
		// and two lowest outliers (we use a quicksort to achieve this).
		mTimeElapsed = Real(0.0);
		Real Samples[11];
		memcpy(Samples, mFrameTime, sizeof(mFrameTime));
		qsort(Samples, 11, sizeof(Real), FrameTimeSort);
		for (int i = 2; i < 9; ++i)
			mTimeElapsed += Samples[i];
		mTimeElapsed /= Real(7.0);

		// Smooth time step from previous frame.
		mTimeElapsed = (Real(0.2) * mTimeElapsed) + (Real(0.8) * mPrevTimeElapsed);

		// Keep track of the time 'debt'; the difference between the sum of all
		// smoothed time deltas (in total, over time) and the real world clock.
		mTimeDebt += mTimeElapsed - fTimeElapsed;

	} // End if smoothing
	else
	{
		mTimeElapsed = fTimeElapsed;

	} // End if !smoothing

	// Record the final (reported) elapsed time for the previous frame.
	mPrevTimeElapsed = mTimeElapsed;

	// Store current time and final simulation elapsed time in seconds
	// to save the multiply in each call to the various accessors.
	mCurrentTimeSeconds = duration_cast<rSeconds>(mCurrentTime - mStartTime).count();

	mSimulationTimeElapsed = mTimeElapsed * mSimulationSpeed;
}

Timer::Real Timer::getTime(bool bQueryPerfCounter)
{
	if (bQueryPerfCounter)
	{
		mCurrentTimeSeconds = duration_cast<rSeconds>(clock::now() - mStartTime).count();
	} // End if re-query.

	return mCurrentTimeSeconds;
}

Timer::clock::time_point Timer::now(bool bQuery)
{
	if (bQuery)
	{
		mCurrentTime = clock::now();
	} // End if re-query.

	return mCurrentTime;
}

Timer::clock::time_point Timer::now()
{
	return clock::now();
}

std::uint32_t Timer::getFrameRate() const
{
	return mFrameRate;
}

void Timer::setRateLimit(Real fRateLimit)
{
	mRateLimit = fRateLimit;
}

void Timer::enableSmoothing(bool bEnabled)
{
	mUseSmoothing = bEnabled;
}

void Timer::setSimulationSpeed(Real speed)
{
	mSimulationSpeed = speed;
}

void Timer::incrementFrameCounter()
{
	++mFrameCounter;
}
