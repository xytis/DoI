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
            //Pagrindiniai darbiniai masyvai
            std::vector<IBlock *>  m_blockArray;
            std::vector<cField *>  m_fieldArray;
            //Pagalbinis duomenų masyvas, be link'ų
            std::vector<cData> m_backupArray;
            //Laiko kintamieji
            double m_time;
            double m_backup_time;

            //Pagrindinių masyvu inicijavimo f-ja
            //Sukuria link'us naudodamasi pagalbinėmis f-jos
            void init(const uint64_t &);
            //Pagalbinės init f-jos
            void link(IBlock *, IBlock *);
            void surround(cField *, IBlock *, cField *);

            //Pagrindinio ciklo pagalbinės f-jos
            //Laikrodukas
            void clock_tick();
            //Duomenų mainams su backup masyvu
            void backup();
            void restore();
            //Buferiuotų dalelių įrašymas į pagrindines
            void flush();
            //Skaičiavimai.
            void calc();
            //Laiko žingsnio didinimas\mažinimas, su gylio saugojimu.
            void changeTimeInterval(int);



        public:
            //Kai tik main neliks nieko valdančio, ji pereis į privačią sritį.
            //Tik cInterface galės valdyti. Ir tik netiesiogiai.
            cConstants * m_constants;

            //Konstruktoriai
            cMaterial(cConstants *, const uint64_t & size);
            cMaterial(cConstants *, const uint64_t & size, const cData & first, const cData & other, const cData & last);
            cMaterial(cConstants *, std::vector<cData> &);

            //Pagrindinio ciklo žingsnis
            void run();
            //Dabartinės būsenos išvedimas į ekraną.
            void report(std::string name = "", bool hold = false);

            //Parodo kas vyksta dabar. Gali būti naudojamas komunikuoti su cInterface klase.
            void fstats(std::ostream & out = std::cout);
            void stats(sSnapshot &);
            void fcurrent(std::ostream & out = std::cout);

            //Pasiunčia visą duomenų masyvą lauk į failą (id::string)
            void write_material(std::string);
            void write_field(std::string);
            //Parsigabeną visą duomenų masyvą iš formatuoto failo.
            void load(std::string);
            //Sumeta visą duomenų masyvą į formatuotą failą.
            void dump(std::string);

            //Vidinių parametrų rodymui:
            double current();
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
