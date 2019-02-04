#include "timer.h"

Timer::Timer()
{
  start_ = clock();
}

bool Timer::timeOver(double limit)
{
  double time_passed = (double)(clock())/CLOCKS_PER_SEC - (double)(start_)/CLOCKS_PER_SEC;
  if (time_passed > limit)
  {
    start_ = clock(); //refresh starting time
    return true;
  }

  else
    return false;
}
