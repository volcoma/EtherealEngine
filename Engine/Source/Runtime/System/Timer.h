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
	//-----------------------------------------------------------------------------
	//  Name : Timer () (Constructor)
	/// <summary>
	/// Timer Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	Timer();

	//-----------------------------------------------------------------------------
	//  Name : Timer () (Destructor)
	/// <summary>
	/// Timer Class Destructor
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~Timer();

	//------------------------------------------------------------
	// Public Methods
	//------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : tick () 
	/// <summary>
	/// Function which signals that frame has advanced
	/// Note : You can specify a number of frames per second to lock the frame rate
	/// to. This will simply soak up the remaining time to hit that target.
	/// </summary>
	//-----------------------------------------------------------------------------
	void tick();
	void tick(Real lockFPS);

	//-----------------------------------------------------------------------------
	//  Name : enableSmoothing () 
	/// <summary>
	/// Enable elapsed time smoothing.
	/// </summary>
	//-----------------------------------------------------------------------------
	void enableSmoothing(bool enabled);

	//-----------------------------------------------------------------------------
	//  Name : getTime ()
	/// <summary>
	/// Returns the current floating point time in seconds. This can either be
	/// the time at the last call to tick, or can optionally be re-sampled 
	/// precisely.
	/// </summary>
	//-----------------------------------------------------------------------------
	Real getTime(bool queryTimer = true);

	//-----------------------------------------------------------------------------
	//  Name : measurePeriod ()
	/// <summary>
	/// Convert the difference between the two performance counter values into
	/// a measured time in seconds.
	/// </summary>
	//-----------------------------------------------------------------------------
	template<typename T>
	static T measurePeriod(const clock::time_point& counterBegin, const clock::time_point& counterEnd);

	//-----------------------------------------------------------------------------
	//  Name : now ()
	/// <summary>
	/// Returns the value read from the performance counter hardware.
	/// </summary>
	//-----------------------------------------------------------------------------
	static clock::time_point now();

	//-----------------------------------------------------------------------------
	//  Name : now ()
	/// <summary>
	/// Returns the value read from the performance counter hardware.
	/// This can either be the value at the last call to tick, or can optionally
	/// be re-sampled precisely.
	/// </summary>
	//-----------------------------------------------------------------------------
	clock::time_point now(bool queryTimer = true);

	//-----------------------------------------------------------------------------
	//  Name : getFrameRate () 
	/// <summary>
	/// Returns the frame rate, sampled over the last second or so.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t getFrameRate() const;

	//-----------------------------------------------------------------------------
	//  Name : setSimulationSpeed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setSimulationSpeed(Real speed);

	//-----------------------------------------------------------------------------
	//  Name : setRateLimit () 
	/// <summary>
	/// Set the permanent frame rate limit to use (if any) when none is supplied
	/// directly to the 'tick()' method. Set to a value <= 0 to disable.
	/// </summary>
	//-----------------------------------------------------------------------------
	void setRateLimit(Real rateLimit);

	//-----------------------------------------------------------------------------
	//  Name : incrementFrameCounter ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void incrementFrameCounter();

	//-----------------------------------------------------------------------------
	//  Name : setFrameCounter ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setFrameCounter(std::uint32_t counter) { mFrameCounter = counter; }

	//-----------------------------------------------------------------------------
	//  Name : getFrameCounter ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::uint32_t getFrameCounter() const { return mFrameCounter; }

	//-----------------------------------------------------------------------------
	//  Name : getDeltaTime ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Real getDeltaTime() const { return mSimulationTimeElapsed; }

	//-----------------------------------------------------------------------------
	//  Name : getSimulationSpeed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Real getSimulationSpeed() const { return mSimulationSpeed; }

private:
	//------------------------------------------------------------
	// Private Member Variables
	//------------------------------------------------------------
	/// Is time-step smoothing enabled?
	bool mUseSmoothing;
	/// Current Performance Counter
	clock::time_point mCurrentTime;
	/// Time at which timer was initialized.
	clock::time_point mStartTime;
	/// Performance Counter last frame
	clock::time_point mLastTime;
	/// Final REPORTED elapsed time since previous frame (may be smoothed)
	Real mTimeElapsed;
	/// Previous REPORTED elapsed time (may be smoothed).
	Real mPrevTimeElapsed;
	/// Pre-multiplied mTimeElapsed * mSimulationSpeed.
	Real mSimulationTimeElapsed;
	/// How much have the summed elapsed time's drifted from the real-world clock?
	Real mTimeDebt;
	/// Current time.
	Real mCurrentTimeSeconds;
	///
	Real mFrameTime[11];
	///
	Real mSimulationSpeed;
	///
	Real mRateLimit;
	/// Elapsed frames in any given second
	std::uint32_t mFPSFrameCount;
	/// How much time has passed during FPS sample
	Real mFPSTimeElapsed;
	/// Stores current frame rate
	std::uint32_t mFrameRate;
	/// Simple Application counter
	std::uint32_t mFrameCounter;
};

template<typename T>
inline T Timer::measurePeriod(const clock::time_point& nCounter1, const clock::time_point& nCounter2)
{
	return duration_cast<duration<T>>(nCounter2 - nCounter1).count();
}