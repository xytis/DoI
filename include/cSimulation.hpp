#ifndef CSIMULATION_HPP_INCLUDED
#define CSIMULATION_HPP_INCLUDED

#include "CExeptions.h"
#include "cConstants.h"
#include "cMaterial.h"
#include "tFunctors.hpp"

#include <map>
#include <string>
#include <iostream>

namespace DoI {
    class cSimulation {
        /** Ði klasë turi valdyti simuliacijà, leisti prijungti gui,
        scenarijø vykdymas irgi jos rûpestis.
        */
        private:
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
            cMaterial  * m_object;

            //Stream'as į kurį saugoma srovė
            std::ostream * current_output;

            void init();    //Funkcija susiejanti vardus su funkcijomis map'e

            void load_constants(std::ifstream &);
            void load_global(std::ifstream &);

            void set_voltage(std::ifstream &);
            void set_contacts(std::ifstream &);
            void set_output(std::ifstream &);

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

        public:
            cSimulation();
            //Scenarijus uþraðomas tekstu,
            //su nuorodomis á paprogrames ir parametrais
            //Kiekviena nuoroda yra kitaip interpretuojama, ir prie jø reikalingi spec. parametrai.
            void read_n_execute(std::string);




    };
};

#endif //CSIMULATION_HPP_INCLUDED
