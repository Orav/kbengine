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

#ifndef KBE_TIMESTAMP_H
#define KBE_TIMESTAMP_H

#include "common/platform.h"

namespace KBEngine{

// Indicates whether you can by calling the RDTSC (timestamp counters)
// Compute the time stamp. The advantage of using this is, it can quickly and accurately return the actual clock ticks
// . Deficiencies are, it does not use Speed step technology to change the clock speed of the CPU.
#ifdef unix
//#define KBE_USE_RDTSC
#else // unix
//#define KBE_USE_RDTSC
#endif // unix

enum KBETimingMethod
{
	RDTSC_TIMING_METHOD, // Since the CPU is powered on after a number of clock cycles, achieve nanosecond timing accuracy
	GET_TIME_OF_DAY_TIMING_METHOD,
	GET_TIME_TIMING_METHOD,
	NO_TIMING_METHOD,
};

extern KBETimingMethod g_timingMethod;

const char* getTimingMethodName();

#ifdef unix

inline uint64 timestamp_rdtsc()
{
	uint32 rethi, retlo;
	__asm__ __volatile__ (
		"rdtsc":
		"=d"    (rethi),
		"=a"    (retlo)
						  );
	return uint64(rethi) << 32 | retlo; 
}

// Using gettimeofday. Test than RDTSC20 times-about 600 times.
// In addition, there is a problem
// Under the 2.4 kernel, twice calling gettimeofday may
// Returns a result that is going backwards.
#include <sys/time.h>

inline uint64 timestamp_gettimeofday()
{
	timeval tv;
	gettimeofday( &tv, NULL );
	return 1000000ULL * uint64( tv.tv_sec ) + uint64( tv.tv_usec );
}

#include <time.h>
#include <asm/unistd.h>

inline uint64 timestamp_gettime()
{
	timespec tv;
	assert(syscall( __NR_clock_gettime, CLOCK_MONOTONIC, &tv ) == 0);
	return 1000000000ULL * tv.tv_sec + tv.tv_nsec;
}

inline uint64 timestamp()
{
#ifdef KBE_USE_RDTSC
	return timestamp_rdtsc();
#else // KBE_USE_RDTSC
	if (g_timingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if (g_timingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else //if (g_timingMethod == GET_TIME_TIMING_METHOD)
		return timestamp_gettime();

#endif // KBE_USE_RDTSC
}

#elif defined(_WIN32)

#ifdef KBE_USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
inline uint64 timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // KBE_USE_RDTSC

#include <windows.h>

inline uint64 timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter( &counter );
	return counter.QuadPart;
}

#endif // KBE_USE_RDTSC

#else
	#error Unsupported platform!
#endif

uint64 stampsPerSecond();
double stampsPerSecondD();

uint64 stampsPerSecond_rdtsc();
double stampsPerSecondD_rdtsc();

uint64 stampsPerSecond_gettimeofday();
double stampsPerSecondD_gettimeofday();

inline double stampsToSeconds( uint64 stamps )
{
	return double( stamps )/stampsPerSecondD();
}

// -----------------------------------------------------------------------------
class TimeStamp
{
public:
	TimeStamp( uint64 stamps = 0 ) : stamp_( stamps ) {}

	operator uint64 &()				{ return stamp_; }
	operator uint64() const			{ return stamp_; }
	
	inline uint64 stamp(){ return stamp_; }

	inline double inSeconds() const;
	inline void setInSeconds( double seconds );

	inline TimeStamp ageInStamps() const;
	inline double ageInSeconds() const;

	inline static double toSeconds( uint64 stamps );
	inline static TimeStamp fromSeconds( double seconds );

	uint64	stamp_;
};


inline double TimeStamp::toSeconds( uint64 stamps )
{
	return double( stamps )/stampsPerSecondD();
}

inline TimeStamp TimeStamp::fromSeconds( double seconds )
{
	return uint64( seconds * stampsPerSecondD() );
}

inline double TimeStamp::inSeconds() const
{
	return toSeconds( stamp_ );
}

inline void TimeStamp::setInSeconds( double seconds )
{
	stamp_ = fromSeconds( seconds );
}

inline TimeStamp TimeStamp::ageInStamps() const
{
	return timestamp() - stamp_;
}

inline double TimeStamp::ageInSeconds() const
{
	return toSeconds( this->ageInStamps() );
}

}

#endif // KBE_TIMESTAMP_H
