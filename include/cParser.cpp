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
    current(NULL),last(NULL)
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
        if (last)
        {
            delete last;
        }
        last = current;
        m_stack.pop();
        current = m_stack.top();
        return last;
    }

    void cParserStack::push(cParser * p)
    {
        if (last)
        {
            delete last;
        }
        last = current;
        current = p;
        m_stack.push(p);
    }
    /*
        cParser
    */
    cParser::cParser():m_object(NULL)
    {
        cParserStack::instance()->push(reinterpret_cast<cParser *>(this));
        m_actions[PARSER_INCLUDE] = &cParser::include;
        m_actions[PARSER_END] = &cParser::end;
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
        cParserStack::instance()->current->close(params);
        cParserStack::instance()->pop();
        cParserStack::instance()->current->resume(params);
        return true;
    }

    bool cParser::close(std::stringstream & params)
    {
        //Nothing to do here.
        return true;
    }

    bool cParser::resume(std::stringstream & params)
    {
        //Nothing to do here.
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
                cLogger::warning(std::string("Line ") + line + std::string(" parsing failed."));
                return false;
            }
            bool (cParser::*function) (std::stringstream &) = act->second;
            return (*this.*function) (reader);
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
    cMainParser::cMainParser()
    {
        m_actions[MAINPARSER_SIMULATION] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::simulation);
        m_actions[MAINPARSER_ENVIRONMENT] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cMainParser::environment);
    }

    bool cMainParser::simulation(std::stringstream & params)
    {
        m_real_object = new cSimulation();
        return true;
    }

    bool cMainParser::environment(std::stringstream & params)
    {
        cEnvironmentParser * parser = new cEnvironmentParser();
        m_last = MAINPARSER_ENVIRONMENT;
        return true;
    }

    bool cMainParser::printer(std::stringstream & params)
    {
        cPrinterParser * parser = new cPrinterParser();
        m_last = MAINPARSER_PRINTER;
        return true;
    }

    bool cMainParser::close(std::stringstream & params)
    {
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

    bool cMainParser::resume(std::stringstream & params)
    {
        //Lets find out what we called last time:
        if (m_last == MAINPARSER_ENVIRONMENT)
        {
            m_environment = reinterpret_cast<cEnvironment *>(cParserStack::instance()->last->object());
        }
        if (m_last == MAINPARSER_PRINTER)
        {
            m_printer = reinterpret_cast<cPrinter *>(cParserStack::instance()->last->object());
        }
        /*
        if (m_last == MAINPARSER_MATERIAL)
        {
            m_material = reinterpret_cast<cMaterial *>(cParserStack::instance()->last->object());
        }
        */
        /*
        if (m_last == MAINPARSER_PROCEDURE)
        {
            m_procedure = reinterpret_cast<cProcedure *>(cParserStack::instance()->last->object());
        }
        */
        return true;
    }

    /*
        cEnvironmentParser
    */
    cEnvironmentParser::cEnvironmentParser()
    {
        m_actions[ENVIRONMENTPARSER_CONSTANT_BETA] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::beta);
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
        m_actions[ENVIRONMENTPARSER_SPACE_DIVISION] = reinterpret_cast<bool (cParser::*) (std::stringstream &)>(&cEnvironmentParser::space_division);

    }

    bool cEnvironmentParser::close(std::stringstream & params)
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

         return (m_object = reinterpret_cast<cObject *>(new cEnvironment(constants, m_time_step, m_space_division)));
    }

    bool cEnvironmentParser::beta(std::stringstream & params)
    {
        std::string temp;
        params >> temp;
        if (inlineTrimSpaces(temp) == "none")
        {
            m_beta = recombinationConst(m_charge, m_mobility_n, m_mobility_p, m_epsilon, m_epsilon_0);
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

    bool cEnvironmentParser::area(std::stringstream & params)
    {
        return params >> m_area;
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
            m_beta = driftConst(m_epsilon, m_epsilon_0, m_area);
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
            m_beta = diffusionConst(m_temperature, m_mobility_n, m_charge);
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
            m_beta = diffusionConst(m_temperature, m_mobility_p, m_charge);
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


    bool cEnvironmentParser::space_division(std::stringstream & params)
    {
        return params >> m_space_division;
    }


 }
