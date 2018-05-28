#include "IntegerEnergy.h"
#include "autodiff.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <igl/cat.h>

using namespace std;

IntegerEnergy::IntegerEnergy()
{
}

void IntegerEnergy::init(int n)
{
    igl::cat(1, EVvar1, EVvar2, Eint);
    Eintt = Eint.transpose();
}

void IntegerEnergy::value(const MatX2 &X, double &f)
{
    EintP = Eint * X;
    EintP_cosine_rowwise_sum = EintP.array().cos().rowwise().sum();
}