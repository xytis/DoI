#include "cSimulation.hpp"

namespace DoI
{
    cSimulation::
    cSimulation():
    m_constants(NULL),
    m_global(NULL)
    {

    }

    void cSimulation::
    read_n_execute(std::string filename)
    {
        init();

        //Open ZE file
        std::ifstream fin(filename.c_str());
        if (!fin)
            throw exception::FileMisingExeption(filename);

        //Parse ZE file
        std::string label="";
        void (cSimulation::*function)        (std::ifstream &);
        while (label!="END")
        {
            fin >> label;
            function = m_controlers[label];
            (*this.*function) (fin);
        }
    }

    void cSimulation::
    init ()
    {
        m_controlers["LOAD_CONSTANTS"]  = &cSimulation::load_constants;
        m_controlers["LOAD_GLOBAL"]     = &cSimulation::load_global;
        m_controlers["SET_OUTPUT"]      = &cSimulation::set_output;
        m_controlers["RUN_BY_TIME"]     = &cSimulation::run_by_time;
        m_controlers["END"]             = &cSimulation::end;
    }

    void cSimulation::
    load_constants(std::ifstream & fin)
    {
        std::string filename;
        fin >> filename;

        std::ifstream cfile(filename.c_str());
        if (!cfile)
            throw exception::FileMisingExeption(filename);

        double beta, S, MIN, q, eps, eps0, k, T, n_miu, p_miu, n_D, p_D,  U, dt, width, timeout, size;
        std::string id;
        std::string none;

        cfile >> id >> none;
        cfile >> id >> S;
        cfile >> id >> MIN;
        cfile >> id >> q;
        cfile >> id >> eps;
        cfile >> id >> eps0;
        cfile >> id >> none;
        cfile >> id >> T;
        cfile >> id >> n_miu;
        cfile >> id >> p_miu;
        cfile >> id >> none;
        cfile >> id >> none;

        cfile.close();

        if (m_constants != NULL)
            delete m_constants;

        m_constants = new cConstants(recombinationConst(q, n_miu, p_miu, eps, eps0),
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
                                        diffusionConst(T, p_miu, q));

        std::cout << ">>LOADED CONSTANTS FROM FILE: " << filename << std::endl;
        std::cout << *m_constants << std::endl;
    }

    void cSimulation::
    load_global(std::ifstream & fin)
    {
        std::string filename;
        fin >> filename;

        std::ifstream gfile(filename.c_str());
        if (!gfile)
            throw exception::FileMisingExeption(filename);

        double U;
        double dt;
        double width;
        uint64_t size;

        std::string id;
        gfile >> id >> U;
        gfile >> id >> dt;
        gfile >> id >> width;
        gfile >> id >> size;

        gfile.close();

        m_global = new cGlobal(U, dt, width, 0, 0, size);
        std::cout << ">>LOADED GLOBALS FROM FILE: " << filename << std::endl;
        std::cout << *m_global << std::endl;
    }

    void cSimulation::
    set_output(std::ifstream & fin)
    {
        fin >> current_output;
        std::cout << ">>SET OUTPUT FILE: " << current_output << std::endl;
    }

    void cSimulation::
    run_by_time(std::ifstream & fin)
    {
        double transit_cycles;

        cMaterial * test = new cMaterial(m_constants, m_global);

        fin >> transit_cycles;
        std::cout << ">>RUNING FOR " << transit_cycles << " TRANSIT CYCLES" << std::endl;

        std::ofstream fcurrent (current_output.c_str());
        if (!fcurrent)
            throw exception::FileBadExeption(current_output);
        std::cout << ">>USING FILE FOR OUTPUT:" << current_output << std::endl;

        double transit_time = transitTime(*m_constants, *m_global);

        uint64_t output_file_num = 100;

        double interval = transit_cycles*transit_time/output_file_num;
        double log_inc = 1.00001; //1 kas kiekvienà ciklà áraðyti.
        //Pakankamas tikslumas 1.001, geras tikslumas 1.00001, na ir tobula 1, bet failai didþiuliai.

        double log_cummulative = 1;
        double last_time = 0;

        uint64_t number = 0;

        std::cout << "BEGIN" << std::endl;
        std::cout << "T_tr: " << transit_time << std::endl;

        while (test->time() < transit_cycles*transit_time)
        {
            if (test->time() > number*interval)
            {
                #ifdef DUMP
                std::ostringstream temp;
                temp << "dumps/material" << std::setfill('0') << std::setw(3) << number << ".dat";
                test->write_material(temp.str());
                temp.str("");
                temp << "dumps/field" << std::setfill('0') << std::setw(3) << number << ".dat";
                test->write_field(temp.str());
                #endif //DUMP
                std::cout << "TIME: " << test->time() << '\t';
                std::cout << "PERCENT: " << number*100.0/output_file_num << std::endl;
                number ++;
            }

            //Logaritminë srovë
            if (test->time() > last_time*log_cummulative)
            {
                test->fcurrent(fcurrent);
                log_cummulative *= log_inc;
                last_time = test->time();
            }

            test->run();
        }

    }

    void cSimulation::
    end(std::ifstream & fin)
    {
        std::cout << "...death of all..." << std::endl;
    }
};
