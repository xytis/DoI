#include "cConstants.h"

namespace DoI
{
    cConstants * readConstants(std::string filename)
    {

        std::ifstream fin(filename.c_str());
        if (!fin)
            throw exception::FileMisingExeption(filename);

        double beta, S, MIN, q, eps, eps0, k, T, n_miu, p_miu, n_D, p_D,  U, dt, width, timeout, size;
        std::string id;
        std::string none;

        fin >> id >> none;
        fin >> id >> S;
        fin >> id >> MIN;
        fin >> id >> q;
        fin >> id >> eps;
        fin >> id >> eps0;
        fin >> id >> none;
        fin >> id >> T;
        fin >> id >> n_miu;
        fin >> id >> p_miu;
        fin >> id >> none;
        fin >> id >> none;
        fin >> id >> U;
        fin >> id >> dt;
        fin >> id >> width;
        fin >> id >> timeout;
        fin >> id >> size;

        cConstants * C = new cConstants(recombinationConst(q, n_miu, p_miu, eps, eps0),
                                        S,
                                        MIN,
                                        q,
                                        eps,
                                        eps0,
                                        driftConst(eps, eps0, S),
                                        T,
                                        n_miu,
                                        p_miu,
                                        diffusionConst(T, n_miu, q),
                                        diffusionConst(T, p_miu, q),
                                        U,
                                        dt,
                                        width,
                                        timeout,
                                        size);
        return C;
    }

    std::ostream & operator << (std::ostream & out, const cConstants & C)
    {
        out << "Plotas\t\t\t:\t" << C.c_S << std::endl;
        out << "MIN\t\t\t:\t" << C.c_MIN << std::endl;
        out << "Krūvis\t\t\t:\t" << C.c_q << std::endl;
        out << "Eps\t\t\t:\t" << C.c_eps << std::endl;
        out << "Eps0\t\t\t:\t" << C.c_eps0 << std::endl;
        out << "Dreifo K\t\t:\t" << C.c_k << std::endl;
        out << "Temperatūra\t\t:\t" << C.c_T << std::endl;
        out << "Judris n\t\t:\t" << C.c_n_miu << std::endl;
        out << "Judris p\t\t:\t" << C.c_p_miu << std::endl;
        out << "Diffusion n\t\t:\t" << C.c_n_D << std::endl;
        out << "Diffusion p\t\t:\t" << C.c_p_D << std::endl;
        out << "Recombination Beta\t:\t" << C.c_beta << std::endl;
        out << "Voltage\t\t\t:\t" << C.m_U << std::endl;
        out << "Time Step\t\t:\t" << C.m_dt << std::endl;
        out << "Width\t\t\t:\t" << C.m_width << std::endl;
        out << "Timeout\t\t\t:\t" << C.m_timeout << std::endl;
        out << "Size\t\t\t:\t" << C.m_size << std::endl;
    }

    double cConstants::
    transitTime()
    {
        return 0.78*m_width*m_width/c_n_miu/m_U;
    }

    double cConstants::
    currentMax()
    {
        return 9.0/8.0*c_eps*c_eps0*(c_n_miu +c_p_miu)*m_U*m_U/m_width/m_width/m_width * c_S;
    }

    double
    diffusionConst(double T, double miu, double q)
    {
        return (1.38e-23 * T * miu / q);
    }

    double
    recombinationConst(double q, double n_miu, double p_miu, double eps, double eps0)
    {
        return q * (n_miu + p_miu) / eps /eps0; //Landþeveno rekombinacija
    }

    double
    driftConst(double eps, double eps0, double S)
    {
        return (1/(eps * eps0 * S));
    }
};
