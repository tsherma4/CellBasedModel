// [[Rcpp::depends(BH)]]

#include <boost/random.hpp>
#include <stdint.h>

#include "Random.h"

static boost::random::mt19937 rng;

void Random::setSeed(uint32_t newSeed)
{
    rng.seed(newSeed);
}

double Random::normal(double mean, double var)
{
    boost::random::normal_distribution<double> dist(mean, var);
    return dist(rng);
}

int Random::uniformInt(int a, int b)
{
    if (a > b)
    {
        throw std::invalid_argument("uniformInt: invalid range\n");
    }
    else if (a == b)
    {
        return a;
    }
    else
    {
        boost::random::uniform_int_distribution<> dist(a,b);
        return dist(rng);
    }
}

double Random::uniform(double a, double b)
{
    if (a > b)
    {
        throw std::invalid_argument("uniform: invalid range\n");
    }
    else if (a == b)
    {
        return a;
    }
    else
    {
        boost::random::uniform_real_distribution<> dist(a,b);
        return dist(rng);
    }
}

