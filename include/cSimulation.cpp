#include "cSimulation.hpp"

namespace DoI
{
    cSimulation::
    cSimulation():
    m_constants(NULL),
    m_global(NULL),
    m_object(NULL),
    current_output(NULL)
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
        m_controlers["CREATE_OBJECT"]   = &cSimulation::create_object;
        m_controlers["LOAD_OBJECT"]     = &cSimulation::load_object;
        m_controlers["SAVE_OBJECT"]     = &cSimulation::save_object;
        m_controlers["RUN_BY_TRANSIT"]  = &cSimulation::run_by_transit;
        m_controlers["RUN_UNTIL"]       = &cSimulation::run_until;
        m_controlers["RUN_ITER"]        = &cSimulation::run_iter;
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

        //double beta, S, MIN, q, eps, eps0, k, T, n_miu, p_miu, n_D, p_D,  U, dt, width, timeout, size;
        double S, MIN, q, eps, eps0, T, n_miu, p_miu;
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
        std::string filename;
        fin >> filename;
        if (filename == "cout")
        {
            current_output = &(std::cout);
            std::cout << ">>SET OUTPUT TO STD::COUT" << std::endl;
        }
        else
        {
            current_output = new std::ofstream(filename.c_str());
            std::cout << ">>SET OUTPUT FILE: " << current_output << std::endl;
        }
        if (!current_output)
            throw exception::FileMisingExeption(filename);
    }

    void cSimulation::
    run_by_transit(std::ifstream & fin)
    {
        if (!m_object)
        {
            throw exception::BadCommand("Test Object not initialized.");
        }

        double transit_cycles;

        fin >> transit_cycles;
        std::cout << ">>RUNING FOR " << transit_cycles << " TRANSIT CYCLES" << std::endl;

        if (!current_output)
        {
            std::cout << "W>OUTPUT FILE NOT SET!" << std::endl;
        }



        double transit_time = transitTime(*m_constants, *m_global);

        uint64_t output_file_num = 100;

        double interval = transit_cycles*transit_time/output_file_num;
        double log_inc = 1.00001; //1 kas kiekvien� cikl� �ra�yti.
        //Pakankamas tikslumas 1.001, geras tikslumas 1.00001, na ir tobula 1, bet failai did�iuliai.

        double log_cummulative = 1;
        double last_time = 0;

        uint64_t number = 0;

        std::cout << "BEGIN" << std::endl;
        std::cout << "T_tr: " << transit_time << std::endl;

        while (m_object->time() < transit_cycles*transit_time)
        {
            if (m_object->time() > number*interval)
            {
                #ifdef DUMP
                std::ostringstream temp;
                temp << "dumps/material" << std::setfill('0') << std::setw(3) << number << ".dat";
                m_object->write_material(temp.str());
                temp.str("");
                temp << "dumps/field" << std::setfill('0') << std::setw(3) << number << ".dat";
                m_object->write_field(temp.str());
                #endif //DUMP
                std::cout << "TIME: " << m_object->time() << '\t';
                std::cout << "PERCENT: " << number*100.0/output_file_num << std::endl;
                number ++;
            }

            //Logaritmin� srov�
            if (m_object->time() > last_time*log_cummulative)
            {
                m_object->fcurrent(*current_output);
                log_cummulative *= log_inc;
                last_time = m_object->time();
            }

            m_object->run();
        }

    }

    void cSimulation::
    run_until(std::ifstream & fin)
    {
        if (!m_object)
        {
            throw exception::BadCommand("Test Object not initialized.");
        }

        double end_time;
        fin >>end_time;
        if (end_time < m_object->time())
        {
            std::cout << "E>END TIME PASSED" << std::endl;
            return ;
        }

        uint64_t output_file_num = 100;

        double interval = (end_time-m_object->time())/output_file_num;
        double log_inc = 1.00001; //1 kas kiekvien� cikl� �ra�yti.
        //Pakankamas tikslumas 1.001, geras tikslumas 1.00001, na ir tobula 1, bet failai did�iuliai.

        double log_cummulative = 1;
        double last_time = 0;

        uint64_t number = 0;

        while (m_object->time() < end_time)
        {
            if (m_object->time() > number*interval)
            {
                #ifdef DUMP
                std::ostringstream temp;
                temp << "dumps/material" << std::setfill('0') << std::setw(3) << number << ".dat";
                m_object->write_material(temp.str());
                temp.str("");
                temp << "dumps/field" << std::setfill('0') << std::setw(3) << number << ".dat";
                m_object->write_field(temp.str());
                #endif //DUMP
                std::cout << "TIME: " << m_object->time() << '\t';
                std::cout << "PERCENT: " << number*100.0/output_file_num << std::endl;
                number ++;
            }

            //Logaritmin� srov�
            if (m_object->time() > last_time*log_cummulative)
            {
                m_object->fcurrent(*current_output);
                log_cummulative *= log_inc;
                last_time = m_object->time();
            }

            //Watching for the last step:
            if (m_object->time() > end_time - m_global->dt())
            {
                //Decreasing the dt, to meet the time limit.
                m_global->s_dt(end_time-m_object->time());
            }

            m_object->run();
        }

        //Making the last step:
        std::cout << ">>ENDED AT: " << m_object->time() << std::endl;

    }

    void cSimulation::
    run_iter(std::ifstream & fin)
    {
        if (!m_object)
        {
            throw exception::BadCommand("Test Object not initialized.");
        }

        uint64_t loop_count;
        fin >> loop_count;

        uint64_t count = 0;

        std::cout << ">>RUNING FOR " << loop_count << " LOOPS" << std::endl;

        while (count < loop_count)
        {
            if (count % 10000 == 0)
            {
                #ifdef DUMP
                static uint64_t number = 1;
                std::ostringstream temp;
                temp << "dumps/material" << std::setfill('0') << std::setw(3) << number << ".dat";
                m_object->write_material(temp.str());
                temp.str("");
                temp << "dumps/field" << std::setfill('0') << std::setw(3) << number << ".dat";
                m_object->write_field(temp.str());
                number ++;
                #endif //DUMP
                std::cout << "TIME: " << m_object->time() << '\t';
                std::cout << "PERCENT: " << count*100.0/loop_count <<std::endl;
            }

            count ++;
            m_object->run();
        }
    }

    void cSimulation::
    create_object(std::ifstream & fin)
    {
        std::cout << ">>CREATING TEST OBJECT" << std::endl;
        m_object = new cMaterial(m_constants, m_global);
        std::cout << "..DONE" << std::endl;
    }

    void cSimulation::
    load_object(std::ifstream & fin)
    {
        std::cout << ">>LOADING TEST OBJECT" << std::endl;
        std::string filename;
        fin >> filename;
        m_object = new cMaterial(m_constants, m_global);
        m_object->load(filename);
        std::cout << "..DONE" << std::endl;
    }

    void cSimulation::
    save_object(std::ifstream & fin)
    {
        std::cout << ">>SAVING TEST OBJECT" << std::endl;
        std::string filename;
        fin >> filename;
        if (!m_object)
        {
            throw exception::BadCommand("Test Object not initialized.");
        }
        m_object->dump(filename);
        std::cout << "..DONE" << std::endl;
    }

    void cSimulation::
    end(std::ifstream & fin)
    {
        std::cout << "...death of all..." << std::endl;
    }
};
