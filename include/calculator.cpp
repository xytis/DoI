// The desk calulator

// includes character-level input (sec6.1.3) and
// command line input (sec6.1.7),
// but no namespaces and
// no exceptions

// pp 107-119, sec 6.1, A Desk Calculator


// uses += rather than push_back() for string
// to work around standard library bug

// uses istrstream from <strstream> rather than istringstream from <sstream>
// to work around standard library bug

// No guarantees offered. Constructive comments to bs@research.att.com


/*
    program:
	END			   // END is end-of-input
	expr_list END

    expr_list:
	expression PRINT	   // PRINT is semicolon
	expression PRINT expr_list

    expression:
	expression + term
	expression - term
	term

    term:
	term / primary
	term * primary
	primary

    primary:
	NUMBER
	NAME
	NAME = expression
	- primary
	( expression )
*/

#include "calculator.h"

int no_of_errors;	// note: default initialized to 0

double error(const char* s)
{
    no_of_errors++;
    std::cerr << "error: " << s << '\n';
    return 1;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

cCalculator::cCalculator():input(NULL)
{
    init();
}

cCalculator::cCalculator(std::istream * n_input):input(NULL)
{
    init();
    set_input(n_input);
}

void cCalculator::init()
{
    curr_tok = PRINT;
    values["pi"] = 3.1415926535897932385;	// insert predefined names
	values["e"] = 2.7182818284590452354;
}

void cCalculator::set_input(std::istream * n_input)
{
    input = n_input;
}

/** Main loop, for debuging, or interactive calculator */
int cCalculator::calculate()
{
    while (*input) {
		get_token();
		if (curr_tok == END) break;
		if (curr_tok == PRINT) continue;
		std::cout << expr(false) << '\n';
	}

	return no_of_errors;
}

cCalculator::Token_value cCalculator::get_token()
{
	char ch;

	do {	// skip whitespace except '\en'
		if(!input->get(ch)) return curr_tok = END;
	} while (ch!='\n' && isspace(ch));

	switch (ch) {
	case ';':
	case '\n':
		return curr_tok=PRINT;
	case '*':
	case '/':
    case '^':
	case '+':
	case '-':
	case '(':
	case ')':
	case '=':
		return curr_tok=Token_value(ch);
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	case '.':
		input->putback(ch);
		*input >> number_value;
		return curr_tok=NUMBER;
	default:			// NAME, NAME=, FUNCTION(), FUNCTION()= or error
		if (isalpha(ch) ) {
			string_value = ch;
			while (input->get(ch) && ( isalnum(ch) || ch == '_' ))
			{
				string_value += ch;	// string_value.push_back(ch);
							// to work around library bug
			}

			if (ch == '(')  //If function name found
            {
                //Check against math functions:
                if (string_value == "sin")
                {
                   input->putback(ch);
                   return curr_tok=SIN;
                }
                if (string_value == "cos")
                {
                    input->putback(ch);
                    return curr_tok=COS;
                }
                if (string_value == "tan")
                {
                    input->putback(ch);
                    return curr_tok=TAN;
                }
                if (string_value == "asin")
                {
                    input->putback(ch);
                    return curr_tok=ASIN;
                }
                if (string_value == "acos")
                {
                    input->putback(ch);
                    return curr_tok=ACOS;
                }
                if (string_value == "atan")
                {
                    input->putback(ch);
                    return curr_tok=ATAN;
                }
                if (string_value == "log")
                {
                    input->putback(ch);
                    return curr_tok=LOG;
                }
                if (string_value == "log10")
                {
                    input->putback(ch);
                    return curr_tok=LOG10;
                }
                if (string_value == "exp")
                {
                    input->putback(ch);
                    return curr_tok=EXP;
                }
                if (string_value == "sqrt")
                {
                    input->putback(ch);
                    return curr_tok=SQRT;
                }
                //Continue with user defined functions
                string_value += ch;
                param_list = "";
                while (input->get(ch) && ch != ')')
                {
                    param_list += ch;
                }
                string_value += ch; // add ')'
                return curr_tok=FUNCTION;
            }
            input->putback(ch);
            return curr_tok=NAME;
		}
		error("bad token");
		return curr_tok=PRINT;
	}
}

double cCalculator::prim(bool get)		// handle primaries
{
	if (get) get_token();

	switch (curr_tok) {
	case NUMBER:		// floating-point constant
	{	double v = number_value;
		get_token();
		return v;
	}
	case NAME:
	{
	    double& v = values[string_value];
		if (get_token() == ASSIGN) v = expr(true);
		return v;
	}
	case FUNCTION:
	{
	    std::string & f = functions[string_value];
	    std::vector<std::string> & names = param_names[string_value];
	    if (get_token() == ASSIGN)
	    {
	        std::getline(*input, f, ';');
	        names = split(param_list, ',');
	        return 0;
	    }
	    std::istream * temp = input;
        //Add current values of function params to their names in values map.
        std::vector<std::string> val = split(param_list, ',');
        if (names.size() != val.size())
        {
            return error("given parameters do not match function definition");
        }
        for (int i = 0; i < val.size(); i++)
        {
            std::stringstream t(val[i]);
            double tt;
            t >> tt; // whitespace and commas should be ignored.
            values[names[i]] = tt;
        }
	    std::stringstream formula(f);
	    input = &formula;
	    Token_value last_token = curr_tok;
	    double v = expr(true);  //Calculate it.
	    input = temp;
	    //get next token
	    curr_tok = last_token;
	    return v;
	}
	case MINUS:		// unary minus
		return -prim(true);
	case LP:
	{
	    double e = expr(true);
		if (curr_tok != RP) return error(") expected");
		get_token();		// eat ')'
		return e;
	}
	case SIN:
    {
        double e = prim(true);
        return sin(e);
    }
    case COS:
    {
        double e = prim(true);
        return cos(e);
    }
    case TAN:
    {
        double e = prim(true);
        return tan(e);
    }
    case ASIN:
    {
        double e = prim(true);
        return asin(e);
    }
    case ACOS:
    {
        double e = prim(true);
        return acos(e);
    }
    case ATAN:
    {
        double e = prim(true);
        return atan(e);
    }
    case LOG:
    {
        double e = prim(true);
        return log(e);
    }
    case LOG10:
    {
        double e = prim(true);
        return log10(e);
    }
    case EXP:
    {
        double e = prim(true);
        return exp(e);
    }
    case SQRT:
    {
        double e = prim(true);
        return sqrt(e);
    }
	default:
		return error("primary expected");
	}
}

double cCalculator::term(bool get)		// multiply and divide
{
	double left = prim(get);

	for (;;)
		switch (curr_tok) {
		case MUL:
			left *= prim(true);
			break;
		case DIV:
			if (double d = prim(true)) {
				left /= d;
				break;
			}
			return error("divide by 0");
        case POW:
            left = std::pow(left,prim(true));
            break;
		default:
			return left;
		}
}

double cCalculator::expr(bool get)		// add and subtract
{
	double left = term(get);

	for (;;)				// ``forever''
		switch (curr_tok) {
		case PLUS:
			left += term(true);
			break;
		case MINUS:
			left -= term(true);
			break;
		default:
			return left;
		}
}

double cCalculator::call(std::string name, std::vector<double> args)
{
    std::string & function = functions[name];
    std::vector<std::string> & names = param_names[name];
    if (names.size() != args.size())
    {
        return error("given parameters do not match function definition");
    }
    for (int i = 0; i < args.size(); i++)
    {
        values[names[i]] = args[i];
    }
    std::stringstream formula(function);
    set_input(&formula);
    return expr(true);  //Calculate it.
}

double cCalculator::get(std::string name)
{
    return values[name];
}

int cCalculator::parse_line(std::string line)
{
    std::istringstream stream(line);
    return parse_stream(stream);
}

int cCalculator::parse_stream(std::istream & stream)
{
    std::istream * temp = input;
    set_input(&stream);
    while (*input) {
		get_token();
		if (curr_tok == END) break;
		if (curr_tok == PRINT) continue;
		expr(false);    //Parse
	}
    set_input(temp);
	return no_of_errors;

}

double cCalculator::calculate_line(std::string line)
{
    std::istringstream stream(line);
    return calculate_stream(stream);
}

double cCalculator::calculate_stream(std::istream & stream)
{
    std::istream * temp = input;
    set_input(&stream);
    double last_result(0);
    while (*input) {
		get_token();
		if (curr_tok == END) break;
		if (curr_tok == PRINT) continue;
		last_result = expr(false);    //Parse
	}
    set_input(temp);
	return last_result;

}

