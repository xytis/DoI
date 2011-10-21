#ifndef CSIMULATION_HPP_INCLUDED
#define CSIMULATION_HPP_INCLUDED

#include "global.h"

#include "cExeptions.h"
#include "cConstants.h"
#include "cMaterial.h"
#include "tFunctors.h"
#include "cPrinter.h"
#include "cEnvironment.h"

#include <map>
#include <string>
#include <iostream>

namespace DoI {
    /** Main simulation executive class.
    Controls constant clases, output classes, parsing of scenario file.
    */
    class cSimulation : public cObject {

        private:
            ///Environment for the calculation
            cEnvironment * m_environment;
            ///Object for testing
            cMaterial * m_object;
            ///Printer for output
            cPrinter * m_printer;

        public:
            ///Constructor which creates the object
            cSimulation();
            ///Main function, which controls the flow
            void run();
            ///Validation function, checks if run method is ok to run
            bool validityCheck();

    };
}

#endif //CSIMULATION_HPP_INCLUDED
