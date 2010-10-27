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

            cConstants(double beta, double S, double MIN, double q, double eps, double eps0,
                       double k, double T, double n_miu, double p_miu, double n_D, double p_D):
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
                c_p_D(p_D)
            {

            }

            friend std::ostream & operator << (std::ostream & out, const cConstants & C);
    };

    class cGlobal {
        private:
            double        m_U;
            double        m_dt;
            double        m_width;
            int           m_timeout;
            int           m_time_depth;         //Naudojamas nustatyti kiek kartų laikas buvo sumažintas.
            uint64_t      m_size;
        public:
            cGlobal (double U, double dt, double width,
                    int timeout, int time_depth, uint64_t size):
            m_U(U),
            m_dt(dt),
            m_width(width),
            m_timeout(timeout),
            m_time_depth(time_depth),
            m_size(size)
            {};

            double        U() { return m_U; };
            double        dt() { return m_dt; };
            double        width() { return m_width; };
            int           timeout() { return m_timeout; };
            int           time_depth() { return m_time_depth; };
            uint64_t      size() { return m_size; };

            void          s_U(double n_U) { m_U = n_U; };
            void          s_dt(double n_dt) { m_dt = n_dt; };
            void          s_width(double n_width) { m_width = n_width; };
            void          s_timeout(int n_timeout) { m_timeout = n_timeout; };
            void          s_time_depth(int n_time_depth) { m_time_depth = n_time_depth; };
            void          s_size(uint64_t n_size) { m_size = n_size; };

            friend std::ostream & operator << (std::ostream & out, cGlobal & G);
    };

    double diffusionConst(double T, double miu, double q);
    double recombinationConst(double q, double n_miu, double p_miu, double eps, double eps0);
    double driftConst(double eps, double eps0, double S);
    double transitTime(cConstants & C, cGlobal & G);
    double currentMax(cConstants & C, cGlobal & G);
};

#endif // CCONSTANTS_H_INCLUDED
