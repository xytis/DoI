#include <iostream>
#include <iomanip>
#include <sstream>

#include "include/cMaterial.h"
#include "include/cConstants.h"
#include "include/cData.h"


//#include "include/cInterface.h"

using namespace DoI;

int main(int argc, char * argv[])
{
/*
    cInterface * interface = new cInterface();
    interface->init(&argc, &argv);
    interface->connect(new cMaterial(readConstants("DoIconst.txt"),200));
    return interface->lounch();
*/
    cMaterial * test = new cMaterial(readConstants("DoIconst.txt"),200);
/*
    cMaterial * test = new cMaterial(readConstants("DoIconst.txt"), 2000,
    cData(1e10,0,0,0,0,5e-09),
    cData(0,0,0,0,0,5e-09),
    cData(0,1e10,0,0,0,5e-09));
*/
    uint64_t count = 0;
    uint64_t done = 0;

    //Current file
    std::ofstream fcurrent ("current.dat");

    /*
    test->run();
    test->dump("dump1.dat");
    test->run();
    test->dump("dump2.dat");
    test->fstats();
    */
    uint64_t loop_count = 10000;

    while (count < loop_count)//(test->time() < 5*test->m_constants->transitTime())
    {
        if (count % 100 == 0)
        {
            std::ostringstream temp;
            temp << "dumps/material" << std::setfill('0') << std::setw(3) << count/100 << ".dat";
            test->write_material(temp.str());
            temp.str("");
            temp << "dumps/field" << std::setfill('0') << std::setw(3) << count/100 << ".dat";
            test->write_field(temp.str());

            std::cout << "PERCENT: " << double(count)/loop_count <<std::endl;
        }

        count ++;
        test->run();
/*
        if ((test->time() > done*test->m_constants->transitTime()/10))
        {
            std::ostringstream temp;
            temp << "dumps/material" << done << ".dat";
            test->write_material(temp.str());
            temp.str("");
            temp << "dumps/field" << done << ".dat";
            test->write_field(temp.str());
            done ++;
        }

        if (count % 100 == 0)
        {
            //test->report();
            std::cout << "PERCENT: " << (test->time() / (5*test->m_constants->transitTime()))*100 << std::endl;
            //test->fstats();

        }
*/

    }

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

    return 0;
}
