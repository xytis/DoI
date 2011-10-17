#include <iostream>
#include <iomanip>
#include <sstream>

#include "include/cMaterial.h"
#include "include/cConstants.h"
#include "include/cData.h"
#include "include/tFunctors.h"

#include "include/cSimulation.h"

//#define DUMP

using namespace DoI;

int main(int argc, char * argv[])
{
    cSimulation * sim = new cSimulation();
    try {
        if (argc>1)
        {
            std::string filename(argv[1]);
            sim->read_n_execute(filename);
        }
        else
        {
            sim->read_n_execute("scenario.txt");
        }
    }
    catch (exception::FileMisingExeption & e)
    {
        std::cout << "E>" << e.what() << std::endl;
    }
    catch (exception::FileBadExeption & e)
    {
        std::cout << "E>" << e.what() << std::endl;
    }
    catch (exception::BadCommand & e)
    {
        std::cout << "E>" << e.what() << ": " << e.reason() << std::endl;
    }

    return 0;

//    cMaterial * test = new cMaterial(readConstants("DoIconst.txt"),500);
/*
    cMaterial * test = new cMaterial(readConstants("DoIconst.txt"), 2000,
    cData(1e10,0,0,0,0,5e-09),
    cData(0,0,0,0,0,5e-09),
    cData(0,1e10,0,0,0,5e-09));
*/
/*
    uint64_t count = 0;
    uint64_t done = 0;

    //Current file
    std::ofstream fcurrent ("current.dat");

*/
    /**Diskretus ciklas*/
/*
    uint64_t loop_count = 1000000;
    uint64_t number = 1;

    while (count < loop_count)
    {
        if (count % 10000 == 0)
        {
            #ifdef DUMP
            std::ostringstream temp;
            temp << "dumps/material" << std::setfill('0') << std::setw(3) << number << ".dat";
            test->write_material(temp.str());
            temp.str("");
            temp << "dumps/field" << std::setfill('0') << std::setw(3) << number << ".dat";
            test->write_field(temp.str());
            #endif //DUMP
            std::cout << "TIME: " << test->time() << '\t';
            std::cout << "PERCENT: " << count*100.0/loop_count <<std::endl;
            number ++;
        }

        count ++;
        test->run();
    }
*/

    /**Laikinis ciklas*/
/*
    double transit_cycles = 7;
    double transit_time = test->m_constants->transitTime();

    uint64_t output_file_num = 100;

    double interval = transit_cycles*transit_time/output_file_num;
    double log_inc = 1.00001; //1 kas kiekvieną ciklą įrašyti.
    //Pakankamas tikslumas 1.001, geras tikslumas 1.00001, na ir tobula 1, bet failai didžiuliai.

    double log_cummulative = 1;
    double last_time = 0;

    uint64_t number = 0;

    std::cout << "BEGIN" << std::endl;
    std::cout << "T_tr: " << transit_time << std::endl;

    while (test->time() < transit_cycles*transit_time)
    {
        if (test->time() > number*interval)
        {
            #ifdef DUMP
            std::ostringstream temp;
            temp << "dumps/material" << std::setfill('0') << std::setw(3) << number << ".dat";
            test->write_material(temp.str());
            temp.str("");
            temp << "dumps/field" << std::setfill('0') << std::setw(3) << number << ".dat";
            test->write_field(temp.str());
            #endif //DUMP
            std::cout << "TIME: " << test->time() << '\t';
            std::cout << "PERCENT: " << number*100.0/output_file_num << std::endl;
            number ++;
        }

        //Logaritminė srovė
        if (test->time() > last_time*log_cummulative)
        {
            test->fcurrent(fcurrent);
            log_cummulative *= log_inc;
            last_time = test->time();
        }

        test->run();
    }

    std::cout << "END" << std::endl;

    /*
    test->dump("dump.txt");
    test->check_run();
    test->load("dump.txt");
    test->report();
    */
    /*
    test->check_link();
    test->check_backup();
    test->check_run();
    */
/*
    return 0;
    */
}
