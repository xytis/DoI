#ifndef CPARSER_H_INCLUDED
#define CPARSER_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <stack>

#include "cLogger.h"

#include "global.h"

#include "cSimulation.h"

#define PARSER_INCLUDE "INCLUDE"
#define PARSER_END "END"

#define MAINPARSER_SIMULATION "SIMULATION"
#define MAINPARSER_ENVIRONMENT "ENVIRONMENT"
#define MAINPARSER_PRINTER "PRINTER"
#define MAINPARSER_MATERIAL "MATERIAL"
#define MAINPARSER_SCENARIO "SCENARIO"

#define ENVIRONMENTPARSER_CONSTANT_BETA "beta"
#define ENVIRONMENTPARSER_CONSTANT_AREA "S"
#define ENVIRONMENTPARSER_CONSTANT_MIN "MIN"
#define ENVIRONMENTPARSER_CONSTANT_CHARGE "q"
#define ENVIRONMENTPARSER_CONSTANT_EPSILON "eps"
#define ENVIRONMENTPARSER_CONSTANT_EPSILON_0 "eps0"
#define ENVIRONMENTPARSER_CONSTANT_DRIFT "k"
#define ENVIRONMENTPARSER_CONSTANT_TEMPERATURE "T"
#define ENVIRONMENTPARSER_CONSTANT_MOBILITY_N "n_miu"
#define ENVIRONMENTPARSER_CONSTANT_MOBILITY_P "p_miu"
#define ENVIRONMENTPARSER_CONSTANT_DIFFUSION_N "n_D"
#define ENVIRONMENTPARSER_CONSTANT_DIFFUSION_P "p_D"
#define ENVIRONMENTPARSER_CONSTANT_GLUE "k_glue"
#define ENVIRONMENTPARSER_TIME_STEP "TIME_STEP"
#define ENVIRONMENTPARSER_SPACE_DIVISION "SPACE_DIVISION"
#define ENVIRONMENTPARSER_WIDTH "WIDTH"
#define ENVIRONMENTPARSER_CONTACTS "CONTACTS"
#define ENVIRONMENTPARSER_CAPACITY_N "cap_n"
#define ENVIRONMENTPARSER_CAPACITY_P "cap_p"

namespace DoI
{
    class cParser;

    ///WARNING: Multithreading pitfal goes below!
    class cParserStack
    {
        public:
            static cParserStack * instance();
            cParser * current;
            void push(cParser *);
            cParser * pop();
        private:
            std::stack<cParser *> m_stack;

            static cParserStack * s_instance;

            cParserStack();
            ~cParserStack() {};
    };

    /** Class, which is used as an iterface to parse config files for
    diferent classes.
    Binds INCLUDE statement.
    */
    class cParser
    {
        public:
            cParser(cParser *);
            bool include(std::stringstream & param);
            bool end(std::stringstream & param);
            bool close(std::stringstream & param);
            bool resume(std::stringstream & param);
            bool parse(const std::string & name);
            bool comment(std::string & word);

            virtual void report(cParser * child, cObject * object) = 0;

            cObject * object();
        protected:
            cParser * m_parent;
            bool parseLine(std::string & line);
            bool parseFile(std::istream & file);
            std::map<std::string, bool (cParser::*) (std::stringstream &)> m_actions;
            cObject * m_object;
        private:
    };

    class cMainParser : public cParser
    {
        public:
            cMainParser();
            bool simulation(std::stringstream & param);
    		bool environment(std::stringstream & param);
    		bool printer(std::stringstream & param);
    		bool end(std::stringstream & param);
    		void report(cParser * child, cObject * object);
            ~cMainParser() {};
        protected:
        private:
            cEnvironment * m_environment;
            cPrinter * m_printer;
            cSimulation * m_real_object;
            std::string m_last;

    };

    class cEnvironmentParser : public cParser
    {
    	public:
    		cEnvironmentParser(cParser * parent);
    		void report(cParser * child, cObject * object) {};
    		bool end(std::stringstream & param);
    		///Constants
    		bool beta(std::stringstream & params);
            bool area(std::stringstream & params);
            bool min(std::stringstream & params);
            bool charge(std::stringstream & params);
            bool epsilon(std::stringstream & params);
            bool epsilon_0(std::stringstream & params);
            bool drift(std::stringstream & params);
            bool temperature(std::stringstream & params);
            bool mobility_n(std::stringstream & params);
            bool mobility_p(std::stringstream & params);
            bool diffusion_n(std::stringstream & params);
            bool diffusion_p(std::stringstream & params);
            bool glue(std::stringstream & params);
            ///Global
            bool time_step(std::stringstream & params);
            bool space_division(std::stringstream & params);
            bool width(std::stringstream & params);
            bool contacts(std::stringstream & params);
            bool capacity_n(std::stringstream & params);
            bool capacity_p(std::stringstream & params);
            ~cEnvironmentParser() {};
    	protected:

    	private:
            ///Constants
            double m_beta;
            double m_area;
            double m_min;
            double m_charge;
            double m_epsilon;
            double m_epsilon_0;
            double m_drift;
            double m_temperature;
            double m_mobility_n;
            double m_mobility_p;
            double m_diffusion_n;
            double m_diffusion_p;
            double m_glue;
            ///Global
            double m_time_step;
            uint64_t m_space_division;
            double m_width;
            CONTACTS_TYPE m_contacts;
            double m_capacity_n;
            double m_capacity_p;

    };

    class cPrinterParser : public cParser
    {
        public:
            cPrinterParser(cParser * parent);
            void report(cParser * child, cObject * object) {};
    };

    class cScenarioParser : public cParser
    {
        public:
            cScenarioParser(cParser * parent);
            void report(cParser * child, cObject * object) {};
    };
}
#endif // CPARSER_H_INCLUDED
