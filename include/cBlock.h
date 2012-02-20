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

#include "physics.h"
#include "cData.h"
#include "cField.h"
#include "cEnvironment.h"

namespace DoI
{
    class cData;
    class cField;
    class cEnvironment;

    /** Interface class for inherinting. */
    class iBlock
    {
        private:

        public:
            virtual     iBlock *        next() = 0;
            virtual     iBlock *        prev() = 0;
            virtual     cField *        E_next() = 0;
            virtual     cField *        E_prev() = 0;
            virtual     void            set_next(iBlock *) = 0;
            virtual     void            set_prev(iBlock *) = 0;
            virtual     void            set_E_next(cField *) = 0;
            virtual     void            set_E_prev(cField *) = 0;
            virtual     void            relax() = 0;
            virtual     void            field_create() = 0;
            virtual     void            emit() = 0;
            virtual     void            flush() = 0;
            virtual     void            receive(const double &, const double &) = 0;
            virtual const cData &       read() = 0;
            virtual const cData &       write(const cData &) = 0;
            //virtual const eStatus       check() = 0;
            virtual const double        current() = 0;
            virtual const double       drift_current() = 0;
            virtual const double       diff_current() = 0;

    };

    class cBlock : public iBlock
    {
        private:
            cBlock * m_next;
            cBlock * m_prev;
            cField * m_E_next;
            cField * m_E_prev;
            cData    m_data;

            double m_current;
            double  m_diff_current;
            double  m_drift_current;

            cEnvironment * m_E;

            void recombination();
            void glue_unglue();

        public:
            cBlock(const cData &, cEnvironment *);
            virtual     iBlock *        next();
            virtual     iBlock *        prev();
            virtual     cField *        E_next();
            virtual     cField *        E_prev();
            virtual     void            set_next(iBlock *);
            virtual     void            set_prev(iBlock *);
            virtual     void            set_E_next(cField *);
            virtual     void            set_E_prev(cField *);
            virtual     void            relax();
            virtual     void            field_create();
            virtual     void            emit();
            virtual     void            flush();
            virtual     void            receive(const double &, const double &);
            virtual const cData &       read();
            virtual const cData &       write(const cData &);
            //virtual const eStatus       check();
            virtual const double       current();
            virtual const double       drift_current();
            virtual const double       diff_current();
    };

    enum eContactType
    {
        RIGHT,
        LEFT
    };

    class cContact : public iBlock
    {
        private:
            cBlock * m_block;
            cField * m_E_next;
            cField * m_E_prev;
            const eContactType m_type;
            cData    m_data;
            double  m_current;
            double  m_diff_current;
            double  m_drift_current;

            cEnvironment * m_E;

            void injection();
            void escape();
            void extraction();
            void recombination();
            void glue_unglue();

        public:
            cContact(eContactType, const cData &, cEnvironment *);
            virtual     iBlock *        next();
            virtual     iBlock *        prev();
            virtual     cField *        E_next();
            virtual     cField *        E_prev();
            virtual     void            set_next(iBlock *);
            virtual     void            set_prev(iBlock *);
            virtual     void            set_E_next(cField *);
            virtual     void            set_E_prev(cField *);
            virtual     void            relax();
            virtual     void            field_create();
            virtual     void            emit();
            virtual     void            flush();
            virtual     void            receive(const double &, const double &);
            virtual const cData &       read();
            virtual const cData &       write(const cData &);
            //virtual const eStatus       check();
            virtual const double       current();
            virtual const double       drift_current();
            virtual const double       diff_current();
    };

}

#endif // CBLOCK_H_INCLUDED
