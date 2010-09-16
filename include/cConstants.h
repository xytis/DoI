#ifndef CCONSTANTS_H_INCLUDED
#define CCONSTANTS_H_INCLUDED

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>


#include "global.h"
#include "CExeptions.h"

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
            double        m_U;
            double        m_dt;
            double        m_width;
            int           m_timeout;
            uint64_t      m_size;

            cConstants(double beta, double S, double min, double q, double eps, double eps0):
                c_S(4e-6),
                c_MIN(0.0001),
                c_q(1.6e-19),
                c_eps(3.5),
                c_eps0(8.85e-12),
                c_k(1/(c_eps * c_eps0 * c_S)),
                c_T(273),
                c_n_miu(1e-6),
                c_p_miu(1e-7),
                c_n_D(1.38e-23 * c_T * c_n_miu / c_q),
                c_p_D(1.38e-23 * c_T * c_p_miu / c_q),
                c_beta(c_q * (c_n_miu + c_p_miu) / c_eps /c_eps0),//5.165e-11), //Landþeveno rekombinacija
                m_U(8),
                m_dt(1e-11),
                m_width(1e-6),
                m_timeout(-1),
                m_size(0)
            {


            }

            double transitTime();
            double currentMax();
    };

    cConstants * readConstants(std::string file);
};

#endif // CCONSTANTS_H_INCLUDED
