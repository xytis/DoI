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
#include "tFunctors.hpp"

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

            cConstants * m_constants;
            cGlobal * m_global;

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
            //Konstruktoriai
            cMaterial(cConstants *, cGlobal *);
            cMaterial(cConstants *, cGlobal *, const uint64_t & size, const cData & first, const cData & other, const cData & last);
            cMaterial(cConstants *, cGlobal *, std::vector<cData> &);

            //Funkcija leidžianti įvairiai užpildyti medžiagą dalelėmis
            void fill(mathFunction *, mathFunction *);

            //Pagrindinio ciklo žingsnis
            void run();
            //Dabartinės būsenos išvedimas į ekraną.
            void report(std::string name = "", bool hold = false);

            //Parodo kas vyksta dabar. Gali būti naudojamas komunikuoti su cInterface klase.
            void fstats(std::ostream & out = std::cout);
            void stats(sSnapshot &);
            void fcurrent(std::ostream & out = std::cout, double x_norm = 1, double y_norm = 1);

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

        class printFunction
    {
        public:
            virtual void operator()() = 0;  // call using operator
            virtual void Call() = 0;       // call using function
    };

    class iterPrint : public printFunction
    {
        private:
            std::ostream * out;
            cMaterial * object;
            double lastTime;
        public:
            iterPrint(std::ostream * n_out, cMaterial * n_object):
            out(n_out), object(n_object), lastTime(n_object->time()) {};
            void operator()()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out));
                    lastTime = object->time();
                }
            };

            void Call()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out));
                    lastTime = object->time();
                }
            };

            ~iterPrint()
            {
                out->flush();
            };
    };

    class logPrint : public printFunction
    {
        private:
            uint64_t callTimes;
            uint64_t lastPrint;
            std::ostream * out;
            cMaterial * object;
        public:
            logPrint(std::ostream * n_out, cMaterial * n_object):
            callTimes(0), lastPrint(1), out(n_out), object(n_object) {};
            void operator()()
            {
                callTimes++;
                if (lastPrint * 2 <= callTimes)
                {
                    object->fcurrent((*out));
                    lastPrint *= 2;
                }

            };
            void Call()
            {
                callTimes++;
                if (lastPrint * 2 <= callTimes)
                {
                    object->fcurrent((*out));
                    lastPrint *= 2;
                }
            };
            ~logPrint()
            {
                out->flush();
            };
    };

    class normIterPrint : public printFunction
    {
        private:
            std::ostream * out;
            cMaterial * object;
            double lastTime;
            double  x_norm;
            double  y_norm;
        public:
            normIterPrint(std::ostream * n_out, cMaterial * n_object,
                        double n_x_norm, double n_y_norm):
            lastTime(n_object->time()),
            x_norm(n_x_norm), y_norm(n_y_norm),
            out(n_out), object(n_object) {};
            void operator()()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out), x_norm, y_norm);
                    lastTime = object->time();
                }
            };

            void Call()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out), x_norm, y_norm);
                    lastTime = object->time();
                }
            };

            ~normIterPrint()
            {
                out->flush();
            };
    };

    class normLogPrint : public printFunction
    {
        private:
            double  nextTime;
            double  x_norm;
            double  y_norm;
            std::ostream * out;
            cMaterial * object;
        public:
            normLogPrint(std::ostream * n_out, cMaterial * n_object,
                        double n_x_norm, double n_y_norm):
            nextTime(n_object->time()),
            x_norm(n_x_norm), y_norm(n_y_norm),
            out(n_out), object(n_object) {};
            void operator()()
            {
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out), x_norm, y_norm);
                    nextTime = object->time()*1.01;
                }

            };
            void Call()
            {
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out), x_norm, y_norm);
                    nextTime = object->time()*1.01;
                }
            };
            ~normLogPrint()
            {
                out->flush();
            };
    };
};

#endif // CMATERIAL_H_INCLUDED
