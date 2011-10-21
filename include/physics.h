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
            static double drift(const double &, const double &, const double &,
                         const cEnvironment *, const cData &, std::string);
            static double diffusion(const double &, const double &, const double &,
                             const cEnvironment *, const cData &, std::string);
            static double recombine(const cEnvironment *, const cData &, std::string);

            static double diffusionConst(double T, double miu, double q);
            static double recombinationConst(double q, double n_miu, double p_miu, double eps, double eps0);
            static double driftConst(double eps, double eps0, double S);
            static double transitTime(cEnvironment & E);
            static double currentMax(cEnvironment & E);
    };

/*
    static double diffusion(const double & interest,const double & neighbour, const double & D,
                      const cConstants * C, cEnvironment * G, const cData & data, std::string place)
    {

        double dc;
        dc = (interest - neighbour)* D * G->time_step() / (data.m_width*data.m_width);  //dc = D * deltaC * dt / dx^2
        if (dc>interest/ALLOWED)
            throw exception::TimeIntervalTooLarge(10, std::string("Diffusion ") + place);
        if (dc < C->c_MIN)
            dc = 0;
        return dc;

        //return 0; //No diff
    }

    static double drift(const double & interest, const double & field, const double & miu,
                  const cConstants * C, cEnvironment * G, const cData & data, std::string place)
    {

        double dc;
        dc = miu * field * G->time_step() / data.m_width * interest;
        if (dc > interest/ALLOWED)
            throw exception::TimeIntervalTooLarge(10, std::string("Drift ") + place);
        if (dc < C->c_MIN)
            dc = 0;
        return dc;

        //return 0;   //no drift
    }

    static double recombine(const cConstants * C, cEnvironment * G, const cData & data, std::string place)
    {

        double pairs;
        pairs = data.m_n * data.m_p * C->c_beta * G->time_step() / C->c_S / data.m_width;
        if ((pairs > data.m_n/ALLOWED)||(pairs > data.m_p/ALLOWED))
            throw exception::TimeIntervalTooLarge(10, std::string("Recombination ") + place);
            //pairs = data.m_n>data.m_p?data.m_p:data.m_n;
        if (pairs < C->c_MIN)
        {
            pairs = 0;
        }
        return pairs;

        //return 0;
    }
    */
}
#endif // PHYSICS_H_INCLUDED
