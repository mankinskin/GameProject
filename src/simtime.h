#pragma once
#include <cstddef>
#include <chrono>


namespace simtime
{
  extern float speed;	  // multiply to speed up global time
  extern float factor;	  // the ratio of real time to simulation time
  extern size_t total_frames;
  extern std::chrono::nanoseconds last_frame_ns;	  // the miliseconds the last frame took
  extern std::chrono::nanoseconds ns_min;		  // minimum ms per frame
  extern std::chrono::milliseconds ms_since_epoch;	  // ms since unix epoch, different for each execution
  extern std::chrono::milliseconds total_ms;	  // ms duration of this execution

  void update();
  void setFpsCap(const size_t cap);
  void print();
  template<typename Res>
	Res getTimeSinceEpoch()
	{
	  return std::chrono::duration_cast<Res>(std::chrono::high_resolution_clock::now().time_since_epoch());
	}
}
