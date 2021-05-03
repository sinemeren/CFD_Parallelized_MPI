#ifndef __RANDWERTE_HPP__
#define __RANDWERTE_HPP__
#include "datastructures.hpp"

/**
 * The boundary values of the problem are set.
 */
void boundaryvalues(
  int imax,
  int jmax,
  matrix<double> &U,
  matrix<double> &V,
  int l_rank,
  int r_rank,
  int b_rank,
  int t_rank
);

#endif
