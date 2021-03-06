#ifndef CPARSER_H_INCLUDED
#define CPARSER_H_INCLUDED

#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <queue>
#include <stack>

#include "cLogger.h"

#include "global.h"

#include "cSimulation.h"
#include "calculator.h"
#include "tFunctors.h"

#define PARSER_INCLUDE "INCLUDE"
#define PARSER_END "END"
#define PARSER_MATH "%"
#define PARSER_ECHO "ECHO"

#define MAINPARSER_SIMULATION "SIMULATION"
#define MAINPARSER_ENVIRONMENT "ENVIRONMENT"
#define MAINPARSER_PRINTER "PRINTER"
#define MAINPARSER_MATERIAL "MATERIAL"
#define MAINPARSER_RUN "RUN"

#define ENVIRONMENTPARSER_CONSTANT_BETA "beta"
#define ENVIRONMENTPARSER_CONSTANT_BETA_FACTOR "beta_factor"
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
#define ENVIRONMENTPARSER_TIME_OFFSET "TIME_OFFSET"
#define ENVIRONMENTPARSER_TIME_STEP "TIME_STEP"
#define ENVIRONMENTPARSER_SPACE_DIVISION "SPACE_DIVISION"
#define ENVIRONMENTPARSER_WIDTH "WIDTH"
#define ENVIRONMENTPARSER_CONTACTS "CONTACTS"
#define ENVIRONMENTPARSER_CAPACITY_N "cap_n"
#define ENVIRONMENTPARSER_CAPACITY_P "cap_p"

#define MATERIALPARSER_INITIAL "INITIAL"
#define MATERIALPARSER_DIRAC "DIRAC"

#define PRINTERPARSER_TYPE "TYPE"
#define PRINTERPARSER_MODE "MODE"
#define PRINTERPARSER_NORM "NORM"
#define PRINTERPARSER_FILENAME "FILENAME"

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
    Binds % (calculator).
    */
    class cParser
    {
        public:
            cParser(cParser *);
            bool include(std::stringstream & param);
            bool end(std::stringstream & param);
            bool math(std::stringstream & param);
            bool echo(std::stringstream & param);
            bool close(std::stringstream & param);
            bool resume(std::stringstream & param);
            bool parse(const std::string & name);
            bool comment(std::string & word);

            virtual void report(cParser * child, cObject * object) = 0;

            cObject * object();
        protected:
            cCalculator m_calculator;
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
    		bool material(std::stringstream & param);
    		bool printer(std::stringstream & param);

    		bool run(std::stringstream & param);

    		bool end(std::stringstream & param);
    		void report(cParser * child, cObject * object);
            ~cMainParser() {};
        protected:
        private:
            cEnvironment * m_environment;
            cPrinter * m_printer;
            cMaterial * m_material;
            std::queue<sOperation> * m_todo;
            cSimulation * m_real_object;
            std::string m_last;

    };

    /**
     * DONE
     */
    class cEnvironmentParser : public cParser
    {
    	public:
    		cEnvironmentParser(cParser * parent);
    		void report(cParser * child, cObject * object) {};
    		bool end(std::stringstream & param);
    		///Constants
    		bool beta(std::stringstream & params);
    		bool beta_factor(std::stringstream & params);
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
            bool time_offset(std::stringstream & params);
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
            double m_beta_factor;
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
            double m_time_offset;
            uint64_t m_space_division;
            double m_width;
            CONTACTS_TYPE m_contacts;
            double m_capacity_n;
            double m_capacity_p;

    };


    /**
    DONE
    */
    class cPrinterParser : public cParser
    {
        public:
            cPrinterParser(cParser * parent, cMaterial * object);
            void report(cParser * child, cObject * object) {};

            bool type(std::stringstream & params);
            bool mode(std::stringstream & params);
            bool norm(std::stringstream & params);
            bool filename(std::stringstream & params);
            bool end(std::stringstream & params);
        private:
            enum TYPE
            {
                NONE,
                LOG,
                TIMER
            };

            cMaterial * m_test_object;
            TYPE m_type;
            MODE m_mode;
            double m_norm_x;
            double m_norm_y;
            std::string m_filename;
            double m_increment;
    };

    /**
    DONE
    */
    class cMaterialParser : public cParser
    {
        public:
            cMaterialParser(cParser * parent, cEnvironment *);
            void report(cParser * child, cObject * object) {};

            bool initial(std::stringstream & params);
            bool dirac_initial(std::stringstream & params);
            bool end(std::stringstream & params);
        private:
            cMaterial * m_real_object;
    };
}
#endif // CPARSER_H_INCLUDED
