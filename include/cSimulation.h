#ifndef CSIMULATION_HPP_INCLUDED
#define CSIMULATION_HPP_INCLUDED

#include "global.h"

#include "cExeptions.h"
#include "cConstants.h"
#include "cMaterial.h"
#include "tFunctors.h"
#include "cPrinter.h"
#include "cEnvironment.h"
#include "cLogger.h"

#include <map>
#include <string>
#include <iostream>
#include <queue>

namespace DoI {
    /** Main simulation executive class.
    Controls constant clases, output classes, parsing of scenario file.
    */
    struct sOperation
    {
        std::string name;
        std::string params;
    };

    class cSimulation : public cObject {

        private:
            ///Environment for the calculation
            cEnvironment * m_environment;
            ///Object for testing
            cMaterial * m_object;
            ///Printer for output
            cPrinter * m_printer;
            ///Operation queue:
            std::queue<sOperation> * m_operation_queue;
            ///Operation map:
            std::map<std::string, bool (cSimulation::*) (std::istream &)> m_operation_map;
            ///Operations:
            bool celiv(std::istream & params);
            bool stable(std::istream & params);

            void do_until(double);
            void do_iter();

        public:
            ///Constructor which creates the object
            cSimulation(cEnvironment *, cMaterial *, cPrinter *, std::queue<sOperation> *);
            ///Main function, which controls the flow
            void run();
            ///Validation function, checks if run method is ok to run
            bool validityCheck();

    };
}

#endif //CSIMULATION_HPP_INCLUDED
