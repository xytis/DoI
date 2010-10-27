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
        /** Ði klasë turi valdyti simuliacijà, leisti prijungti gui,
        scenarijø vykdymas irgi jos rûpestis.
        */
        private:
            //Konstantø klasë, joje vertës nekinta per amþiø.
            //Taèiau pati klasë gali bûti pakeista kita klase.
            cConstants * m_constants;
            //Parametrø klasë, kurioje vyksta keitimai scenarijaus eigoje
            cGlobal    * m_global;

            //Apdorojanèiø funkcijø þemëlapis,
            //kiekviena f-ja pati nusiskaito parametrus.
            std::map<std::string, void (cSimulation::*) (std::ifstream &)> m_controlers;

            //Failo vardas á kurá saugoma srovë
            std::string current_output;

            void init();    //Funkcija susiejanti vardus su funkcijomis map'e

            void load_constants(std::ifstream &);
            void load_global(std::ifstream &);
            void set_output(std::ifstream &);
            void run_by_time(std::ifstream &);
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
