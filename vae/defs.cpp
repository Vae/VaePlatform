//
// Created by protocol on 4/28/21.
//

#include "defs.hpp"

double vae::randomDouble(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}