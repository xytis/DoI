/*
 * =====================================================================================
 *
 *       Filename:  cBlock.cpp
 *
 *    Description:  implementation file
 *
 *        Version:  2.0
 *        Created:  2011-10-17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
#include "physics.h"

namespace DoI
{
    // Helpers for formulaes

    double diffusion(const double & interest,const double & neighbour, const double & D,
                      cEnvironment * E, const cData & data, std::string place)
    {

        double dc;
        dc = (interest - neighbour)* D * E->time_step() / (data.m_width*data.m_width);  //dc = D * deltaC * dt / dx^2
        if (dc>interest/ALLOWED)
            throw exception::TimeIntervalTooLarge(10, std::string("Diffusion ") + place);
        if (dc < E->C()->c_MIN)
            dc = 0;
        return dc;

        return 0; //No diff
    }

    double drift(const double & interest, const double & field, const double & miu,
                  cEnvironment * E, const cData & data, std::string place)
    {

        double dc;
        dc = miu * field * E->time_step() / data.m_width * interest;
        if (dc > interest/ALLOWED)
            throw exception::TimeIntervalTooLarge(10, std::string("Drift ") + place);
        if (dc < E->C()->c_MIN)
            dc = 0;
        return dc;

        //return 0;   //no drift
    }

    double recombine(cEnvironment * E, const cData & data, std::string place)
    {

        double pairs;
        pairs = data.m_n * data.m_p * E->C()->c_beta * E->time_step() / E->C()->c_S / data.m_width;
        if ((pairs > data.m_n/ALLOWED)||(pairs > data.m_p/ALLOWED))
            throw exception::TimeIntervalTooLarge(10, std::string("Recombination ") + place);
            //pairs = data.m_n>data.m_p?data.m_p:data.m_n;
        if (pairs < E->C()->c_MIN)
        {
            pairs = 0;
        }
        return pairs;

        //return 0;
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
