#include "simtime.h"
#include <thread>

using namespace std::chrono;
float simtime::speed = 1.0;
float simtime::factor = 1.0;
size_t simtime::total_frames = 0;
nanoseconds simtime::last_frame_ns = nanoseconds();
nanoseconds simtime::ns_min = nanoseconds();
milliseconds simtime::ms_since_epoch = simtime::getTimeSinceEpoch<milliseconds>();
milliseconds simtime::total_ms = milliseconds();


void capFPS()
{
  using namespace simtime;
  if (ns_min.count() > last_frame_ns.count()) {
	std::this_thread::sleep_for(ns_min - last_frame_ns);
  	last_frame_ns = ns_min;
  }
}
void simtime::update()
{
  ++total_frames;
  std::chrono::milliseconds ms_now = getTimeSinceEpoch<milliseconds>();
  last_frame_ns = (ms_now - ms_since_epoch) - total_ms;
  capFPS();
  factor = (float)last_frame_ns.count() / 1000000000.0f;
  total_ms += duration_cast<milliseconds>(last_frame_ns);
  //print();
}
constexpr size_t toFPS(const nanoseconds ns)
{
  return ns.count() ? (size_t)(1000000000.0/(float)ns.count()) : 0;
}
constexpr nanoseconds toMS(const size_t fps)
{
  return fps ? nanoseconds((size_t)(1000000000.0/(float)fps)) : nanoseconds();
}
void simtime::setFpsCap(const size_t fps)
{
  ns_min = toMS(fps);
}
void simtime::print()
{
	printf("%lu\tFPS\n", toFPS(last_frame_ns));
}
