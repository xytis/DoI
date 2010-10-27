#ifndef CSIMULATION_HPP_INCLUDED
#define CSIMULATION_HPP_INCLUDED

#include "CExeptions.h"
#include "cConstants.h"
#include "cMaterial.h"

#include <map>
#include <string>
#include <iostream>

namespace DoI {
    class cSimulation {
        /** �i klas� turi valdyti simuliacij�, leisti prijungti gui,
        scenarij� vykdymas irgi jos r�pestis.
        */
        private:
            //Konstant� klas�, joje vert�s nekinta per am�i�.
            //Ta�iau pati klas� gali b�ti pakeista kita klase.
            cConstants * m_constants;
            //Parametr� klas�, kurioje vyksta keitimai scenarijaus eigoje
            cGlobal    * m_global;

            //Apdorojan�i� funkcij� �em�lapis,
            //kiekviena f-ja pati nusiskaito parametrus.
            std::map<std::string, void (cSimulation::*) (std::ifstream &)> m_controlers;

            //Failo vardas � kur� saugoma srov�
            std::string current_output;

            void init();    //Funkcija susiejanti vardus su funkcijomis map'e

            void load_constants(std::ifstream &);
            void load_global(std::ifstream &);
            void set_output(std::ifstream &);
            void run_by_time(std::ifstream &);
            void end(std::ifstream &);

        public:
            cSimulation();
            //Scenarijus u�ra�omas tekstu,
            //su nuorodomis � paprogrames ir parametrais
            //Kiekviena nuoroda yra kitaip interpretuojama, ir prie j� reikalingi spec. parametrai.
            void read_n_execute(std::string);




    };
};

#endif //CSIMULATION_HPP_INCLUDED
