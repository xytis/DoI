#ifndef TFUNCTOR_HPP
#define TFUNCTOR_HPP

#include <cmath>
#include "calculator.h"

namespace DoI
{
    // abstract base class
    class mathFunction
    {
        public:
            virtual double operator()(double) = 0;  // call using operator
            virtual double Call(double) = 0;       // call using function
    };

    class calculatorFunction : public mathFunction
    {
        private:
            cCalculator * calc;
            std::string name;
        public:
            calculatorFunction(std::string verbose_name, cCalculator * n_calc): calc(n_calc)
            {
                std::stringstream ss(verbose_name);
                std::getline(ss, name, '(');    //Ignore parameter names.
                name.append("()");   //Add parentheses.
            };
            double operator()(double x)
            {
                std::vector<double> params;
                params.push_back(x);
                return calc->call(name, params);
            };
            double Call(double x)
            {
                std::vector<double> params;
                params.push_back(x);
                return calc->call(name, params);
            };
    };

    class diracFunction: public mathFunction
    {
        private:
            double value;
            double coordinate;
            bool trigered;
        public:
            diracFunction(double n_value, double n_coordinate):value(n_value), coordinate(n_coordinate), trigered(false) {};
            //No srsly. WTF? =]
            double operator()(double x) {return trigered ? 0 : (x >= coordinate ? (trigered = true, value): 0);};
            double Call(double x) {return trigered ? 0 : (x >= coordinate ? (trigered = true, value): 0);};
    };

    class mathLine : public mathFunction
    {
        private:
            double a;
            double b;
        public:
            mathLine(double n_a, double n_b):a(n_a), b(n_b) {};
            double operator()(double x) {return a+b*x;};
            double Call(double x) {return a+b*x;};
    };

    class mathPositiveLine : public mathFunction
    {
        private:
            double a;
            double b;
        public:
            mathPositiveLine(double n_a, double n_b):a(n_a), b(n_b) {};
            double operator()(double x) {return x>0?a+b*x:0;};
            double Call(double x) {return x>0?a+b*x:0;};
    };

    class mathExp : public mathFunction
    {
        private:
            double alpha;
            double luminosity;

        public:
            mathExp(double a, double l):alpha(a), luminosity(l) {};
            double operator()(double x) {return alpha*luminosity*exp(-x*alpha);};
            double Call(double x) {return alpha*luminosity*exp(-x*alpha);};
    };

    class statusFunction
    {
        private:
            int current_status;
            int last_status;
            double interval;

        public:
            statusFunction(double start, double end):
            current_status(0),
            last_status(0),
            interval((end-start)/100) {};

            void operator() (double now)
            {
                Call(now);
            };
            void Call(double now)
            {
                if (current_status != last_status)
                {
                    std::cout << "TIME: " << now << '\t';
                    std::cout << "PERCENT: " << current_status << "%" << std::endl;
                    last_status = current_status;
                }
                while (now - current_status*interval > 0)
                    current_status++;
            };
            void operator() (int iter)
            {
                Call(iter);
            };
            void Call(int iter)
            {
                if (current_status != last_status)
                {
                    std::cout << "PERCENT: " << current_status << "%" << std::endl;
                    last_status = current_status;
                }
                while (iter - current_status*interval > 0)
                    current_status++;
            };
    };

}
#endif
