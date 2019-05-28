#include <thread>
#include "util/thread.hpp"


namespace util::thread
{
	void Sleep(const real& secs)
	{
		std::this_thread::sleep_for(std::chrono::duration<real>(secs));
	}
}
