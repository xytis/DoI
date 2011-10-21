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

    std::ostream & operator << (std::ostream & out, cEnvironment & E)
    {
        out << "Įtampa\t\t\t:\t" << E.voltage() << std::endl;
        out << "Laiko žingsnis\t\t:\t" << E.time_step() << std::endl;
        out << "Storis\t\t\t:\t" << E.width() << std::endl;
        out << "Kontaktai\t\t:\t";
        switch (E.contacts())
        {
            case (BLOCKING) : out << "blokuojantys"; break;
            case (INJECTING): out << "injektuojantys"; break;
            case (EXTRACTING): out << "ištraukiantys"; break;
            case (NON_BLOCKING): out << "pralaidūs"; break;
            default : out << "nenustatyti"; break;
        }
        out << std::endl;
        out << "timeout\t\t\t:\t" << E.m_timeout << std::endl;
        out << "time_depth\t\t:\t" << E.m_time_depth << std::endl;
        out << "Padalinimų sk.\t\t:\t" << E.space_division() << std::endl;
        out << "N talpa\t\t\t:\t" << E.capacity_n() << std::endl;
        out << "P talpa\t\t\t:\t" << E.capacity_p() << std::endl;
        return out;
    }




}
