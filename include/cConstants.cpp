#include "cConstants.h"

namespace DoI
{
    std::ostream & operator << (std::ostream & out, const cConstants & C)
    {
        out << "Plotas\t\t\t:\t" << C.c_S << std::endl;
        out << "MIN\t\t\t:\t" << C.c_MIN << std::endl;
        out << "Krūvis\t\t\t:\t" << C.c_q << std::endl;
        out << "Eps\t\t\t:\t" << C.c_eps << std::endl;
        out << "Eps0\t\t\t:\t" << C.c_eps0 << std::endl;
        out << "Dreifo K\t\t:\t" << C.c_k << std::endl;
        out << "Temperatūra\t\t:\t" << C.c_T << std::endl;
        out << "Judris n\t\t:\t" << C.c_n_miu << std::endl;
        out << "Judris p\t\t:\t" << C.c_p_miu << std::endl;
        out << "Diffusion n\t\t:\t" << C.c_n_D << std::endl;
        out << "Diffusion p\t\t:\t" << C.c_p_D << std::endl;
        out << "Recombination Beta\t:\t" << C.c_beta << std::endl;
        return out;
    }

    std::ostream & operator << (std::ostream & out, cGlobal & G)
    {
        out << "Įtampa\t\t\t:\t" << G.U() << std::endl;
        out << "Laiko žingsnis\t\t:\t" << G.dt() << std::endl;
        out << "Storis\t\t\t:\t" << G.width() << std::endl;
        out << "timeout\t\t\t:\t" << G.timeout() << std::endl;
        out << "time_depth\t\t:\t" << G.time_depth() << std::endl;
        out << "Padalinimų sk.\t\t:\t" << G.size() << std::endl;
        return out;
    }



    double
    transitTime(cConstants & C, cGlobal & G)
    {
        return 0.78*G.width()*G.width()/C.c_n_miu/G.U();
    }

    double
    currentMax(cConstants & C, cGlobal & G)
    {
        return 9.0/8.0*C.c_eps*C.c_eps0*(C.c_n_miu +C.c_p_miu)*G.U()*G.U()/G.width()/G.width()/G.width() * C.c_S;
    }

    double
    diffusionConst(double T, double miu, double q)
    {
        return (1.38e-23 * T * miu / q);
    }

    double
    recombinationConst(double q, double n_miu, double p_miu, double eps, double eps0)
    {
        return q * (n_miu + p_miu) / eps /eps0; //Landþeveno rekombinacija
    }

    double
    driftConst(double eps, double eps0, double S)
    {
        return (1/(eps * eps0 * S));
    }
};
