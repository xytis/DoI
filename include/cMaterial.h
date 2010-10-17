/*
 * =====================================================================================
 *
 *       Filename:  cMaterial.h
 *
 *    Description:  Class witch hold the testing enviroment
 *
 *        Version:  2.0
 *        Created:  2010-04-13-21.55.26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */

#ifndef CMATERIAL_H_INCLUDED
#define CMATERIAL_H_INCLUDED

#define DEBUG
#include <cassert>

#include <cstdlib>
#include <vector>

#include <iostream>
#include <fstream>


#include "global.h"
#include "cBlock.h"
#include "cData.h"
#include "CExeptions.h"

namespace DoI
{
    struct sSnapshot
    {
        double cur_time;
        uint64_t array_size;
        cData * array;
        double current;
    };

    class cMaterial
    {
        private:
            std::vector<IBlock *>  m_blockArray;

            std::vector<cData> m_backupArray;



            double m_time;
            double m_backup_time;

            void init(const uint64_t &);

            void link(IBlock *, IBlock *);

            void clock_tick();

            void changeTimeInterval(int);

            void backup();
            void restore();

            void flush();
            void calc();



        public:
            cConstants * m_constants;

            cMaterial(cConstants *, const uint64_t & size);
            cMaterial(cConstants *, const uint64_t & size, const cData & first, const cData & other, const cData & last);
            cMaterial(cConstants *, std::vector<cData> &);
            void run();
            void report(std::string name = "", bool hold = false);

            //Parodo kas vyksta dabar. Gali būti naudojamas komunikuoti su cInterface klase.
            void fstats(std::ostream & out = std::cout);
            void stats(sSnapshot &);
            void fcurrent(std::ostream & out = std::cout);
            double current();

            //Pasiunčia visą duomenų masyvą lauk į failą (id::string)
            void dump(std::string);
            //Parsigabeną visą duomenų masyvą iš formatuoto failo.
            void load(std::string);

            double time();

            #ifdef DEBUG
            //Jei neišeina dviem būdais vienodai suiteruoti masyvų, nutraukia veiklą.
            void check_link();
            //Patikrina ar veikia backup() ir restore()
            void check_backup();
            //Patikrina ar veikia flush() ir calc()
            void check_run();
            #endif //DEBUG
    };
};

#endif // CMATERIAL_H_INCLUDED