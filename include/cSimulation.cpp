#include "cSimulation.h"

namespace DoI
{
    cSimulation::
    cSimulation(cEnvironment * environment,
                cMaterial * material,
                cPrinter * printer,
                std::queue<sOperation> * todo):
    m_environment(environment),
    m_object(material),
    m_printer(printer),
    m_operation_queue(todo)
    {
        m_operation_map["STABLE"]          = &cSimulation::stable;
        m_operation_map["CELIV"]           = &cSimulation::celiv;
    }

    void cSimulation::
    run()
    {
        sOperation cur;
        while (!m_operation_queue->empty())
        {
            cur = m_operation_queue->front();
            std::map<std::string, bool (cSimulation::*) (std::istream &)>::iterator act;
            act = m_operation_map.find(cur.name);
            //If line not understood, give warning:
            if (act == m_operation_map.end())
            {
                cLogger::error(std::string("Scenario line ") + cur.name + std::string(" not understood, ignoring."));
            }
            bool (cSimulation::*function) (std::istream &) = act->second;
            std::stringstream params(cur.params);
            if (!(*this.*function) (params))
            {
                cLogger::warning(std::string("Scenario line ") + cur.name + std::string(" executing failed."));
                return;
            }
            m_operation_queue->pop();
        }
    }

    bool cSimulation::
    stable(std::istream & fin)
    /**Funkcija atjungianti įtampą,
    Kviečiama taip: STABLE laikas
    */
    {
        m_environment->voltage(0.0);
        double end_time;
        fin >> end_time;
        do_until(end_time);
    }

    bool cSimulation::
    celiv(std::istream & fin)
    /**Pagrindinė CELIV funkcija,
    Kviečiama taip: CELIV pradinė_įtampa galinė_įtampa laikas
    */
    {
        double s_voltage, e_voltage, end_time;
        fin >> s_voltage >> e_voltage >> end_time;

        statusFunction status(m_object->time(), end_time);

        //Įtampos linija U = a+b*t;
        mathFunction * foo = \
            new mathPositiveLine( \
                s_voltage - (s_voltage-e_voltage)/(m_object->time()-end_time)*m_object->time(), \
                (s_voltage-e_voltage)/(m_object->time()-end_time) \
            );

        std::cout << "BEGIN" << std::endl;

        double dt = m_environment->time_step();

        while (m_object->time() < end_time)
        {
            status(m_object->time());
            //Watching for the last step:
            if (m_object->time() > end_time - m_environment->time_step())
            {
                //Decreasing the dt, to meet the time limit.
                m_environment->time_step(end_time-m_object->time());
            }

            m_environment->voltage(foo->Call(m_object->time()));
            do_iter();
        }

        //Making the last step:
        std::cout << ">>ENDED AT: " << m_object->time() << std::endl;
        //Returning dt to starting value:
        m_environment->time_step(dt);
    }

    //HELPERS

    void cSimulation::
    do_iter()
    {
        //Makes 1 iteration, reports current to given function.
        if (m_object->run())
            m_printer->Call();
    }

