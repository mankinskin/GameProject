#pragma once
#include <cstddef>
#include <chrono>


namespace simtime
{
  extern double factor;	  // multiply to speed up global time
  extern size_t total_frames;
  extern std::chrono::milliseconds last_frame_ms;	  // the miliseconds the last frame took
  extern std::chrono::milliseconds ms_min;		  // minimum ms per frame
  extern std::chrono::milliseconds ms_since_epoch;	  // ms since unix epoch, different for each execution
  extern std::chrono::milliseconds total_ms;	  // ms duration of this execution

  void update();
  void setFpsCap(const size_t cap);
  void print();
  std::chrono::milliseconds getTimeSinceEpoch();
}
