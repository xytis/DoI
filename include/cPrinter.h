#ifndef CPRINTER_H_INCLUDED
#define CPRINTER_H_INCLUDED

#include <cassert>

#include <cstdlib>
#include <vector>

#include <iostream>
#include <fstream>

#include "global.h"
#include "cBlock.h"
#include "cData.h"
#include "cMaterial.h"


namespace DoI
{
    class cPrinter
    {
        public:
            virtual void operator()() = 0;  // call using operator
            virtual void Call() = 0;        // call using function
    };

    /**
        Printer: cLogCurrent
        Type: LOG NUMBER
        Mode: CURRENT
        Flags: REWRITE
      ?
    */

    class cumulPrint : public cPrinter
    {
        private:
            double  nextTime;
            double  x_norm;
            double  y_norm;
            double  last_current;
            double  last_delta;
            std::ostream * out;
            std::ostream * cumul;
            cMaterial * object;
        public:
            cumulPrint(std::ostream * n_out, std::ostream * n_cumul, cMaterial * n_object,
                        double n_x_norm, double n_y_norm):
            nextTime(n_object->time()),
            x_norm(n_x_norm), y_norm(n_y_norm),
            last_current(n_object->current()), last_delta(0),
            out(n_out), cumul(n_cumul), object(n_object)
            {

            };

            void operator()()
            {
                double delta = object->current() - last_current;
                //(*cumul) << delta << std::endl;
                if (((delta > 0) && (last_delta < 0))||((delta < 0) && (last_delta > 0)))
                {
                    object->fcurrent((*cumul), x_norm, y_norm);
                }

                last_delta = delta;
                last_current = object->current();
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out), x_norm, y_norm);
                    nextTime = object->time()*1.01;
                }

            };

            void Call()
            {
                double delta = object->current() - last_current;
                //(*cumul) << delta << std::endl;
                if (((delta > 0) && (last_delta < 0))||((delta < 0) && (last_delta > 0)))
                {
                    object->fcurrent((*cumul), x_norm, y_norm);
                }

                last_delta = object->current() - last_current;
                last_current = object->current();
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out), x_norm, y_norm);
                    nextTime = object->time()*1.01;
                }
            };
            ~cumulPrint()
            {
                out->flush();
            };
    };

    class iterPrint : public cPrinter
    {
        private:
            std::ostream * out;
            cMaterial * object;
            double lastTime;
        public:
            iterPrint(std::ostream * n_out, cMaterial * n_object):
            out(n_out), object(n_object), lastTime(n_object->time()) {};
            void operator()()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out));
                    lastTime = object->time();
                }
            };

            void Call()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out));
                    lastTime = object->time();
                }
            };

            ~iterPrint()
            {
                out->flush();
            };
    };

    class logPrint : public cPrinter
    {
        private:
            double  nextTime;
            std::ostream * out;
            cMaterial * object;
        public:
            logPrint(std::ostream * n_out, cMaterial * n_object):
            nextTime(n_object->time()), out(n_out), object(n_object) {};
            void operator()()
            {
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out));
                    nextTime = object->time()*1.01;
                }

            };
            void Call()
            {
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out));
                    nextTime = object->time()*1.01;
                }
            };
            ~logPrint()
            {
                out->flush();
            };
    };

    class normIterPrint : public cPrinter
    {
        private:
            std::ostream * out;
            cMaterial * object;
            double lastTime;
            double  x_norm;
            double  y_norm;
        public:
            normIterPrint(std::ostream * n_out, cMaterial * n_object,
                        double n_x_norm, double n_y_norm):
            lastTime(n_object->time()),
            x_norm(n_x_norm), y_norm(n_y_norm),
            out(n_out), object(n_object) {};
            void operator()()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out), x_norm, y_norm);
                    lastTime = object->time();
                }
            };

            void Call()
            {
                if (object->time()>lastTime)
                {
                    object->fcurrent(*(out), x_norm, y_norm);
                    lastTime = object->time();
                }
            };

            ~normIterPrint()
            {
                out->flush();
            };
    };

    class normLogPrint : public cPrinter
    {
        private:
            double  nextTime;
            double  x_norm;
            double  y_norm;
            std::ostream * out;
            cMaterial * object;
        public:
            normLogPrint(std::ostream * n_out, cMaterial * n_object,
                        double n_x_norm, double n_y_norm):
            nextTime(n_object->time()),
            x_norm(n_x_norm), y_norm(n_y_norm),
            out(n_out), object(n_object) {};
            void operator()()
            {
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out), x_norm, y_norm);
                    nextTime = object->time()*1.01;
                }

            };
            void Call()
            {
                if (object->time() > nextTime)
                {
                    object->fcurrent((*out), x_norm, y_norm);
                    nextTime = object->time()*1.01;
                }
            };
            ~normLogPrint()
            {
                out->flush();
            };
    };
}

#endif // CPRINTER_H_INCLUDED
