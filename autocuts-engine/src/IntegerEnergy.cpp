#include "IntegerEnergy.h"
#include "autodiff.h"

#include <chrono>
#include <thread>
#include <iostream>
#include <igl/cat.h>

using namespace std;

double IntegerEnergySin(double x)
{
    return sin(x);
}


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
    EintX = Eint * X;
    EintX_cosine_rowwise_sum = (EintX.array().cos() + 1).rowwise().sum();
    f = EintX_cosine_rowwise_sum.sum();
}

void IntegerEnergy::gradient(const MatX2& X, Vec& g)
{
    // Reminder: J(f(Av)) = A_t * df(Av). f is coefficient-wise function.
    
    // Calculate Av
    EintX = Eint * X;

    // Calculate df(Av), while f = cos(x)
    MatX2 EintX_cwise_dcosine = -1 * EintX.unaryExpr([](double x) { return sin(x); });

    // Calculate A_t * df(Av)
    MatX2 ge = Eint.transpose() * EintX_cwise_dcosine;

    // Rearrange ge (which is a matrix) to achieve the final gradient vector
    g = Eigen::Map<Vec>(ge.data(), 2.0 * ge.rows(), 1);
}

void IntegerEnergy::hessian(const MatX2& X)
{

}