    void cSimulation::
    do_until(double end_time)
    {
        statusFunction status(m_object->time(), end_time);

        if (end_time < m_object->time())
        {
            std::cout << "E>END TIME PASSED" << std::endl;
            return ;
        }

        std::cout << "BEGIN" << std::endl;

        double dt = m_environment->time_step(); //Saving starting value

        while (m_object->time() < end_time)
        {
            status(m_object->time());
            //Watching for the last step:
            if (m_object->time() > end_time - m_environment->time_step())
            {
                //Decreasing the dt, to meet the time limit.
                m_environment->time_step(end_time-m_object->time());
            }

            do_iter();
        }

        //Making the last step:
        std::cout << ">>ENDED AT: " << m_object->time() << std::endl;
        //Returning dt to starting value:
        m_environment->time_step(dt);

    }

//    void cSimulation::
//    run_until(std::ifstream & fin)
//    {
//        check_init();
//
//        double end_time;
//        fin >>end_time;
//
//        do_until(end_time);
//
//    }

/*
    void cSimulation::
    read_n_execute(std::string filename)
    {
        init();

        //Open ZE file
        std::ifstream fin(filename.c_str());
        if (!fin)
            throw exception::FileMisingExeption(filename);

        std::cout << ">>DOING SIMULATION FROM FILE: " << filename << std::endl;

        //Parse ZE file
        std::string label="";
        void (cSimulation::*function)        (std::ifstream &);
        while (label!="END")
        {
            fin >> label;
            if (!check_and_discard_comment(label, fin))
            {
                function = m_controlers[label];
                if (!function)
                    throw exception::BadCommand("Function "+label+" not found");
                (*this.*function) (fin);
                //Now ignore anything till the end of line.
                fin.ignore(2048, '\n');
            }
        }
    }
*/
//    void cSimulation::
//    init ()
//    {
//        m_controlers["LOAD_CONSTANTS"]  = &cSimulation::load_constants;
//        m_controlers["LOAD_GLOBAL"]     = &cSimulation::load_global;
//        m_controlers["SET_VOLTAGE"]     = &cSimulation::set_voltage;
//        m_controlers["SET_CONTACTS"]    = &cSimulation::set_contacts;
//        m_controlers["SET_TIME_STEP"]   = &cSimulation::set_time_step;
//        m_controlers["SET_OUTPUT"]      = &cSimulation::set_output;
//        m_controlers["CREATE_OBJECT"]   = &cSimulation::create_object;
//        m_controlers["LOAD_OBJECT"]     = &cSimulation::load_object;
//        m_controlers["SAVE_OBJECT"]     = &cSimulation::save_object;
//        m_controlers["FOTO_INJECTION"]  = &cSimulation::foto_injection;
//        m_controlers["RUN_BY_TRANSIT"]  = &cSimulation::run_by_transit;
//        m_controlers["RUN_UNTIL"]       = &cSimulation::run_until;
//        m_controlers["RUN_ITER"]        = &cSimulation::run_iter;
//        m_controlers["DOI"]             = &cSimulation::doi;
//        m_controlers["STABLE"]          = &cSimulation::stable;
//        m_controlers["CELIV"]           = &cSimulation::celiv;
//        m_controlers["PAUSE"]           = &cSimulation::pause;
//        m_controlers["END"]             = &cSimulation::end;
//
//    }
//
//    void cSimulation::
//    load_constants(std::ifstream & fin)
//    {
//        std::string filename;
//        fin >> filename;
//
//        std::ifstream cfile(filename.c_str());
//        if (!cfile)
//            throw exception::FileMisingExeption(filename);
//
//        //double beta, S, MIN, q, eps, eps0, k, T, n_miu, p_miu, n_D, p_D,  U, dt, width, timeout, size;
//        double S, MIN, q, eps, eps0, T, n_miu, p_miu, k_glue;
//        std::string id;
//        std::string none;
//
//        cfile >> id >> none;
//        cfile >> id >> S;
//        cfile >> id >> MIN;
//        cfile >> id >> q;
//        cfile >> id >> eps;
//        cfile >> id >> eps0;
//        cfile >> id >> none;
//        cfile >> id >> T;
//        cfile >> id >> n_miu;
//        cfile >> id >> p_miu;
//        cfile >> id >> none;
//        cfile >> id >> none;
//        cfile >> id >> k_glue;
//
//        cfile.close();
//
//        if (m_constants != NULL)
//            delete m_constants;
//
//        m_constants = new cConstants(   recombinationConst(q, n_miu, p_miu, eps, eps0),
//                                        S,
//                                        MIN,
//                                        q,
//                                        eps,
//                                        eps0,
//                                        driftConst(eps, eps0, S),
//                                        T,
//                                        n_miu,
//                                        p_miu,
//                                        diffusionConst(T, n_miu, q),
//                                        diffusionConst(T, p_miu, q),
//                                        k_glue );
//
//        std::cout << ">>LOADED CONSTANTS FROM FILE: " << filename << std::endl;
//        std::cout << *m_constants << std::endl;
//    }
//
//    void cSimulation::
//    load_global(std::ifstream & fin)
//    {
//        std::string filename;
//        fin >> filename;
//
//        std::ifstream gfile(filename.c_str());
//        if (!gfile)
//            throw exception::FileMisingExeption(filename);
//
//        double U;
//        double dt;
//        double width;
//        uint64_t type;
//        uint64_t size;
//        double n_cap;
//        double p_cap;
//
//        std::string id;
//        gfile >> id >> U;
//        gfile >> id >> dt;
//        gfile >> id >> width;
//        gfile >> id >> type;
//        gfile >> id >> size;
//        gfile >> id >> n_cap;
//        gfile >> id >> p_cap;
//
//        gfile.close();
//
//        if (m_global)
//            delete m_global;
//
//        m_global = new cGlobal(U, dt, width, (CONTACTS_TYPE)type, 0, 0, size, n_cap, p_cap);
//        std::cout << ">>LOADED GLOBALS FROM FILE: " << filename << std::endl;
//        std::cout << *m_global << std::endl;
//    }
//
//    void cSimulation::
//    set_voltage(std::ifstream & fin)
//    {
//        if (!m_global)
//            throw exception::BadCommand("Global constants not loaded.");
//        double U;
//        fin >> U;
//        m_global->s_U(U);
//        std::cout << ">>VOLTAGE SET TO: " << U << std::endl;
//    }
//
//    void cSimulation::
//    set_contacts(std::ifstream & fin)
//    {
//        if (!m_global)
//            throw exception::BadCommand("Global constants not loaded.");
//        uint64_t type;
//        fin >> type;
//        m_global->s_contacts_type((CONTACTS_TYPE)type);
//        std::cout << ">>CONTACTS SET TO: ";
//        switch (m_global->contacts_type())
//        {
//            case (BLOCKING) : std::cout << "BLOCKING"; break;
//            case (INJECTING): std::cout << "INJECTING"; break;
//            case (EXTRACTING): std::cout << "EXTRACTING"; break;
//            case (NON_BLOCKING): std::cout << "NON_BLOCKING"; break;
//            default : std::cout << "NONE";
//                throw exception::BadCommand("Contact type not defined");
//                break;
//        }
//        std::cout << std::endl;
//    }
//
//    void cSimulation::
//    set_output(std::ifstream & fin)
//    {
//        if (m_current_output)
//            delete m_current_output;
//
//        std::string filename;
//        uint64_t type;
//        fin >> filename >> type;
//        double x_norm, y_norm;
//        if (filename == "cout")
//        {
//            switch (type)
//            {
//                case 0:
//                    m_current_output = new logPrint(&(std::cout), m_object);
//                    std::cout << ">>SET OUTPUT TO STD::COUT TYPE LOG" << std::endl;
//                break;
//
//                case 1:
//                    m_current_output = new iterPrint(&(std::cout), m_object);
//                    std::cout << ">>SET OUTPUT TO STD::COUT TYPE ITER" << std::endl;
//                break;
//
//                case 2:
//                    fin >> x_norm >> y_norm;
//                    m_current_output = new normLogPrint(&(std::cout), m_object, x_norm, y_norm);
//                    std::cout << ">>SET OUTPUT TO STD::COUT TYPE NORM LOG" << std::endl;
//                break;
//
//                case 3:
//                    fin >> x_norm >> y_norm;
//                    m_current_output = new normIterPrint(&(std::cout), m_object, x_norm, y_norm);
//                    std::cout << ">>SET OUTPUT TO STD::COUT TYPE NORM ITER" << std::endl;
//                break;
//            }
//        }
//        else
//        {
//            std::ofstream * out = new std::ofstream(filename.c_str());
//            if (!out)
//                throw exception::FileMisingExeption(filename);
//            switch (type)
//            {
//                case 0:
//                    m_current_output = new logPrint(out, m_object);
//                    std::cout << ">>SET OUTPUT TO FILE: " << filename << " TYPE LOG" << std::endl;
//                break;
//
//                case 1:
//                    m_current_output = new iterPrint(out, m_object);
//                    std::cout << ">>SET OUTPUT TO FILE: " << filename << " TYPE ITER" <<std::endl;
//                break;
//
//                case 2:
//                    fin >> x_norm >> y_norm;
//                    m_current_output = new normLogPrint(out, m_object, x_norm, y_norm);
//                    std::cout << ">>SET OUTPUT TO FILE: " << filename << " TYPE NORM LOG" <<std::endl;
//                break;
//
//                case 3:
//                    fin >> x_norm >> y_norm;
//                    m_current_output = new normIterPrint(out, m_object, x_norm, y_norm);
//                    std::cout << ">>SET OUTPUT TO FILE: " << filename << " TYPE NORM ITER" <<std::endl;
//                break;
//
//                case 4:
//                    std::string tl;
//                    fin >> tl >> x_norm >> y_norm;
//                    std::ofstream * cumul = new std::ofstream(tl.c_str(), std::ios::app);
//                    (*cumul) << "MAX FROM " << filename << std::endl;
//                    m_current_output = new cumulPrint(out, cumul, m_object, x_norm, y_norm);
//                    std::cout << ">>SET OUTPUT TO FILE: " << filename << " TYPE APP" <<std::endl;
//                break;
//            }
//        }
//
//    }
//
//    void cSimulation::
//    set_time_step(std::ifstream & fin)
//    {
//        if (!m_global)
//            throw exception::BadCommand("Global constants not loaded.");
//        double dt;
//        fin >> dt;
//        m_global->s_dt(dt);
//        std::cout << ">>TIME STEP SET TO: " << dt << std::endl;
//    }
//
//    void cSimulation::
//    run_by_transit(std::ifstream & fin)
//    {
//        double transit_cycles;
//
//        fin >> transit_cycles;
//        std::cout << ">>RUNING FOR " << transit_cycles << " TRANSIT CYCLES" << std::endl;
//
//        double transit_time = transitTime(*m_environment);
//        std::cout << "T_tr: " << transit_time << std::endl;
//
//        do_until(m_object->time() + transit_time*transit_cycles);
//
//    }
//

//
//    void cSimulation::
//    run_iter(std::ifstream & fin)
//    {
//        check_init();
//
//        uint64_t loop_count;
//        fin >> loop_count;
//
//        std::cout << ">>RUNING FOR " << loop_count << " LOOPS" << std::endl;
//
//        for (uint64_t c = 0; c < loop_count; c++)
//        {
//            std::cout << c << std::endl;
//            do_iter();
//        }
//    }
//
//    void cSimulation::
//    create_object(std::ifstream & fin)
//    {
//        std::cout << ">>CREATING TEST OBJECT" << std::endl;
//        m_object = new cMaterial(m_environment);
//        std::cout << "..DONE" << std::endl;
//    }
//
//    void cSimulation::
//    load_object(std::ifstream & fin)
//    {
//        std::cout << ">>LOADING TEST OBJECT" << std::endl;
//        std::string filename;
//        fin >> filename;
//        m_object = new cMaterial(m_environment);
//        m_object->load(filename);
//        std::cout << "..DONE" << std::endl;
//    }
//
//    void cSimulation::
//    save_object(std::ifstream & fin)
//    {
//        std::cout << ">>SAVING TEST OBJECT" << std::endl;
//        std::string filename;
//        fin >> filename;
//        if (!m_object)
//        {
//            throw exception::BadCommand("Test Object not initialized.");
//        }
//        m_object->dump(filename);
//        std::cout << "..DONE" << std::endl;
//    }
//
//    void cSimulation::
//    foto_injection(std::ifstream & fin)
//    /**Fotoinjekcijos funkcija:
//    FOTO_INJECTION alpha luminosity time
//    */
//    {
//        double alpha, luminocity, time;
//        fin >> alpha >> luminocity >> time;
//        //Skaičiuojama funkcijos vertė laukelio viduryje,
//        // ir toks dalelių kiekis priskiriamas laukeliui.
//
//        mathExp * foo = new mathExp(alpha, luminocity);
//
//        m_object->fill(reinterpret_cast<mathFunction *>(foo), reinterpret_cast<mathFunction *>(foo));
//
//        std::cout << ">>FOTO INJECTED" << std::endl;
//    }
//
//    void cSimulation::
//    doi(std::ifstream & fin)
//    /**Pagrindinė DoI funkcija,
//    Kviečiama taip: DOI įtampa laikas
//    */
//    {
//        if (!m_global || !m_constants || !m_object)
//            throw exception::BadCommand("BAD INITIATION");
//
//        double voltage;
//        fin >> voltage;
//        m_global->s_U(voltage);
//        run_until(fin);
//    }
//
//    void cSimulation::
//    stable(std::ifstream & fin)
//    /**Funkcija atjungianti įtampą,
//    Kviečiama taip: STABLE laikas
//    */
//    {
//        if (!m_global || !m_constants || !m_object)
//            throw exception::BadCommand("BAD INITIATION");
//        m_global->s_U(0);
//        run_until(fin);
//    }
//
//    void cSimulation::
//    celiv(std::ifstream & fin)
//    /**Pagrindinė CELIV funkcija,
//    Kviečiama taip: CELIV pradinė_įtampa galinė_įtampa laikas
//    */
//    {
//        check_init();
//
//        double s_voltage, e_voltage, end_time;
//        fin >> s_voltage >> e_voltage >> end_time;
//
//        statusFunction status(m_object->time(), end_time);
//
//        //Įtampos linija U = a+b*t;
//        mathFunction * foo = \
//            new mathPositiveLine( \
//                s_voltage - (s_voltage-e_voltage)/(m_object->time()-end_time)*m_object->time(), \
//                (s_voltage-e_voltage)/(m_object->time()-end_time) \
//            );
//
//        std::cout << "BEGIN" << std::endl;
//
//        double dt = m_global->dt();
//
//        while (m_object->time() < end_time)
//        {
//            status(m_object->time());
//            //Watching for the last step:
//            if (m_object->time() > end_time - m_global->dt())
//            {
//                //Decreasing the dt, to meet the time limit.
//                m_global->s_dt(end_time-m_object->time());
//            }
//
//            m_global->s_U(foo->Call(m_object->time()));
//            do_iter();
//        }
//
//        //Making the last step:
//        std::cout << ">>ENDED AT: " << m_object->time() << std::endl;
//        //Returning dt to starting value:
//        m_global->s_dt(dt);
//    }
//
//    void cSimulation::
//    pause(std::ifstream & fin)
//    {
//        std::cout << ">>PAUSED" << std::endl;
//        std::cout << "..press ENTER to continue";
//        std::cin.get();
//    }
//
//    void cSimulation::
//    end(std::ifstream & fin)
//    {
//        delete m_constants;
//        delete m_global;
//        delete m_object;
//        delete m_current_output;
//        std::cout << ">>END ...death of all electrons..." << std::endl;
//    }
//

//
//    bool cSimulation::
//    check_and_discard_comment(std::string & label, std::istream & fin)
//    {
//        if (label.at(0) == '#')
//        {
//            fin.ignore(2048, '\n');
//            label = "COMMENT";
//            return true;
//        }
//        else return false;
//    }

    bool cSimulation::validityCheck()
    {
        return m_environment and m_object and m_printer;
    }
}
