/*
 * =====================================================================================
 *
 *       Filename:  CBlock.h
 *
 *    Description:  Abstract virtual class for defining a block in material
 *
 *        Version:  2.0
 *        Created:  2010-04-13-10.53.52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
#ifndef CBLOCK_H_INCLUDED
#define CBLOCK_H_INCLUDED

#include <cstdlib>
#include <cassert>

#include "global.h"
#include "cData.h"
#include "cStatus.h"
#include "cConstants.h"

namespace DoI
{
    enum eStatus
    {
        OK,
        WRONG
    };

    class cData;

    class IBlock
    {
        private:
            //friend class IBlock;
        public:
            virtual     IBlock *        next() = 0;
            virtual     IBlock *        prev() = 0;
            virtual     void            set_next(IBlock *) = 0;
            virtual     void            set_prev(IBlock *) = 0;
            virtual     void            relax() = 0;
            virtual     void            field(double &, double &) = 0;
            virtual     void            applyVoltage(double) = 0;
            virtual     void            emit() = 0;
            virtual     void            flush() = 0;
            virtual     void            receive(const double &, const double &) = 0;
            virtual const cData &       read() = 0;
            virtual const cData &       write(const cData &) = 0;
            virtual const eStatus       check() = 0;
            virtual const double       current() = 0;

    };

    class cBlock : public IBlock
    {
        private:
            cBlock * m_next;
            cBlock * m_prev;
            cData    m_data;
            double   m_n_buffer;
            double   m_p_buffer;

            double m_current;

            cConstants * m_C;

            void recombination();
            void glue_unglue();

            double drift(const double &, const double &, const double &, std::string);
            double diffusion(const double &, const double &, const double &, std::string);

        public:
            cBlock(const cData &, cConstants *);
            virtual     IBlock *        next();
            virtual     IBlock *        prev();
            virtual     void            set_next(IBlock *);
            virtual     void            set_prev(IBlock *);
            virtual     void            relax();
            virtual     void            field(double &,double &);
            virtual     void            applyVoltage(double);
            virtual     void            emit();
            virtual     void            flush();
            virtual     void            receive(const double &, const double &);
            virtual const cData &       read();
            virtual const cData &       write(const cData &);
            virtual const eStatus       check();
            virtual const double       current();
    };

    enum eContactType
    {
        RIGHT,
        LEFT
    };

    class cContact : public IBlock
    {
        private:
            cBlock * m_block;
            const eContactType m_type;
            cData    m_data;
            double   m_n_buffer;
            double   m_p_buffer;
            double  m_current;
            cConstants * m_C;

            void injection();
            void recombination();
            void glue_unglue();

            double drift(const double &, const double &, const double &, std::string);
            double diffusion(const double &, const double &, const double &, std::string);

        public:
            cContact(eContactType, const cData &, cConstants *);
            virtual     IBlock *        next();
            virtual     IBlock *        prev();
            virtual     void            set_next(IBlock *);
            virtual     void            set_prev(IBlock *);
            virtual     void            relax();
            virtual     void            field(double &,double &);
            virtual     void            applyVoltage(double);
            virtual     void            emit();
            virtual     void            flush();
            virtual     void            receive(const double &, const double &);
            virtual const cData &       read();
            virtual const cData &       write(const cData &);
            virtual const eStatus       check();
            virtual const double       current();
    };

};

#endif // CBLOCK_H_INCLUDED
