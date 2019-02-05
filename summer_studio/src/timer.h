#include <time.h>
#include <iostream>
class Timer
{
public:
  Timer();
  bool timeOver(double limit);

private:
  double counter_;
  clock_t start_;
};
