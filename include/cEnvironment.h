#ifndef CENVIRONMENT_H_INCLUDED
#define CENVIRONMENT_H_INCLUDED

#include "global.h"
#include "cConstants.h"
#include "cMaterial.h"

namespace DoI
{
    class cConstants;

    enum CONTACTS_TYPE
    {
        BLOCKING  = 0,
        INJECTING = 1,
        EXTRACTING = 2,
        NON_BLOCKING = 3    //NON_BLOCKING = INJECTING+EXTRACTING
    };

    class cEnvironment: public cObject
    {
        public:
            cEnvironment(cConstants * constants, double time_step, uint64_t space_division, double width, CONTACTS_TYPE contacts, double capacity_n, double capacity_p);
            cConstants * C();

            double time_step();
            void time_step(double);

            uint64_t space_division();
            void space_division(uint64_t);

            double width();
            void width(double);

            CONTACTS_TYPE contacts();
            void contacts(CONTACTS_TYPE);

            double voltage();
            void voltage(double);

            double capacity_n();
            double capacity_p();

            double m_time;
            double m_backup_time;
            int m_timeout;
            int m_time_depth;

            ~cEnvironment() {};
        protected:

        private:
            cConstants * m_constants;
            double m_time_step;
            uint64_t m_space_division;
            double m_width;
            CONTACTS_TYPE m_contacts;
            double m_voltage;
            double m_capacity_n;
            double m_capacity_p;


    };
}

#endif // CENVIRONMENT_H_INCLUDED
