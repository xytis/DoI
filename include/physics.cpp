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
/*
    static double diffusion(const double & interest,const double & neighbour, const double & D,
                      const cConstants * C, cGlobal * G, const cData & data, std::string place)
    {

        double dc;
        dc = (interest - neighbour)* D * G->dt() / (data.m_width*data.m_width);  //dc = D * deltaC * dt / dx^2
        if (dc>interest/ALLOWED)
            throw exception::TimeIntervalTooLarge(10, std::string("Diffusion ") + place);
        if (dc < C->c_MIN)
            dc = 0;
        return dc;

        return 0; //No diff
    }

    static double drift(const double & interest, const double & field, const double & miu,
                  const cConstants * C, cGlobal * G, const cData & data, std::string place)
    {

        double dc;
        dc = miu * field * G->dt() / data.m_width * interest;
        if (dc > interest/ALLOWED)
            throw exception::TimeIntervalTooLarge(10, std::string("Drift ") + place);
        if (dc < C->c_MIN)
            dc = 0;
        return dc;

        //return 0;   //no drift
    }

    static double recombine(const cConstants * C, cGlobal * G, const cData & data, std::string place)
    {

        double pairs;
        pairs = data.m_n * data.m_p * C->c_beta * G->dt() / C->c_S / data.m_width;
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
