/*
This source file is part of KBEngine
For the latest info, see http://www.kbengine.org/

Copyright (c) 2008-2016 KBEngine.

KBEngine is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

KBEngine is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.
 
You should have received a copy of the GNU Lesser General Public License
along with KBEngine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KBENGINE_PROFILE_H
#define KBENGINE_PROFILE_H

#include "debug_helper.h"
#include "common/common.h"
#include "common/timer.h"
#include "common/timestamp.h"

namespace KBEngine
{

#if ENABLE_WATCHERS

class ProfileVal;

class ProfileGroup
{
public:
	ProfileGroup(std::string name = "default");
	~ProfileGroup();

	typedef std::vector<ProfileVal*> PROFILEVALS;
	typedef PROFILEVALS::iterator iterator;

	static ProfileGroup & defaultGroup();

	PROFILEVALS & stack() { return stack_; }
	void add(ProfileVal * pVal);

	iterator begin() { return profiles_.begin(); }
	iterator end() { return profiles_.end(); }

	ProfileVal * pRunningTime() { return profiles_[0]; }
	const ProfileVal * pRunningTime() const { return profiles_[0]; }
	TimeStamp runningTime() const;

	INLINE const char* name() const;

	bool initializeWatcher();

	static void finalise(void);

	INLINE const ProfileGroup::PROFILEVALS& profiles() const;
private:
	PROFILEVALS profiles_;

	PROFILEVALS stack_;

	std::string name_;
};

class ProfileVal
{
public:
	ProfileVal(std::string name, ProfileGroup * pGroup = NULL);
	~ProfileVal();

	bool initializeWatcher();

	void start()
	{
		if(!initWatcher_ && count_ > 10)
			initializeWatcher();

		TimeStamp now = timestamp();

		// Record how many times
		if (inProgress_++ == 0)
			lastTime_ = now;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack();

		// If objects in the stack itself is from the previous Profile of the Val function to call
		// We can get the last function to function for a period of time
		// Then add it to the sum int time 
		if (!stack.empty()){
			ProfileVal & profile = *stack.back();
			profile.lastIntTime_ = now - profile.lastIntTime_;
			profile.sumIntTime_ += profile.lastIntTime_;
		}

		// His pushes
		stack.push_back(this);

		// Record start time
		lastIntTime_ = now;
	}

	void stop(uint32 qty = 0)
	{
		TimeStamp now = timestamp();

		// If 0 then their is the call stack From this, 
		// we can get the function total elapsed time
		if (--inProgress_ == 0){
			lastTime_ = now - lastTime_;
			sumTime_ += lastTime_;
		}

		lastQuantity_ = qty;
		sumQuantity_ += qty;
		++count_;

		ProfileGroup::PROFILEVALS & stack = pProfileGroup_->stack();
		KBE_ASSERT( stack.back() == this );

		stack.pop_back();

		// Time spent in this function
		lastIntTime_ = now - lastIntTime_;
		sumIntTime_ += lastIntTime_;

		// We need to reset the last function within the profile object in the last time
		// So that it can get after calling this function correctly at start time when you enter the next profile function after consumption
		// Pieces of time
		if (!stack.empty())
			stack.back()->lastIntTime_ = now;
	}

	
	INLINE bool stop( const char * filename, int lineNum, uint32 qty = 0);

	INLINE bool running() const;

	INLINE const char * c_str() const;

	INLINE TimeStamp sumTime() const;
	INLINE TimeStamp lastIntTime() const ;
	INLINE TimeStamp sumIntTime() const ;
	INLINE TimeStamp lastTime() const;

	INLINE double lastTimeInSeconds() const;
	INLINE double sumTimeInSeconds() const ;
	INLINE double lastIntTimeInSeconds() const ;
	INLINE double sumIntTimeInSeconds() const;

	

	INLINE const char* name() const;

	INLINE uint32 count() const;

	// The name
	std::string	name_;

	// Profile group pointer
	ProfileGroup * pProfileGroup_;

	// Startd after time.
	TimeStamp		lastTime_;

	// Count the total time
	TimeStamp		sumTime_;

	// Record the last time
	TimeStamp		lastIntTime_;

	// Count the total time
	TimeStamp		sumIntTime_;

	uint32		lastQuantity_;	///< The last value passed into stop.
	uint32		sumQuantity_;	///< The total of all values passed into stop.
	uint32		count_;			///< The number of times stop has been called.

	// Record how many times, such as recursion,
	int			inProgress_;

	bool initWatcher_;

	INLINE bool isTooLong() const;

	static void setWarningPeriod( TimeStamp warningPeriod )
							{ warningPeriod_ = warningPeriod; }

private:
	static TimeStamp warningPeriod_;
};

class ScopedProfile
{
public:
	ScopedProfile(ProfileVal & profile, const char * filename, int lineNum) :
		profile_(profile),
		filename_(filename),
		lineNum_(lineNum)
	{
		profile_.start();
	}

	~ScopedProfile()
	{
		profile_.stop(filename_, lineNum_);
	}

private:
	ProfileVal & profile_;
	const char * filename_;
	int lineNum_;
};

#define START_PROFILE( PROFILE ) PROFILE.start();

#define STOP_PROFILE( PROFILE )	PROFILE.stop( __FILE__, __LINE__ );

#define AUTO_SCOPED_PROFILE( NAME )												\
	static ProfileVal _localProfile( NAME );									\
	ScopedProfile _autoScopedProfile( _localProfile, __FILE__, __LINE__ );

#define SCOPED_PROFILE(PROFILE)													\
	ScopedProfile PROFILE##_scopedProfile(PROFILE, __FILE__, __LINE__);

#define STOP_PROFILE_WITH_CHECK( PROFILE )										\
	if (PROFILE.stop( __FILE__, __LINE__ ))

#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )									\
	PROFILE.stop( __FILE__, __LINE__ , DATA );

// This system profile
uint64 runningTime();

#else

#define AUTO_SCOPED_PROFILE( NAME )
#define STOP_PROFILE_WITH_DATA( PROFILE, DATA )
#define STOP_PROFILE_WITH_CHECK( PROFILE )
#define SCOPED_PROFILE(PROFILE)
#define STOP_PROFILE( PROFILE )
#define START_PROFILE( PROFILE )

uint64 runningTime(){
	return 0;
}

#endif //ENABLE_WATCHERS


}

#ifdef CODE_INLINE
#include "profile.inl"
#endif

#endif // KBENGINE_PROFILE_H


