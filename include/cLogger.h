#ifndef CLOGGER_H_INCLUDED
#define CLOGGER_H_INCLUDED

#include <iostream>
#include <string>

class cLogger
{
	public:
        static void warning(std::string);
        static void error(std::string);
	protected:

	private:
		cLogger() {};
		~cLogger() {};
};


#endif // CLOGGER_H_INCLUDED
