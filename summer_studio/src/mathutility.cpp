#include "mathutility.h"

namespace mathutility
{
  double dotProduct(Vectortype one, Vectortype two)
  {
    return (one.i_ * two.i_) + (one.j_ * two.j_);
  }

  Vectortype normalize(Vectortype one)
  {
    double magnitude = sqrt(pow(one.i_,2) + pow(one.j_, 2));
    double temp_i = one.i_/magnitude;
    double temp_j = one.j_/magnitude;

    return Vectortype(temp_i, temp_j);
  }


  Vectortype reflectVect(Vectortype incident)
  {
    Vectortype normal = normalize(incident);
    double a = -2 * dotProduct(incident, normal);
    Vectortype b(a * normal.i_, a * normal.j_);
    return Vectortype(b.i_ + incident.i_, b.j_ + incident.j_);
  }
}
