#ifndef TFUNCTOR_HPP
#define TFUNCTOR_HPP

#include <cmath>

namespace DoI
{
    // abstract base class
    class mathFunction
    {
        public:
            virtual double operator()(double) = 0;  // call using operator
            virtual double Call(double) = 0;       // call using function
    };

    class mathExp : public mathFunction
    {
        private:
            double alpha;
            double luminosity;

        public:
            mathExp(double a, double l):alpha(a), luminosity(l) {};
            double operator()(double x) {return luminosity*exp(-x*alpha);};
            double Call(double x) {return luminosity*exp(-x*alpha);};
    };
};
#endif
