#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <cctype>
#include <iostream>
#include <map>
#include <vector>
#include <cmath>

#include <sstream>	// string streams

class cCalculator
{
    public:
        cCalculator();
        cCalculator(std::istream * input);
        void set_input(std::istream * input);
        /** Token types, which can be encountered in line */
        enum Token_value {
            FUNCTION,   NAME,		NUMBER,		END,
            PLUS='+',	MINUS='-',	MUL='*',	DIV='/',    POW='^',
            PRINT=';',	ASSIGN='=',	LP='(',		RP=')',
            SIN=500,    COS,        TAN,
            ASIN,       ACOS,       ATAN,
            LOG,        LOG10,
            EXP,        SQRT,
        };

        std::map<std::string, double>       values;

        std::map<std::string, std::string>  functions;
        std::map<std::string, std::vector<std::string> > param_names;

        double call(std::string name, std::vector<double>);
        double get(std::string name);

        int calculate();

        int parse_line(std::string line);
        int parse_stream(std::istream & stream);
    private:
        void init();

        std::istream * input;
        Token_value curr_tok;
        double number_value;
        std::string string_value;
        std::string param_list;

        /**
            expression:
            expression + term
            expression - term
            term

            term:
            term / primary
            term * primary
            term ^ primary
            primary

            primary:
            NUMBER
            NAME
            FUNCTION
            MATH_FUNCTION primary
            NAME = expression
            - primary
            ( expression )
        */
        Token_value get_token();

        double prim(bool get);      // handle primaries
        double term(bool get);		// multiply and divide
        double expr(bool get);		// add and subtract
};

#endif // CALCULATOR_H

