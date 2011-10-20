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

            //Konstantų klasė, joje vertės nekinta per amžių.
            //Tačiau pati klasė gali būti pakeista kita klase.
            cConstants * m_constants;
            //Parametrų klasė, kurioje vyksta keitimai scenarijaus eigoje
            cGlobal    * m_global;

            //Apdorojančių funkcijų žemėlapis,
            //kiekviena f-ja pati nusiskaito parametrus.
            std::map<std::string, void (cSimulation::*) (std::ifstream &)> m_controlers;

            //Viena simuliacija turi tik vieną medžiagą. Žinoma, niekas netrukdo
            //viduryje simuliacijos ją pakeisti kita.
            //cMaterial  * m_object;

            //Funkcija srovės print'ui
            cPrinter * m_current_output;

            void init();    //Funkcija susiejanti vardus su funkcijomis map'e

            void load_constants(std::ifstream &);
            void load_global(std::ifstream &);

            void set_voltage(std::ifstream &);
            void set_contacts(std::ifstream &);
            void set_output(std::ifstream &);
            void set_time_step(std::ifstream &);

            void create_object(std::ifstream &);
            void load_object(std::ifstream &);
            void save_object(std::ifstream &);

            void foto_injection(std::ifstream &);

            void run_by_transit(std::ifstream &);
            void run_until(std::ifstream &);
            void run_iter(std::ifstream &);

            void doi(std::ifstream &);
            void stable(std::ifstream &);
            void celiv(std::ifstream &);

            void pause(std::ifstream &);
            void end(std::ifstream &);

            //HELPERS
            void check_init();
            void do_iter();
            void do_until(double time);

            bool check_and_discard_comment(std::string &, std::istream &);

        public:
            ///Constructor which creates the object
            cSimulation();
            ///Main function, which controls the flow
            void run();
            ///Validation function, checks if run method is ok to run
            bool validityCheck();
/*
            //Scenarijus uþraðomas tekstu,
            //su nuorodomis á paprogrames ir parametrais
            //Kiekviena nuoroda yra kitaip interpretuojama, ir prie jø reikalingi spec. parametrai.
            void read_n_execute(std::string);
*/



    };
}

#endif //CSIMULATION_HPP_INCLUDED
