#include "simtime.h"
#include <thread>

using namespace std::chrono;
double simtime::factor = 1.0;
size_t simtime::total_frames = 0;
milliseconds simtime::last_frame_ms = milliseconds();
milliseconds simtime::ms_min = milliseconds();
milliseconds simtime::ms_since_epoch = getTimeSinceEpoch();
milliseconds simtime::total_ms = milliseconds();


milliseconds simtime::getTimeSinceEpoch()
{
  return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}
void capFPS()
{
  using namespace simtime;
  if (ms_min > last_frame_ms) {
	std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(ms_min - last_frame_ms));
  	last_frame_ms = ms_min;
  }
}
void simtime::update()
{
  ++total_frames;
  std::chrono::milliseconds ms_now = getTimeSinceEpoch();
  last_frame_ms = (ms_now - ms_since_epoch) - total_ms;
  capFPS();
  total_ms += last_frame_ms;
}
constexpr size_t toFPS(const milliseconds ms)
{
  return ms.count() ? (size_t)(1000.0/(double)ms.count()) : 0;
}
constexpr milliseconds toMS(const size_t fps)
{
  return fps ? milliseconds((size_t)(1000.0/(double)fps)) : milliseconds();
}
void simtime::setFpsCap(const size_t fps)
{
  ms_min = toMS(fps);
}
void simtime::print()
{
	printf("%lu\tFPS\n", toFPS(last_frame_ms));
}
