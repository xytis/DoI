/*
 * =====================================================================================
 *
 *       Filename:
 *
 *    Description:
 *
 *        Version:
 *        Created:  2011-10-18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
 #include "cParser.h"

void trimString(std::string & str, char * delimiters)
{
    // Trim Both leading and trailing delimiters
    size_t startpos = str.find_first_not_of(delimiters);
    size_t endpos = str.find_last_not_of(delimiters);

    // if left empty return an empty string
    if(( std::string::npos == startpos ) || ( std::string::npos == endpos))
    {
        str = "";
    }
    else
        str = str.substr( startpos, endpos-startpos+1 );
}

void trimSpaces( std::string& str)
{
    return trimString(str, " \t\n");
}

std::string inlineTrimSpaces( const std::string& str)
{
    std::string temp = str;
    trimSpaces(temp);
    return temp;
}

void trimQuotes( std::string& str)
{
    return trimString(str, "\'\"");
}

namespace DoI
{
    cParserStack * cParserStack::s_instance = 0;

    cParserStack::cParserStack():
    current(NULL)
    {

    }

    cParserStack * cParserStack::instance()
    {
        if (!s_instance)
            s_instance = new cParserStack();
        return s_instance;
    }

    cParser * cParserStack::pop()
    {
        cParser * last = current;
        m_stack.pop();
        current = m_stack.top();
        return last;
    }

    void cParserStack::push(cParser * p)
    {
        current = p;
        m_stack.push(p);
    }
    /*
        cParser
    */
    cParser::cParser(cParser * parent):m_parent(parent), m_object(NULL)
    {
        cParserStack::instance()->push(reinterpret_cast<cParser *>(this));
        m_actions[PARSER_INCLUDE] = &cParser::include;
        m_actions[PARSER_END] = &cParser::end;
        m_actions[PARSER_MATH] = &cParser::math;
        m_actions[PARSER_ECHO] = &cParser::echo;
    }

    bool cParser::include(std::stringstream & params)
    {
        std::string filename;
        std::getline(params, filename);
        trimSpaces(filename);
        trimQuotes(filename);
        return parse(filename);
    }

    bool cParser::end(std::stringstream & params)
    {
        cParserStack::instance()->pop();
        if (m_parent)
        {
            m_parent->report(reinterpret_cast<cParser *>(this), m_object);
        }
        return true;
    }

    bool cParser::math(std::stringstream & params)
    {
        return (m_calculator.parse_stream(params) == 0);
    }

    bool cParser::echo(std::stringstream & params)
    {
        std::cout << params.str() << std::endl;
        std::cout << m_calculator.calculate_stream(params) << std::endl;
        return true;
    }

    bool cParser::parse(const std::string & filename)
    {
        std::ifstream file(filename.c_str());
        if (file)
            return cParserStack::instance()->current->parseFile(file);
        else
            throw exception::FileMisingExeption(filename);
    }

    bool cParser::comment(std::string & word)
    {
        if (word.at(0) == '#')
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    cObject * cParser::object()
    {
        if (m_object)
        {
            return m_object;
        }
        else
        {
            throw exception::NoObject();
        }
    }

    bool cParser::parseLine(std::string & line)
    {
        std::stringstream reader(line);
        std::string name;
        reader >> name;

        if (!name.empty() and !comment(name))
        {
            std::map<std::string, bool (cParser::*) (std::stringstream &)>::iterator act;
            act = m_actions.find(name);
            //If line not understood, give warning:
            if (act == m_actions.end())
            {
                cLogger::warning(std::string("Line ") + line + std::string(" not understood, ignoring."));
                return false;
            }
            bool (cParser::*function) (std::stringstream &) = act->second;
            if (!(*this.*function) (reader))
            {
                cLogger::warning(std::string("Line ") + line + std::string(" parsing failed."));
                return false;
            }
            return true;
        }
        else
        {
            return true;
        }

    }

    bool cParser::parseFile(std::istream & file)
    {
        std::string line;
        while(getline(file, line))
        {
            cParserStack::instance()->current->parseLine(line);
        }
        return true;
    }

    /*
        cMainParser
    */
    cMainParser::cMainParser():cParser(NULL),m_environment(NULL),m_printer(NULL),m_material(NULL)
    {
        //rewrite end sequence
        m_actions[PARSER_END] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::end);

        m_actions[MAINPARSER_SIMULATION] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::simulation);
        m_actions[MAINPARSER_ENVIRONMENT] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::environment);
        m_actions[MAINPARSER_PRINTER] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::printer);
        m_actions[MAINPARSER_MATERIAL] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::material);
        m_actions[MAINPARSER_RUN] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::run);

    }

    bool cMainParser::simulation(std::stringstream & params)
    {
        return true;
    }

    bool cMainParser::environment(std::stringstream & params)
    {
        cEnvironmentParser * parser = new cEnvironmentParser(this);
        m_last = MAINPARSER_ENVIRONMENT;
        return true;
    }

    bool cMainParser::printer(std::stringstream & params)
    {
        cPrinterParser * parser = new cPrinterParser(this, m_material);
        m_last = MAINPARSER_PRINTER;
        return true;
    }

    bool cMainParser::run(std::stringstream & params)
    {
        if (!m_todo)
        {
            m_todo = new std::queue<sOperation>;
        }
        std::string scheme, temp;
        params >> scheme;
        std::stringstream parsed;
        while (params >> temp)
        {
            parsed << m_calculator.calculate_line(temp) << ' ';
        }
        sOperation t;
        t.name = scheme;
        t.params = parsed.str();
        m_todo->push(t);
    }

    bool cMainParser::material(std::stringstream & params)
    {
        cMaterialParser * parser = new cMaterialParser(this, m_environment);
        m_last = MAINPARSER_MATERIAL;
        return true;
    }

    bool cMainParser::end(std::stringstream & params)
    {
        m_real_object = new cSimulation(m_environment, m_material, m_printer, m_todo);
        if (m_real_object->validityCheck())
        {
            m_object = reinterpret_cast<cObject *>(m_real_object);
            return true;
        }
        else
        {
            throw exception::NoObject();
        }
    }

    void cMainParser::report(cParser * child, cObject * object)
    {
        //Lets find out what we called last time:
        if (m_last == MAINPARSER_ENVIRONMENT)
        {
            m_environment = reinterpret_cast<cEnvironment *>(object);
            //m_real_object.set_environment(m_environment);
        }
        if (m_last == MAINPARSER_PRINTER)
        {
            m_printer = reinterpret_cast<cPrinter *>(object);
            //m_real_object.set_printer(m_printer);
        }

        if (m_last == MAINPARSER_MATERIAL)
        {
            m_material = reinterpret_cast<cMaterial *>(object);
            if (m_printer)
                m_printer->m_object = m_material;
            //m_real_object.set_material(m_material);
        }

        /*
        if (m_last == MAINPARSER_PROCEDURE)
        {
            m_procedure = reinterpret_cast<cProcedure *>(object);
        }
        */
    }

    /*
        cEnvironmentParser
    */
    cEnvironmentParser::cEnvironmentParser(cParser * parent):cParser(parent),m_time_offset(0), m_beta_factor(1)
    {
        //rewrite end sequence
        m_actions[PARSER_END] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::end);

        m_actions[ENVIRONMENTPARSER_CONSTANT_BETA] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::beta);
        m_actions[ENVIRONMENTPARSER_CONSTANT_BETA_FACTOR] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::beta_factor);
        m_actions[ENVIRONMENTPARSER_CONSTANT_AREA] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::area);
        m_actions[ENVIRONMENTPARSER_CONSTANT_MIN] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::min);
        m_actions[ENVIRONMENTPARSER_CONSTANT_CHARGE] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::charge);
        m_actions[ENVIRONMENTPARSER_CONSTANT_EPSILON] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::epsilon);
        m_actions[ENVIRONMENTPARSER_CONSTANT_EPSILON_0] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::epsilon_0);
        m_actions[ENVIRONMENTPARSER_CONSTANT_DRIFT] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::drift);
        m_actions[ENVIRONMENTPARSER_CONSTANT_TEMPERATURE] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::temperature);
        m_actions[ENVIRONMENTPARSER_CONSTANT_MOBILITY_N] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::mobility_n);
        m_actions[ENVIRONMENTPARSER_CONSTANT_MOBILITY_P] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::mobility_p);
        m_actions[ENVIRONMENTPARSER_CONSTANT_DIFFUSION_N] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::diffusion_n);
        m_actions[ENVIRONMENTPARSER_CONSTANT_DIFFUSION_P] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::diffusion_p);
        m_actions[ENVIRONMENTPARSER_CONSTANT_GLUE] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::glue);

        m_actions[ENVIRONMENTPARSER_TIME_STEP] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::time_step);
        m_actions[ENVIRONMENTPARSER_TIME_OFFSET] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::time_offset);
        m_actions[ENVIRONMENTPARSER_SPACE_DIVISION] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::space_division);
        m_actions[ENVIRONMENTPARSER_WIDTH] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::width);
        m_actions[ENVIRONMENTPARSER_CONTACTS] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::contacts);
        m_actions[ENVIRONMENTPARSER_CAPACITY_N] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::capacity_n);
        m_actions[ENVIRONMENTPARSER_CAPACITY_P] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::capacity_p);

    }

    bool cEnvironmentParser::end(std::stringstream & params)
    {
        cConstants * constants = new cConstants(m_beta,
                                              m_area,
                                              m_min,
                                              m_charge,
                                              m_epsilon,
                                              m_epsilon_0,
                                              m_drift,
                                              m_temperature,
                                              m_mobility_n,
                                              m_mobility_p,
                                              m_diffusion_n,
                                              m_diffusion_p,
                                              m_glue);
        m_object = reinterpret_cast<cObject *>(new cEnvironment(constants, m_time_step, m_space_division, m_width, m_contacts, m_capacity_n, m_capacity_p, m_time_offset));
        return cParser::end(params);
    }

    bool cEnvironmentParser::beta(std::stringstream & params)
    {
        std::string temp;
        params >> temp;
        if (inlineTrimSpaces(temp) == "none")
        {
            m_beta = physics::recombinationConst(m_charge, m_mobility_n, m_mobility_p, m_epsilon, m_epsilon_0) * m_beta_factor;
            std::stringstream ss;
            ss << "Calculating beta from curently known values. Please check: " << m_beta;
            cLogger::warning(ss.str());
            return true;
        }
        else
        {
            std::stringstream ss;
            ss << temp;
            return ss >> m_beta;
        }
    }

    bool cEnvironmentParser::beta_factor(std::stringstream & params)
    {
        params >> m_beta_factor;
        return true;
    }

    bool cEnvironmentParser::area(std::stringstream & params)
    {
        params >> m_area;
        return true;
    }

    bool cEnvironmentParser::min(std::stringstream & params)
    {
        return params >> m_min;
    }

    bool cEnvironmentParser::charge(std::stringstream & params)
    {
        return params >> m_charge;
    }

    bool cEnvironmentParser::epsilon(std::stringstream & params)
    {
        return params >> m_epsilon;
    }

    bool cEnvironmentParser::epsilon_0(std::stringstream & params)
    {
        return params >> m_epsilon_0;
    }

    bool cEnvironmentParser::drift(std::stringstream & params)
    {
        std::string temp;
        params >> temp;
        if (inlineTrimSpaces(temp) == "none")
        {
            m_drift = physics::driftConst(m_epsilon, m_epsilon_0, m_area);
            std::stringstream ss;
            ss << "Calculating drift constant from curently known values. Please check: " << m_drift;
            cLogger::warning(ss.str());
            return true;
        }
        else
        {
            std::stringstream ss;
            ss << temp;
            return ss >> m_drift;
        }
    }

    bool cEnvironmentParser::temperature(std::stringstream & params)
    {
        return params >> m_temperature;
    }

    bool cEnvironmentParser::mobility_n(std::stringstream & params)
    {
        return params >> m_mobility_n;
    }

    bool cEnvironmentParser::mobility_p(std::stringstream & params)
    {
        return params >> m_mobility_p;
    }

    bool cEnvironmentParser::diffusion_n(std::stringstream & params)
    {
        std::string temp;
        params >> temp;
        if (inlineTrimSpaces(temp) == "none")
        {
            m_diffusion_n = physics::diffusionConst(m_temperature, m_mobility_n, m_charge);
            std::stringstream ss;
            ss << "Calculating diffusion N coeficient from curently known values. Please check: " << m_diffusion_n;
            cLogger::warning(ss.str());
            return true;
        }
        else
        {
            std::stringstream ss;
            ss << temp;
            return ss >> m_diffusion_n;
        }
    }

    bool cEnvironmentParser::diffusion_p(std::stringstream & params)
    {
        std::string temp;
        params >> temp;
        if (inlineTrimSpaces(temp) == "none")
        {
            m_diffusion_p = physics::diffusionConst(m_temperature, m_mobility_p, m_charge);
            std::stringstream ss;
            ss << "Calculating diffusion P coeficient from curently known values. Please check: " << m_diffusion_p;
            cLogger::warning(ss.str());
            return true;
        }
        else
        {
            std::stringstream ss;
            ss << temp;
            return ss >> m_diffusion_p;
        }
    }

    bool cEnvironmentParser::glue(std::stringstream & params)
    {
        return params >> m_glue;
    }

    bool cEnvironmentParser::time_step(std::stringstream & params)
    {
        return params >> m_time_step;
    }

    bool cEnvironmentParser::time_offset(std::stringstream & params)
    {
        return params >> m_time_offset;
    }

    bool cEnvironmentParser::space_division(std::stringstream & params)
    {
        return params >> m_space_division;
    }

    bool cEnvironmentParser::width(std::stringstream & params)
    {
        return params >> m_width;
    }

    bool cEnvironmentParser::contacts(std::stringstream & params)
    {
        std::string verbal;
        params >> verbal;
        if (verbal == "BLOCKING")
        {
            m_contacts = BLOCKING;
            return true;
        }
        if (verbal == "INJECTING")
        {
            m_contacts = INJECTING;
            return true;
        }
        if (verbal == "EXTRACTING")
        {
            m_contacts = EXTRACTING;
            return true;
        }
        if (verbal == "NON_BLOCKING")
        {
            m_contacts = NON_BLOCKING;
            return true;
        }
        if (verbal == "ESCAPE_LEFT")
        {
            m_contacts = ESCAPE_LEFT;
            return true;
        }
        if (verbal == "ESCAPE_RIGHT")
        {
            m_contacts = ESCAPE_RIGHT;
            return true;
        }
        if (verbal == "ESCAPE")
        {
            m_contacts = ESCAPE;
            return true;
        }
        return false;
    }

    bool cEnvironmentParser::capacity_n(std::stringstream & params)
    {
        return params >> m_capacity_n;
    }

    bool cEnvironmentParser::capacity_p(std::stringstream & params)
    {
        return params >> m_capacity_p;
    }

    /*
    cPrinterParser
    */
    cPrinterParser::cPrinterParser(cParser * parent, cMaterial * object):cParser(parent),m_test_object(object),m_filename(""),
    m_norm_x(1), m_norm_y(1), m_type(NONE), m_mode(CURRENT), m_increment(1)
    {
        m_actions[PARSER_END] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cPrinterParser::end);


        m_actions[PRINTERPARSER_TYPE] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cPrinterParser::type);
        m_actions[PRINTERPARSER_MODE] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cPrinterParser::mode);
        m_actions[PRINTERPARSER_NORM] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cPrinterParser::norm);
        m_actions[PRINTERPARSER_FILENAME] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cPrinterParser::filename);


    }

    bool cPrinterParser::type(std::stringstream & params)
    {
        std::string name;
        params >> name;
        if (name == "NONE")
        {
            m_type = NONE;
            return true;
        }
        if (name == "LOG")
        {
            m_type = LOG;
            m_increment = m_calculator.calculate_stream(params);
            return true;
        }
        if (name == "TIMER")
        {
            m_type = TIMER;
            m_increment = m_calculator.calculate_stream(params);
            return true;
        }
        return false;
    }

    bool cPrinterParser::mode(std::stringstream & params)
    {
        std::string name;
        params >> name;
        if (name == "CURRENT")
        {
            m_mode = CURRENT;
            return true;
        }
        if (name == "DUMP")
        {
            m_mode = DUMP;
            return true;
        }
        return false;
    }

    bool cPrinterParser::norm(std::stringstream & params)
    {
        std::string axis;
        params >> axis;
        if (axis == "X")
        {
            m_norm_x = m_calculator.calculate_stream(params);
            return true;
        }
        if (axis == "Y")
        {
            m_norm_y = m_calculator.calculate_stream(params);
            return true;
        }
        return false;
    }

    bool cPrinterParser::filename(std::stringstream & params)
    {
        std::string name;
        params >> m_filename;
        trimQuotes(m_filename);
        return m_filename != "";
    }

    bool cPrinterParser::end(std::stringstream & params)
    {
        cPrinter * object;
        switch (m_type)
        {
            case NONE:
            {
                cIterPrinter * temp = new cIterPrinter(m_filename, m_test_object, m_norm_x, m_norm_y, m_mode);
                object = reinterpret_cast<cPrinter*>(temp);
                break;
            }
            case LOG:
            {
                cLogPrinter * temp = new cLogPrinter(m_filename, m_test_object, m_increment, m_norm_x, m_norm_y, m_mode);
                object = reinterpret_cast<cPrinter*>(temp);
                break;
            }
            case TIMER:
            {
                cTimerPrinter * temp = new cTimerPrinter(m_filename, m_test_object, m_increment, m_norm_x, m_norm_y, m_mode);
                object = reinterpret_cast<cPrinter*>(temp);
                break;
            }
        }

        m_object = reinterpret_cast<cObject *>(object);
        return cParser::end(params);
    }

    /*
    cMaterialParser
    */
    cMaterialParser::cMaterialParser(cParser * parent, cEnvironment * environment):cParser(parent)
    {
        m_real_object = new cMaterial(environment);
        m_actions[MATERIALPARSER_INITIAL] = static_cast<bool (cParser::*) (std::stringstream &)>(&cMaterialParser::initial);
        m_actions[MATERIALPARSER_DIRAC] = static_cast<bool (cParser::*) (std::stringstream &)>(&cMaterialParser::dirac_initial);
        m_actions[PARSER_END] = static_cast<bool (cParser::*) (std::stringstream &)>(&cMaterialParser::end);
    }

    bool cMaterialParser::initial(std::stringstream & params)
    {
        std::string function;
        params >> function;
        calculatorFunction * foo = new calculatorFunction(function, &m_calculator);
        m_real_object->fill(dynamic_cast<mathFunction *>(foo), dynamic_cast<mathFunction *>(foo));
        return true;
    }

    bool cMaterialParser::dirac_initial(std::stringstream & params)
    {
        double coordinate, value;
        params >> value >> coordinate;
        diracFunction * delta_p = new diracFunction(value, coordinate);
        diracFunction * delta_n = new diracFunction(value, coordinate);
        m_real_object->fill(dynamic_cast<mathFunction *>(delta_n), dynamic_cast<mathFunction *>(delta_p));
        return true;
    }

    bool cMaterialParser::end(std::stringstream & params)
    {
        m_object = reinterpret_cast<cObject *>(m_real_object);
        return cParser::end(params);
    }

 }
