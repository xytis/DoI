#include "cPrinter.h"

bool exists(std::string filename)
{

    std::ifstream check(filename.c_str(), std::ifstream::in);
    check.close();
    return !check.fail();
}

std::string timestamp()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [13];

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    strftime (buffer,13,"%y%m%d%H%M%S",timeinfo);
    return std::string(buffer);
}

std::string dump_file(std::string pre, double time)
{
    static uint64_t number = 0;
    std::stringstream s;
    s << pre << std::setw(5) << std::setfill('0') << number++ << '_' << time;
    return s.str();
}

namespace DoI
{
    cPrinter::cPrinter(std::string filename,
                       cMaterial * object,
                       double norm_x,
                       double norm_y,
                       MODE mode):m_object(object),
                                  m_mode(mode),
                                  m_norm_x(norm_x),
                                  m_norm_y(norm_y),
                                  m_filename(filename)
    {
        //Check the filename:
        if (m_filename == "cout")
        {
            out = &std::cout;
        }
        else
        {
            switch (m_mode)
            {
                case CURRENT:
                {
                    //One file will be used.
                    //Check if overwriting:
                    if (!exists(m_filename))
                    {
                        out = new std::ofstream(m_filename.c_str());
                    }
                    else
                    {
                        //Add timestamp to end of file.
                        m_filename += timestamp();
                        out = new std::ofstream(m_filename.c_str());
                    }
                    break;
                }
                case DUMP:
                {
                    //Create a directory with given filename.
                    //Later, each call will create a file in it.
                    mkdir(m_filename.c_str(), 0777);
                    m_filename += '/'; //Make it a dirname =]
                    std::string current_file = m_filename + "current";
                    out = new std::ofstream(current_file.c_str());
                    break;
                }
            }
        }
    }

    void    cPrinter::operator()()
    {
        if (trigger())
        {
            switch (m_mode)
            {
                case CURRENT:
                {
                    m_object->fcurrent((*out), m_norm_x, m_norm_y);
                    return;
                }
                case DUMP:
                {
                    std::ofstream * temp_out = new std::ofstream(dump_file(m_filename,m_object->time()).c_str());
                    m_object->fcurrent((*out), m_norm_x, m_norm_y);
                    m_object->dump((*temp_out));
                    out->flush();
                    temp_out->flush();
                    delete temp_out;
                    return;
                }
            }
        }
    }
    void    cPrinter::Call()
    {
        if (trigger())
        {
            switch (m_mode)
            {
                case CURRENT:
                {
                    m_object->fcurrent((*out), m_norm_x, m_norm_y);
                    return;
                }
                case DUMP:
                {
                    std::ofstream * temp_out = new std::ofstream(dump_file(m_filename,m_object->time()).c_str());
                    m_object->fcurrent((*out), m_norm_x, m_norm_y);
                    m_object->dump((*temp_out));
                    out->flush();
                    temp_out->flush();
                    delete temp_out;
                    return;
                }
            }
        }
    }

    cIterPrinter::cIterPrinter(std::string filename,
                       cMaterial * object,
                       double norm_x,
                       double norm_y,
                       MODE mode) : cPrinter(filename, object, norm_x, norm_y, mode)
    {
    }

    bool    cIterPrinter::trigger()
    {
        return true;
    }

    cLogPrinter::cLogPrinter(std::string filename,
                       cMaterial * object,
                       double increment,
                       double norm_x,
                       double norm_y,
                       MODE mode) : cPrinter(filename, object, norm_x, norm_y, mode),
                                    m_increment(increment),
                                    m_timer(0)
    {
    }

    bool    cLogPrinter::trigger()
    {
        if (m_object->time() > m_timer)
        {
            m_timer = m_object->time()*m_increment;
            return true;
        }
        return false;
    }

    cTimerPrinter::cTimerPrinter(std::string filename,
                       cMaterial * object,
                       double increment,
                       double norm_x,
                       double norm_y,
                       MODE mode) : cPrinter(filename, object, norm_x, norm_y, mode),
                                    m_increment(increment),
                                    m_timer(0)
    {
    }

    bool    cTimerPrinter::trigger()
    {
        if (m_object->time() > m_timer)
        {
            m_timer += m_increment;
            return true;
        }
        return false;
    }
}
