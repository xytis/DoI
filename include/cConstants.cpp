/*
 * =====================================================================================
 *
 *       Filename:  cConstants.cpp
 *
 *    Description:  implementation file
 *
 *        Version:  2.0
 *        Created:  2010-04-13-18.23.43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
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
        out << "Prilipimo konstanta\t:\t" << C.c_k_glue << std::endl;
        return out;
    }

    std::ostream & operator << (std::ostream & out, cGlobal & G)
    {
        out << "Įtampa\t\t\t:\t" << G.U() << std::endl;
        out << "Laiko žingsnis\t\t:\t" << G.dt() << std::endl;
        out << "Storis\t\t\t:\t" << G.width() << std::endl;
        out << "Kontaktai\t\t:\t";
        switch (G.contacts_type())
        {
            case (BLOCKING) : out << "blokuojantys"; break;
            case (INJECTING): out << "injektuojantys"; break;
            case (EXTRACTING): out << "ištraukiantys"; break;
            case (NON_BLOCKING): out << "pralaidūs"; break;
            default : out << "nenustatyti"; break;
        }
        out << std::endl;
        out << "timeout\t\t\t:\t" << G.timeout() << std::endl;
        out << "time_depth\t\t:\t" << G.time_depth() << std::endl;
        out << "Padalinimų sk.\t\t:\t" << G.size() << std::endl;
        out << "N talpa\t\t\t:\t" << G.n_cap() << std::endl;
        out << "P talpa\t\t\t:\t" << G.p_cap() << std::endl;
        return out;
    }



    double
    transitTime(cEnvironment & E)
    {
        return 0.78*E.width()*E.width()/E.C()->c_n_miu/E.voltage();
    }

    double
    currentMax(cEnvironment & E)
    {
        return 9.0/8.0*E.C()->c_eps*E.C()->c_eps0*(E.C()->c_n_miu +E.C()->c_p_miu)*E.voltage()*E.voltage()/E.width()/E.width()/E.width() * E.C()->c_S;
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

    double
    talpa (double eps, double eps0, double S, double d)
    {
        return eps*eps0*S/d;
    }
}
