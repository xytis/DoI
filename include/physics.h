#ifndef PHYSICS_H_INCLUDED
#define PHYSICS_H_INCLUDED

#include <cstdlib>
#include <cassert>

#include "global.h"
#include "cData.h"
#include "cConstants.h"
#include "cEnvironment.h"

#define ALLOWED 10

namespace DoI
{
    class cConstants;
    class cEnvironment;
    // Formulaes

    class physics
    {
        public:
            static double drift(const double &, const double &, const double &, cEnvironment *, const cData &, std::string);
            static double diffusion(const double &, const double &, const double &, cEnvironment *, const cData &, std::string);
            static double recombine(cEnvironment *, const cData &, std::string);
            static double diffusionConst(double T, double miu, double q);
            static double recombinationConst(double q, double n_miu, double p_miu, double eps, double eps0);
            static double driftConst(double eps, double eps0, double S);
            static double transitTime(cEnvironment & E);
            static double currentMax(cEnvironment & E);
            static double talpa (double eps, double eps0, double S, double d);
    };
}
#endif // PHYSICS_H_INCLUDED
