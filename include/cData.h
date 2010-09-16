/*
 * =====================================================================================
 *
 *       Filename:  cData.h
 *
 *    Description:  Data class which houses all status parameters of a block.
 *
 *        Version:  2.0
 *        Created:  2010-04-13-17.24.57
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
#ifndef CDATA_H_INCLUDED
#define CDATA_H_INCLUDED

#include <iostream>

#include "global.h"
#include "cBlock.h"


namespace DoI
{
    class cData
    {
        public:
            double  m_n;            //active carriers
            double  m_p;
            double  m_n_stuck;      //stuck carriers
            double  m_p_stuck;
            /*
            double  m_n_buffer;     //incoming carriers
            double  m_p_buffer;
            */
            double  m_E;            //electric field in block
            double  m_width;        //width of a block

            //friend class IBlock;

            friend std::ostream & operator << (std::ostream & out, const cData & data)
            {
                out << data.m_n << '\t' << data.m_p << '\t' \
                    << data.m_n_stuck << '\t' << data.m_p_stuck << '\t' \
                    << data.m_E << '\t' << data.m_width << '\n';
                    return out;
            };
            friend std::istream & operator >> (std::istream & in, cData & data)
            {
                in  >> data.m_n >> data.m_p \
                    >> data.m_n_stuck >> data.m_p_stuck \
                    >> data.m_E >> data.m_width;
                return in;
            };

        //public:
            cData() {};
            cData(double n, double p,
                double n_stuck, double p_stuck,
                double E, double width):
                m_n(n), m_p(p),
                m_n_stuck(n_stuck), m_p_stuck(p_stuck),
                m_E(E), m_width(width) {};
            cData(const cData & copy):
                m_n(copy.m_n), m_p(copy.m_p),
                m_n_stuck(copy.m_n_stuck), m_p_stuck(copy.m_p_stuck),
                m_E(copy.m_E), m_width(copy.m_width) {};
    };
};


#endif // CDATA_H_INCLUDED
