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
    class IBlock;

    class cField
    {
        private:
            cField * m_next;
            cField * m_prev;
            IBlock * m_left;
            IBlock * m_right;
        public:
            cField();

            cField * next();
            cField * prev();

            void set_next(cField *);
            void set_prev(cField *);
            void attach_left(IBlock *);
            void attach_right(IBlock *);


            friend std::ostream & operator << (std::ostream &, cField &);
            void iprint(std::ostream &); //Saviiteruojantis metodas.

            double calculate(double); //Saviiteruojantis metodas
            double integrate();
            void force_raise(double);

            //Tarpinis kintamasis dE
            double dE;

            double supE;
            double infE;

            //Kintamasis laukui saugoti
            double E;
    };

    class IBlock
    {
        private:
            //friend class IBlock;
        public:
            virtual     IBlock *        next() = 0;
            virtual     IBlock *        prev() = 0;
            virtual     cField *        E_next() = 0;
            virtual     cField *        E_prev() = 0;
            virtual     void            set_next(IBlock *) = 0;
            virtual     void            set_prev(IBlock *) = 0;
            virtual     void            set_E_next(cField *) = 0;
            virtual     void            set_E_prev(cField *) = 0;
            virtual     void            relax() = 0;
            virtual     void            field_create() = 0;
            virtual     void            emit() = 0;
            virtual     void            flush() = 0;
            virtual     void            receive(const double &, const double &) = 0;
            virtual const cData &       read() = 0;
            virtual const cData &       write(const cData &) = 0;
            virtual const eStatus       check() = 0;
            virtual const double        current() = 0;

    };

    // Formulaes
    double drift(const double &, const double &, const double &,
                 const cConstants *, const cGlobal *, const cData &, std::string);
    double diffusion(const double &, const double &, const double &,
                     const cConstants *, const cGlobal *, const cData &, std::string);
    double recombine(const cConstants *, const cGlobal *, const cData &, std::string);

    class cBlock : public IBlock
    {
        private:
            cBlock * m_next;
            cBlock * m_prev;
            cField * m_E_next;
            cField * m_E_prev;
            cData    m_data;
            //MOVED BUFER AREA TO m_data
            //double   m_n_buffer;
            //double   m_p_buffer;

            double m_current;

            cConstants * m_C;
            cGlobal * m_G;

            void recombination();
            void glue_unglue();



        public:
            cBlock(const cData &, cConstants *, cGlobal *);
            virtual     IBlock *        next();
            virtual     IBlock *        prev();
            virtual     cField *        E_next();
            virtual     cField *        E_prev();
            virtual     void            set_next(IBlock *);
            virtual     void            set_prev(IBlock *);
            virtual     void            set_E_next(cField *);
            virtual     void            set_E_prev(cField *);
            virtual     void            relax();
            virtual     void            field_create();
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
            cField * m_E_next;
            cField * m_E_prev;
            const eContactType m_type;
            cData    m_data;
            //double   m_n_buffer;
            //double   m_p_buffer;
            double  m_current;
            cConstants * m_C;
            cGlobal * m_G;

            void injection();
            void recombination();
            void glue_unglue();

        public:
            cContact(eContactType, const cData &, cConstants *, cGlobal *);
            virtual     IBlock *        next();
            virtual     IBlock *        prev();
            virtual     cField *        E_next();
            virtual     cField *        E_prev();
            virtual     void            set_next(IBlock *);
            virtual     void            set_prev(IBlock *);
            virtual     void            set_E_next(cField *);
            virtual     void            set_E_prev(cField *);
            virtual     void            relax();
            virtual     void            field_create();
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
