#pragma once
#ifndef INFERNO_UTILITIES_TIMER
#define INFERNO_UTILITIES_TIMER

#include <stdexcept>

# if  (defined(_INFERNO_TIMER_MACH__) || defined(__APPLE__))
#   define INFERNO_TIMER_MAC
# elif (defined(_WIN32) || defined(WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(_WIN64))
#   define INFERNO_TIMER_WINDOWS
#   ifndef WIN32_LEAN_AND_MEAN
#     define WIN32_LEAN_AND_MEAN
#   endif
# endif

# if defined(INFERNO_TIMER_MAC)
#    include <mach/mach_time.h>
# elif defined(INFERNO_TIMER_WINDOWS)
#    include <windows.h>
#    undef min
#    undef max
# else
#    include <time.h>
# endif

namespace inferno {

/// Platform-independent runtime measurements
class Timer {
public:
   // construction
   Timer();

   // query
   double elapsedTime() const;

   // manipulation
   void tic();
   void toc();
   void reset();

private:
   #if defined(INFERNO_TIMER_MAC)
      typedef uint64_t TimerT;
      typedef double TimerC;
   #elif defined(INFERNO_TIMER_WINDOWS)
      typedef LONGLONG TimerT;
      typedef LARGE_INTEGER TimerC;
   #else
      typedef double TimerT;
      typedef timespec TimerC;
   #endif

   TimerT start_;
#if !defined(INFERNO_TIMER_MAC)
   TimerC ts_;
#endif
   double duration_;
   double conversionFactor_;
   double elapsedTime_;
};

/// Platform-independent runtime measurements of functors
template<class FUNCTOR>
class Timing {
public:
   typedef FUNCTOR Functor;

   Timing(Functor, const size_t = 1);
   const std::vector<double>& times() const;

private:
   Functor functor_;
   std::vector<double> times_;
};

inline Timer::Timer()
: start_(0), duration_(0), elapsedTime_(0)
{
   #if defined(INFERNO_TIMER_MAC)
      mach_timebase_info_data_t info;
      mach_timebase_info(&info);
      conversionFactor_ = (static_cast<double>(info.numer))/
                     (static_cast<double>(info.denom));
      conversionFactor_ = conversionFactor_*1.0e-9;
   #elif defined(INFERNO_TIMER_WINDOWS)
      TimerC freq;
      QueryPerformanceFrequency(&freq);
      conversionFactor_ = 1.0/(static_cast<double>(freq.QuadPart));
   #else
      conversionFactor_ = 1.0;
   #endif
   reset();
}

inline void Timer::tic() {
   #if defined(INFERNO_TIMER_MAC)
      start_ = mach_absolute_time();
   #elif defined(INFERNO_TIMER_WINDOWS)
      QueryPerformanceCounter(&ts_);
      start_ = ts_.QuadPart;
   #else
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_);
      start_ = static_cast<double>(ts_.tv_sec) + 1.0e-9 *
               static_cast<double>(ts_.tv_nsec);
   #endif
}

inline void Timer::toc() {
   #if defined(INFERNO_TIMER_MAC)
      duration_ =  static_cast<double>(mach_absolute_time() - start_);
   #elif defined(INFERNO_TIMER_WINDOWS)
      LARGE_INTEGER qpc_t; // ???
      QueryPerformanceCounter(&qpc_t);
      duration_ = static_cast<double>(qpc_t.QuadPart - start_);
   #else
      clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_);
      duration_ = (static_cast<double>(ts_.tv_sec) + 1.0e-9 *
                  static_cast<double>(ts_.tv_nsec)) - start_;
   #endif
   elapsedTime_ = duration_*conversionFactor_;
}

inline void Timer::reset() {
   start_ = 0;
   duration_ = 0;
   elapsedTime_ = 0;
}

inline double Timer::elapsedTime() const {
   return elapsedTime_;
}

template<class FUNCTOR>
inline Timing<FUNCTOR>::Timing(
   FUNCTOR functor,
   const size_t repetitions
)
:  functor_(functor), 
   times_(std::vector<double>()) 
{
   if(repetitions < 1) {
      throw std::runtime_error("The number of repetition must be at least 1.");
   }
   for(size_t j=0; j<repetitions; ++j) {
      inferno::Timer timer;
      timer.tic();
      functor_();
      timer.toc();
      times_.push_back(timer.elapsedTime());
   }
}

template<class FUNCTOR>
inline const std::vector<double>&
Timing<FUNCTOR>::times() const {
   return times_;
}

} // namespace inferno

#if defined(INFERNO_TIMER_WINDOWS)
# undef WIN32_LEAN_AND_MEAN
#endif

#endif // INFERNO_UTILITIES_TIMER
