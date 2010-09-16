/*
 * =====================================================================================
 *
 *       Filename:  CExeptions.h
 *
 *    Description:  List of used exeptions
 *
 *        Version:  1.0
 *        Created:  03/10/2010 09:16:19 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */

#ifndef CEXEPTIONS_H_INCLUDED
#define CEXEPTIONS_H_INCLUDED

#include <exception>
#include <string>

namespace DoI
{
    namespace exception
    {

        class FileMisingExeption: public std::exception
        {
            private:
                std::string m_path;
            public:
                FileMisingExeption(std::string path) throw()
                {
                    m_path = path;
                }
                const char* path() const throw()
                {
                    return m_path.c_str();
                }
                const char* what() const throw()
                {
                    return "File at specified path is missing!";
                }
                ~FileMisingExeption () throw () {}
        };

        class TimeIntervalTooLarge: public std::exception
        {
            private:
                double  m_times;
                std::string m_origin;
            public:
                TimeIntervalTooLarge(const double & n, std::string o) throw()
                {
                    m_times = n;
                    m_origin = o;
                }
                const double & times () const throw()
                {
                    return m_times;
                }
                const char* what() const throw()
                {
                    return (std::string("Time interval is too large. Origin: ") + m_origin).c_str();
                }
                ~TimeIntervalTooLarge () throw () {}
        };

        class TimeIntervalTooSmall: public std::exception
        {
            private:
                double  m_times;
                std::string m_origin;
            public:
                TimeIntervalTooSmall(const double & n, std::string o) throw()
                {
                    m_times = n;
                    m_origin = o;
                }
                const double & times () const throw()
                {
                    return m_times;
                }
                const char* what() const throw()
                {
                    return (std::string("Time interval is too small. Origin: ") + m_origin).c_str();
                }
                ~TimeIntervalTooSmall () throw () {}
        };

        class FileBadExeption: public std::exception
        {
            private:
                std::string m_path;
            public:
                FileBadExeption(std::string path) throw()
                {
                    m_path = path;
                }
                const char* path() const throw()
                {
                    return m_path.c_str();
                }
                const char* what() const throw()
                {
                    return "File at specified path can not be read!";
                }
                ~FileBadExeption () throw () {}
        };

        class FileBadVersionExeption: public std::exception
        {
            private:
                std::string m_path;
                std::string m_version;
            public:
                FileBadVersionExeption(std::string path, std::string version) throw()
                {
                    m_path = path;
                    m_version = version;
                }
                const char* path() const throw()
                {
                    return m_path.c_str();
                }
                const char* version() const throw()
                {
                    return m_version.c_str();
                }
                const char* what() const throw()
                {
                    return "File at specified path is in outdated form or wrong format!";
                }
                ~FileBadVersionExeption () throw () {}
        };
    };
};


#endif // CEXEPTIONS_H_INCLUDED
