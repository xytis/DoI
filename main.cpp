#include <iostream>
#include <iomanip>
#include <sstream>

#include "include/cLogger.h"

#include "include/cMaterial.h"
#include "include/cConstants.h"
#include "include/cData.h"
#include "include/tFunctors.h"

#include "include/cSimulation.h"

#include "include/physics.h"

#include "include/cParser.h"

//#define DUMP

using namespace DoI;

int main(int argc, char * argv[])
{
    cMainParser main_parser;
    try {
        if (argc>1)
        {
            std::string filename(argv[1]);
            main_parser.parse(filename);
            cSimulation * sim = reinterpret_cast<cSimulation *>(main_parser.object());
            //sim->run();
        }
        else
        {
            main_parser.parse(std::string("scenario.txt"));
            cSimulation * sim = reinterpret_cast<cSimulation *>(main_parser.object());
            //sim->run();
        }
    }
    catch (exception::FileMisingExeption & e)
    {
        cLogger::error(e.what());
    }
    catch (exception::FileBadExeption & e)
    {
        cLogger::error(e.what());
    }
    catch (exception::BadCommand & e)
    {
        cLogger::error(std::string(e.what()) + ": " + e.reason());
    }
    catch (exception::NoObject & e)
    {
        cLogger::error(e.what());
    }
    return 0;
}
