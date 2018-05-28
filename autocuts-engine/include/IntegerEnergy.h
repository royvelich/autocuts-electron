#pragma once

#include "EigenTypes.h"

#include <list>

using namespace std;

#ifndef INF
#define INF numeric_limits<double>::infinity()
#endif

class IntegerEnergy
{
  public:
      IntegerEnergy();

    void init(int n);
    void value(const MatX2 &X, double &f);

  private:
    SpMat EVvar1, EVvar2, Eint, Eintt, V2V, V2Vt;
    MatX2 EintP;
    Vec EintP_cosine_rowwise_sum;
};