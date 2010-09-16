#include <iostream>

#include "include/cMaterial.h"
#include "include/cConstants.h"
#include "include/cData.h"

using namespace DoI;

int main()
{
    cMaterial * test = new cMaterial(readConstants("DoIconst.txt"),200);
    //cMaterial * test = new cMaterial(readConstants("/home/xytis/DoIconst.txt"), 100, cData(100000,0,0,0,0,1e-6),cData(0,0,0,0,0,1e-6),cData(0,100000,0,0,0,1e-6));

    uint64_t count = 0;
    uint64_t done = 0;

    //Current file
    std::ofstream fcurrent ("current.dat");

    while (test->time() < 5*test->m_constants->transitTime())
    {
        count ++;
        test->run();
        test->fcurrent(fcurrent);
        if ((test->time() > test->m_constants->transitTime()) && (done == 0))
        {
            test->dump("dump1.dat");
            done ++;
        }

        if ((test->time() > 4*test->m_constants->transitTime()) && (done == 1))
        {
            test->dump("dump2.dat");
            done ++;
        }

        if (count % 1000 == 0)
        {
            test->fstats();
        }

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
