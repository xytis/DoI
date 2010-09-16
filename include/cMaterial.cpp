/*
 * =====================================================================================
 *
 *       Filename:  cMaterial.cpp
 *
 *    Description:  Implementation file
 *
 *        Version:  1.0
 *        Created:  2010-04-13-21.57.21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */

#include "cMaterial.h"

namespace DoI
{
    cMaterial::
    cMaterial(cConstants * C, const uint64_t & size):
        m_constants(C),
        m_time(0)
    {
        //Šiuo atveju svarbu koks storio parametras duotas C.
        IBlock * temp = new cContact(LEFT, cData(0,0,0,0,0,C->m_width/size) ,C);
        m_blockArray.push_back(temp);

        for (uint64_t i = 1; i < size-1; i++)
        {
            temp = new cBlock(cData(0,0,0,0,0,C->m_width/size) ,C);
            m_blockArray.push_back(temp);
        }

        temp = new cContact(RIGHT, cData(0,0,0,0,0,C->m_width/size) ,C);
        m_blockArray.push_back(temp);

        init(size);
    }

    cMaterial::
    cMaterial(cConstants * C, const uint64_t & size, const cData & first, const cData & other, const cData & last):
        m_constants(C),
        m_time(0)
    {
        IBlock * temp = new cContact(LEFT, first,C);
        m_blockArray.push_back(temp);

        for (uint64_t i = 1; i < size-1; i++)
        {
            temp = new cBlock(other,C);
            m_blockArray.push_back(temp);
        }

        temp = new cContact(RIGHT, last,C);
        m_blockArray.push_back(temp);

        init(size);
    }

    cMaterial::
    cMaterial(cConstants * C, std::vector<cData> & all):
        m_constants(C),
        m_time(0)
    {
        IBlock * temp = new cContact(LEFT, all.at(0),C);
        m_blockArray.push_back(temp);

        for (uint64_t i = 1; i < all.size()-1; i++)
        {
            temp = new cBlock(all.at(i),C);
            m_blockArray.push_back(temp);
        }

        temp = new cContact(RIGHT, all.at(all.size()-1),C);
        m_blockArray.push_back(temp);

        init(all.size());
    }

    void cMaterial::
    init(const uint64_t & size)
    //This function links main array and inits backup
    {
        //Link part
        for (uint64_t i = 0; i < size-1; i++)
        {
            link(m_blockArray.at(i), m_blockArray.at(i+1));
        }
        //Backup part
        for (uint64_t i = 0; i < size; i++)
        {
            m_backupArray.push_back(cData(0,0,0,0,0,0));
        }
        //Length part
        for (uint64_t i = 0; i < size; i++)
        {
            m_constants->m_width += m_blockArray.at(i)->read().m_width;
        }
        //Paskleidžiame size dydį.
        m_constants->m_size = size;
    }

    void cMaterial::
    link (IBlock * left, IBlock * right)
    {
        left->set_next(right);
        right->set_prev(left);
    }

