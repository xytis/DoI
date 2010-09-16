#include "cConstants.h"

namespace DoI
{
    cConstants * readConstants(std::string filename)
    {

        std::ifstream fin(filename.c_str());
        if (!fin)
            throw exception::FileMisingExeption(filename);

        double beta,S,MIN,q,eps,eps0;
        fin >> beta >> S >> MIN >> q >> eps >> eps0;

        cConstants * C = new cConstants(beta,S,MIN,q, eps, eps0);
        return C;
    }

    double cConstants::
    transitTime()
    {
        return 0.78*m_width*m_width/c_n_miu/m_U;
    }

    double cConstants::
    currentMax()
    {
        return 9.0/8.0*c_eps*c_eps0*c_n_miu*m_U*m_U/m_width/m_width/m_width;
    }
}
