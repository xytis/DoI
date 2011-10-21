#ifndef CCONSTANTS_H_INCLUDED
#define CCONSTANTS_H_INCLUDED

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>


#include "global.h"
#include "cExeptions.h"
#include "cEnvironment.h"

namespace DoI
{
    class cConstants
    {
        public:
            const double c_beta;
            const double c_S;
            const double c_MIN;
            const double c_q;
            const double c_eps;
            const double c_eps0;
            const double c_k;
            const double c_T;
            const double c_n_miu;
            const double c_p_miu;
            const double c_n_D;
            const double c_p_D;
            const double c_k_glue;


            cConstants(double beta, double S, double MIN, double q, double eps, double eps0,
                       double k, double T, double n_miu, double p_miu, double n_D, double p_D, double k_glue):
                c_beta(beta),
                c_S(S),
                c_MIN(MIN),
                c_q(q),
                c_eps(eps),
                c_eps0(eps0),
                c_k(k),
                c_T(T),
                c_n_miu(n_miu),
                c_p_miu(p_miu),
                c_n_D(n_D),
                c_p_D(p_D),
                c_k_glue(k_glue)
            {

            }

            friend std::ostream & operator << (std::ostream & out, const cConstants & C);
    };
}

#endif // CCONSTANTS_H_INCLUDED