    void cMaterial::
    backup()
    //This function reads the working array and copies the data to backup
    {
        m_backup_time = m_time;
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            m_backupArray.at(i) = m_blockArray.at(i)->read();
        }
    }

    void cMaterial::
    restore()
    //This function writes the backup array to working array
    //WARNING: if backup array is bad -- so goes the simulation
    {
        m_time = m_backup_time;
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            m_blockArray.at(i)->write(m_backupArray.at(i));
        }
    }

    void cMaterial::
    flush()
    //Forces all blocks to flush incoming particles
    {
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            m_blockArray.at(i)->flush();
        }
    }

    void cMaterial::
    calc()
    //Commands all blocks to make calculations
    {
        double E = 0, U = 0;
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            //Čia įvyksta lauko skaičiavimas, pirma dalis (su integravimu)
            m_blockArray.at(i)->field(E, U);
        }
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            //Čia įvyksta antra lauko skaičiavimo dalis, išorinio lauko įvedimas
            m_blockArray.at(i)->applyVoltage((m_constants->m_U - U)/m_constants->m_width);
            //Čia įvyksta rekombinacija ir prilipimai, atlipimai, krūvio įnjekcija
            m_blockArray.at(i)->relax();
            //Čia turim dreifą, difuziją ir srovės nustatymą.
            m_blockArray.at(i)->emit();
        }
    }

    double cMaterial::
    current ()
    {
        double c = 0;
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            //Srovė teigiama jei n-> ir <-p.
            c += m_blockArray.at(i)->current();
        }
        return c;
    }

    void cMaterial::
    fcurrent(std::ostream & out)
    {
        out << m_time << '\t' << current() << std::endl;
    }

    void cMaterial::
    run()
    //This one makes things happen
    {
        try {
            backup();
            clock_tick();
            flush();
            calc();
        }
        catch (exception::TimeIntervalTooLarge & e)
        {
            restore();
            changeTimeInterval(-10);
            std::cerr << e.what() << std::endl;
        }
        catch (exception::TimeIntervalTooSmall & e)
        {
            restore();
            changeTimeInterval(+10);
            std::cerr << e.what() << std::endl;
        }
    }

    double cMaterial::
    time()
    {
        return m_time;
    }

    void cMaterial::
    changeTimeInterval(int by)
    {
        if (by < 0)
        {
            m_constants->m_dt /= 10;
            m_constants->m_timeout = -1;
        }
        if (by > 0)
        {
            m_constants->m_dt *= 10;
            m_constants->m_timeout += 100;
        }
    }

    void cMaterial::
    clock_tick()
    {
        m_time += m_constants->m_dt;
        if (m_constants->m_timeout > 0)
            m_constants->m_timeout--;

        if (m_constants->m_timeout == 0)
        {
            throw exception::TimeIntervalTooSmall(10, "clock check");
        }
    }

    void cMaterial::
    report(std::string name, bool hold)
    {
        static uint64_t reportID(0);
        if (name != "")
            std::cout << "#BEGIN REPORT " << reportID << ": " << name << std::endl;
        else
            std::cout << "#BEGIN REPORT " << reportID << " #" << std::endl;
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            std::cout << m_blockArray.at(i)->read();
        }
        std::cout << "#END REPORT " << reportID << " #" << std::endl << std::endl;

        reportID++;
        if (hold)
        {
            std::cin.get();
        }
    }

    void cMaterial::
    fstats(std::ostream & out)
    {
        out << "Statistics on simulation." << std::endl;
        out << "Size of collumns " << m_constants->m_size << ' ' \
        << "time stamp " << m_time << 's' << std::endl;
        out << "Time step " << m_constants->m_dt << std::endl;
        /*
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            std::cout << m_blockArray.at(i)->read();
        }
        */
        out << "Current ";
        fcurrent(out);
        out << "Transit time: " << m_constants->transitTime() << std::endl;
        out << "Voltage: " << m_constants->m_U << std::endl;
        out << std::endl;
    }

    void cMaterial::
    stats(sSnapshot & snap)
    {
        snap.cur_time = m_time;
        snap.array_size = m_constants->m_size;
        delete[] snap.array;
        snap.array = new cData[snap.array_size];
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            snap.array[i] = m_blockArray.at(i)->read();
        }
        snap.current = current();
        return ;
    }

    void cMaterial::
    dump(std::string name)
    {
        std::ofstream fout(name.c_str());
        if (!fout)
            throw exception::FileMisingExeption(name);
        //Writing time
        fout << m_time << std::endl;
        //Writing memory dump
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            fout << m_blockArray.at(i)->read();
        }

    }

    void cMaterial::
    load(std::string name)
    {
        std::ifstream fin(name.c_str());
        if (!fin)
            throw exception::FileMisingExeption(name);
        //Reading time
        fin >> m_time;
        //Reading memory dump
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            cData temp(0,0,0,0,0,0);
            fin >> temp;
            m_blockArray.at(i)->write(temp);
        }
    }

    #ifdef DEBUG

    void cMaterial::
    check_link()
    {
        report("initial");
        IBlock * cur = m_blockArray.at(0);
        for (uint64_t i = 1; i < m_constants->m_size; i++)
        {
            assert(m_blockArray.at(i) == cur->next());
            cur = cur->next();
        }
        report("after forward check");
        cur = m_blockArray.at(m_constants->m_size-1);
        //FIXME: čia su uint64_t bėda, kai išeina į -1 tai gaunam overflow.
        //Todėl ciklas begalinis...
        for (int i = -2; i >= 0; i--)
        {
            assert(m_blockArray.at(i) == cur->prev());
            cur=cur->prev();
        }
        report("after backward check");
    }

    void cMaterial::
    check_backup()
    {
        report("before backup");
        backup();
        report("after backup");
        //Make some changes =]
        for (uint64_t i = 0; i < m_constants->m_size; i++)
        {
            m_blockArray.at(i)->write(cData(1,1,1,1,1,1));
        }
        report("after messing");
        restore();
        report("after restoring");
    }

    void cMaterial::
    check_run()
    {
        report("initial");
        for (int i = 0; i < 300; i++)
        {
            run();
            report("after run");
        }
    }
    #endif //DEBUG
}
