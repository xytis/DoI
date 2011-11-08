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

#include "include/calculator.h"

//#define DUMP

using namespace DoI;



int main(int argc, char* argv[])
{
    cCalculator calc;
    calc.parse_line("f(x) = 2 * x; g(x) = 3 * x; a = 2");

    std::vector<double> params;
    params.push_back(2);
    std::cout << calc.call("f()", params) << std::endl;
    std::cout << calc.call("g()", params) << std::endl;
    std::cout << calc.get("a") << std::endl;
}

int interactive_calc(int argc, char* argv[])
{
    cCalculator calc;
	switch (argc) {
	case 1:					// read from standard input
		calc.set_input(&std::cin);
		break;
	case 2: 				// read argument string
		calc.set_input(new std::istringstream(argv[1]));
		break;
	default:
		cLogger::error("too many arguments");
		return 1;
	}

	return calc.calculate();
}

/*
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
*/
