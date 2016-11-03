#pragma once

#include <chrono>
#include <cstdint>
using namespace std::chrono;

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : Timer (Class)
/// <summary>
/// Timer class, queries performance hardware if available, and 
/// calculates all the various values required for frame rate based
/// vector / value scaling.
/// </summary>
//-----------------------------------------------------------------------------
class Timer
{
public:
	typedef float Real;
	typedef high_resolution_clock clock;
	typedef	duration<Real> rSeconds;

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class
	//-------------------------------------------------------------------------
	Timer();
	virtual ~Timer();

	void						shutdown				();
	//------------------------------------------------------------
	// Public Methods
	//------------------------------------------------------------
	void						tick					();
	void						tick					(Real lockFPS);
	void						enableSmoothing			(bool enabled);
	Real						getTime					(bool queryTimer = true);

	template<typename T>
	static T					measurePeriod			(const clock::time_point& counterBegin, const clock::time_point& counterEnd);
	static clock::time_point	now						();

	clock::time_point			now						(bool queryTimer = true);
	std::uint32_t				getFrameRate			() const;
	void						setSimulationSpeed		(Real speed);
	void						setRateLimit			(Real rateLimit);
	void						incrementFrameCounter	();

	inline void					setFrameCounter			(std::uint32_t counter) { mFrameCounter = counter; }
	inline std::uint32_t		getFrameCounter			() const { return mFrameCounter; }
	inline Real					getDeltaTime			() const { return mSimulationTimeElapsed; }
	inline Real					getSimulationSpeed		() const { return mSimulationSpeed; }

private:
	//------------------------------------------------------------
	// Private Member Variables
	//------------------------------------------------------------
	bool						mUseSmoothing;				// Is time-step smoothing enabled?
	clock::time_point			mCurrentTime;				// Current Performance Counter
	clock::time_point			mStartTime;					// Time at which timer was initialized.
	clock::time_point			mLastTime;					// Performance Counter last frame
	Real						mTimeElapsed;				// Final REPORTED elapsed time since previous frame (may be smoothed)
	Real						mPrevTimeElapsed;			// Previous REPORTED elapsed time (may be smoothed).
	Real						mSimulationTimeElapsed;		// Pre-multiplied mTimeElapsed * mSimulationSpeed.
	Real						mTimeDebt;					// How much have the summed elapsed time's drifted from the real-world clock?
	Real						mCurrentTimeSeconds;

	Real						mFrameTime[11];
	Real						mSimulationSpeed;
	Real						mRateLimit;

	std::uint32_t				mFPSFrameCount;				// Elapsed frames in any given second
	Real						mFPSTimeElapsed;			// How much time has passed during FPS sample

	std::uint32_t				mFrameRate;					// Stores current framerate
	std::uint32_t				mFrameCounter;				// Simple Application counter

};

//---------------------------------------------------------------------------------
// Name : measurePeriod () 
// Desc : Convert the difference between the two performance counter values into
//        a measured time in seconds.
//---------------------------------------------------------------------------------
template<typename T>
inline T Timer::measurePeriod(const clock::time_point& nCounter1, const clock::time_point& nCounter2)
{
	return duration_cast<duration<T>>(nCounter2 - nCounter1).count();
}