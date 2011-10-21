#ifndef CFIELD_HPP_INCLUDED
#define CFIELD_HPP_INCLUDED

#include "cBlock.h"



namespace DoI
{
    class iBlock;

    class cField
    {
        private:
            cField * m_next;
            cField * m_prev;
            iBlock * m_left;
            iBlock * m_right;
        public:
            cField();

            cField * next();
            cField * prev();

            void set_next(cField *);
            void set_prev(cField *);
            void attach_left(iBlock *);
            void attach_right(iBlock *);


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
}

#endif //CFIELD_HPP_INCLUDED
