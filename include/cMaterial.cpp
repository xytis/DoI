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
    /**
    Inicijuojant suteikiamas -dt laikas,
    nes pirmas žingsnis turi baigtis kai t==0.
    Po to laikas eina normaliai =]
    */
    cMaterial::
    cMaterial(cEnvironment * E):
        m_environment(E)
    {
        //E->m_time = - E->time_step();
        //Kuriam paraleliai ir el. lauko masyvą.
        cField * E_temp = new cField(); //Visas jungimas vyksta vėliau.
        m_fieldArray.push_back(E_temp);
        //Šiuo atveju svarbu koks storio parametras duotas C.
        iBlock * temp = new cContact(LEFT,
                                     cData(0,
                                           0,
                                           0,
                                           0,
                                           E->capacity_n()/E->space_division(),
                                           E->capacity_p()/E->space_division(),
                                           0,
                                           0,
                                           0,
                                           E->width()/E->space_division()),
                                     E);
        m_blockArray.push_back(temp);

        for (uint64_t i = 1; i < E->space_division()-1; i++)
        {
            E_temp = new cField();
            m_fieldArray.push_back(E_temp);
            temp = new cBlock(cData(0,
                                    0,
                                    0,
                                    0,
                                    E->capacity_n()/E->space_division(),
                                    E->capacity_p()/E->space_division(),
                                    0,
                                    0,
                                    0,
                                    E->width()/E->space_division()),
                              E);
            m_blockArray.push_back(temp);
        }

        E_temp = new cField();
        m_fieldArray.push_back(E_temp);

        temp = new cContact(RIGHT,
                            cData(0,
                                  0,
                                  0,
                                  0,
                                  E->capacity_n()/E->space_division(),
                                  E->capacity_p()/E->space_division(),
                                  0,
                                  0,
                                  0,
                                  E->width()/E->space_division()),
                            E);
        m_blockArray.push_back(temp);

        //Lauko yra 1 daugiau.
        E_temp = new cField();
        m_fieldArray.push_back(E_temp);

        init(E->space_division());
    }

    cMaterial::
    cMaterial(cEnvironment * E, const uint64_t & size, const cData & first, const cData & other, const cData & last):
        m_environment(E)
    {
        E->m_time = - E->time_step();
        /*
        //Kuriam paraleliai ir el. lauko masyvą.
        cField * E_temp = new cField(); //Visas jungimas vyksta vėliau.
        m_fieldArray.push_back(E_temp);

        iBlock * temp = new cContact(LEFT, first,C,G);
        m_blockArray.push_back(temp);

        for (uint64_t i = 1; i < size-1; i++)
        {
            E_temp = new cField();
            m_fieldArray.push_back(E_temp);
            temp = new cBlock(other,C,G);
            m_blockArray.push_back(temp);
        }

        E_temp = new cField();
        m_fieldArray.push_back(E_temp);

        temp = new cContact(RIGHT, last,C,G);
        m_blockArray.push_back(temp);

        //Lauko yra 1 daugiau.
        E_temp = new cField();
        m_fieldArray.push_back(E_temp);

        init(size);
        */
    }

    cMaterial::
    cMaterial(cEnvironment * E, std::vector<cData> & all):
        m_environment(E)
    {
        E->m_time = - E->time_step();
        /*
        //Lauko masyvas nėra išsaugomas.
        //Kuriam paraleliai ir el. lauko masyvą.
        cField * E_temp = new cField(); //Visas jungimas vyksta vėliau.
        m_fieldArray.push_back(E_temp);

        iBlock * temp = new cContact(LEFT, all.at(0),C,G);
        m_blockArray.push_back(temp);

        for (uint64_t i = 1; i < all.size()-1; i++)
        {
            E_temp = new cField();
            m_fieldArray.push_back(E_temp);
            temp = new cBlock(all.at(i),C,G);
            m_blockArray.push_back(temp);
        }

        E_temp = new cField();
        m_fieldArray.push_back(E_temp);

        temp = new cContact(RIGHT, all.at(all.size()-1),C,G);
        m_blockArray.push_back(temp);

        //Lauko yra 1 daugiau.
        E_temp = new cField();
        m_fieldArray.push_back(E_temp);

        init(all.size());
        */
    }

    void cMaterial::
    init(const uint64_t & size)
    //This function links main array and inits backup
    {
        //Link part
        for (uint64_t i = 0; i < size-1; i++)
        {
            link(m_blockArray.at(i), m_blockArray.at(i+1));
            //Kai yra sulinkinta ... Last <---> Current <-/-> Next ...
            //galime apsiausti elementą lauko masyvu:
            //          <-E->  |  <-E-/
            // Last <---> | Current | <-/-> Next
            surround(m_fieldArray.at(i), m_blockArray.at(i), m_fieldArray.at(i+1));
        }
        //Paskutinis elementas dar neturi sulinkinimo:
        surround(m_fieldArray.at(size-1), m_blockArray.at(size-1), m_fieldArray.at(size));

        //Backup part
        for (uint64_t i = 0; i < size; i++)
        {
            m_backupArray.push_back(cData(0,0,0,0,0,0,0,0,0,0));
        }
        //Length part
        m_environment->width(0);
        for (uint64_t i = 0; i < size; i++)
        {
            m_environment->width(m_environment->width() + m_blockArray.at(i)->read().m_width);
        }
        //Paskleidžiame size dydį.
        m_environment->space_division(size);
    }

    void cMaterial::
    link (iBlock * left, iBlock * right)
    {
        left->set_next(right);
        right->set_prev(left);
    }

    void cMaterial::
    surround (cField * left, iBlock * block, cField * right)
    {
        left->set_next(right);
        right->set_prev(left);
        left->attach_right(block);
        right->attach_left(block);
        block->set_E_prev(left);
        block->set_E_next(right);
    }

    void cMaterial::
    backup()
    //This function reads the working array and copies the data to backup
    {
        m_environment->m_backup_time = m_environment->m_time;
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            m_backupArray.at(i) = m_blockArray.at(i)->read();
        }
    }

    void cMaterial::
    restore()
    //This function writes the backup array to working array
    //WARNING: if backup array is bad -- so goes the simulation
    {
        m_environment->m_time = m_environment->m_backup_time;
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            m_blockArray.at(i)->write(m_backupArray.at(i));
        }
    }

    void cMaterial::
    flush()
    //Forces all blocks to flush incoming particles
    {
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            m_blockArray.at(i)->flush();
        }
    }

    void
    field_integral(cField * left, cField * right)
    /**
    Kaip mes integruojame:
    Turime integralo E diferencialą palei x. Jis pas mus yra dE masyvas.
    Tik dar kad turime tik diskretinę jo išraišką.
    Dabar pagal Darbu (Darboux) yra du sumavimo būdai, viršutinės ir apatinės vertės.
    Taigi kaip skaičiuosim? Abu sumavimo būdai neduos tikrosios vertės, kol |dx|>0.
    Suskaičiuojam abi sumas supE(->) ir infE(<-)
    Tada sakom kad tikra vertė kažkur per vidurį. Ir įrašom.

    **/
    {
        double l, r;
        l = 0;
        r = 0;
        //užsaugom pradžias
        cField * tl(left);
        //cField * tr(right);
        while ((left != NULL) && (right != NULL))
        {
            l += left->dE;
            left->supE = l;

            r += right->dE;
            right->infE = -r; //Nes vektorius kitos krypties.

            left = left->next();
            right = right->prev();
        }
        //Nuo pradžios pereinam ir išsaugom Darbu vidurkį:
        while (tl != NULL)
        {
            tl->E = (tl->supE+tl->infE)/2;
            tl = tl->next();
        }
    }

    void cMaterial::
    calc()
    //Commands all blocks to make calculations
    {
        //m_fieldArray.at(0)->E = 0;
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            //Čia įvyksta lauko skaičiavimas, pirma dalis
            //Kartu nunulinama sena reikšmė
            //Pirmo elemento vertę nuvalo cContact LEFT.
            //Akivaizdu, dE užpildymas eina iš kairės į dešine,
            //o kadangi i-1 su i nekoreliuoja, tai reikšmės neturi.
            m_blockArray.at(i)->field_create();
            //Dabar laukas yra dE verčių kratinys, be integravimo ir saugomas kitame masyve.
        }
        //Čia suintegruojame lauką:
        field_integral(m_fieldArray.at(0), m_fieldArray.at(m_fieldArray.size()-1));
        //Dabar laukas suintegruotas, tačiau vis dar nenormuotas.
        //Reikia jį sunormuoti.
        //Tam reikia susiskaičiuoti lauko integralą, palyginti jį su išorinio lauko integralu,
        //Skirtumą pridėti prie vidinio lauko verčių.
        double E_local = m_fieldArray.at(0)->integrate();
        //Čia yra vidinis laukas. Dabar su išoriniu:
        double E_external = m_environment->voltage() / m_environment->width() * m_environment->space_division();
        //tadaa =]
        //Dabar randam skirtumą, dalinam jį per laukelių sk. ir pridedam prie laukelių.
        //ĖĖĖ!!! O kaip bus kai nėra įtampos ank kontaktų? Visai lauką numušim?
        //Gal reikia pirma pridėti išorinę įtampą? O ir iš vis, kas čia per sąlyga,
        //kad išorinė įtampa turi nustatyti vidinį lauką?
        //Juk išorinio ir vidinių laukų suma, kai dalelių viduje pakankamai (soties sąlyga) turi būti lygi 0.
        //Na, trumpiau kalbant, gal išorinę įtampą taikyti tik injekcijai?
        double dE = (E_external - E_local)/m_environment->space_division();
        //Kolkas, jei nėr išorinės įtampos nedarom nieko.
        //if (E_external > 0)
        m_fieldArray.at(0)->force_raise(dE);

        //Na va. Turim suskaičiuotą lauką. HMM. Sunku, ane?

        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            //Čia įvyksta rekombinacija ir prilipimai, atlipimai, krūvio injekcija
            m_blockArray.at(i)->relax();
            //Čia turim dreifą, difuziją ir srovės nustatymą.
            m_blockArray.at(i)->emit();
        }
    }

    double cMaterial::
    current ()
    {
        double c = 0;
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            //Srovė teigiama jei n-> ir <-p.
            c += m_blockArray.at(i)->current();
        }
        return c*m_environment->C()->c_q/m_environment->time_step()/m_environment->space_division();
    }

    void cMaterial::
    fill(mathFunction * f_n, mathFunction * f_p)
    {
        //Palei x vertę paima dalelių kiekį
        cData temp;
        double x;
        x = 0;
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            temp = m_blockArray.at(i)->read();
            x += temp.m_width /2;
            temp.m_n += (*f_n)(x) * temp.m_width;
            temp.m_p += (*f_p)(x) * temp.m_width;
            m_blockArray.at(i)->write(temp);
            x += temp.m_width /2;
        }
    }

    bool cMaterial::
    run()
    //This one makes things happen
    {
        try {
            backup();   //Saves current state
            clock_tick(); //Advances the clock
            flush();    //Flushes the buffered particles
            calc();     //Makes the calculations, fills the buffers
            return true;
        }
        catch (exception::TimeIntervalTooLarge & e)
        {
            changeTimeInterval(-10);
            std::cerr << e.what() << std::endl;
            std::cerr << "Changed to: " << m_environment->time_step() << std::endl;
            restore();
            return false;
        }
        catch (exception::TimeIntervalTooSmall & e)
        {
            changeTimeInterval(+10);
            std::cerr << e.what() << std::endl;
            std::cerr << "Changed to: " << m_environment->time_step() << std::endl;
            restore();
            return false;
        }
    }

    double cMaterial::
    time()
    {
        return m_environment->m_time;
    }

    void cMaterial::
    changeTimeInterval(int by)
    {
        if (by < 0)
        {
            m_environment->time_step(m_environment->time_step() / 10);
            m_environment->m_timeout += 500;      //FIXME: na, reikia sugalvoti lankstesnį grįžimo mechanizmą.
            m_environment->m_time_depth += 1;

        }
        if (by > 0)
        {
            m_environment->time_step(m_environment->time_step() * 10);
            m_environment->m_time_depth -= 1;
            m_environment->m_timeout = 1;
        }
    }

    void cMaterial::
    clock_tick()
    {
        m_environment->m_time += m_environment->time_step();

        if (m_environment->m_time_depth > 0)
        {
            m_environment->m_timeout = m_environment->m_timeout - 1;

            if (m_environment->m_timeout == 0)
            {
                throw exception::TimeIntervalTooSmall(10, "clock check");
            }
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
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
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
        out << "Size of collumns " << m_environment->space_division() << ' ' \
        << "time stamp " << m_environment->m_time << 's' << std::endl;
        out << "Time step " << m_environment->time_step() << std::endl;
        /*
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            std::cout << m_blockArray.at(i)->read();
        }
        */
        out << "Current ";
        fcurrent(out);
        out << std::endl;
/*
        for (uint64_t i = 0; i < m_environment->space_division()+1; i++)
        {
            std::cout << m_fieldArray.at(i)->E << '\t';
        }
        std::cout << std::endl;
*/

        out << "Transit time: " << physics::transitTime(*m_environment) << std::endl;
        out << "Max Current: " << physics::currentMax(*m_environment) << std::endl;
        out << "Other: " << std::endl;
        out << (*(m_environment->C())) << std::endl;
        out << std::endl;

    }

    void cMaterial::
    fcurrent(std::ostream & out, double x_norm, double y_norm)
    {
        out << m_environment->m_time/x_norm << '\t' << current()/y_norm << '\t' << m_environment->time_step() << std::endl;
    }



    void cMaterial::
    write_material(std::string name)
    {
        std::ofstream fout(name.c_str());
        if (!fout)
            throw exception::FileMisingExeption(name);

        //Writing time
        //fout << m_environment->m_time << std::endl;
        //Writing data
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            fout << m_blockArray.at(i)->read() << std::endl;
        }
        fout.close();
    }

    void cMaterial::
    write_field(std::string name)
    {
        std::ofstream fout(name.c_str());
        if (!fout)
            throw exception::FileMisingExeption(name);
        //Writing time
        //fout << m_environment->m_time << std::endl;
        //Writing data
        for (uint64_t i = 0; i < m_environment->space_division() + 1; i++)
        {
            fout << *(m_fieldArray.at(i)) << std::endl;
        }
        fout.close();
    }

    void cMaterial::
    stats(sSnapshot & snap)
    {
        snap.cur_time = m_environment->m_time;
        snap.array_size = m_environment->space_division();
        delete[] snap.array;
        snap.array = new cData[snap.array_size];
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            snap.array[i] = m_blockArray.at(i)->read();
        }
        snap.current = current();
        return ;
    }

    void cMaterial::
    load(std::string name)
    {
        std::ifstream fin(name.c_str());
        if (!fin)
            throw exception::FileMisingExeption(name);
        //Reading time
        fin >> m_environment->m_time;
        //Reading memory dump
        cData temp(0,0,0,0,0,0,0,0,0,0);
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            fin >> temp;
            m_blockArray.at(i)->write(temp);
        }
        fin.close();
    }

    void cMaterial::
    dump(std::ostream & out)
    {
        //Writing time
        //out << m_environment->m_time << std::endl;
        //Writing memory dump
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            out << m_blockArray.at(i)->read() << '\t' << *(m_fieldArray.at(i)) << std::endl;
        }
        out.flush();
    }

    #ifdef DEBUG

    void cMaterial::
    check_link()
    {
        report("initial");
        iBlock * cur = m_blockArray.at(0);
        for (uint64_t i = 1; i < m_environment->space_division(); i++)
        {
            assert(m_blockArray.at(i) == cur->next());
            cur = cur->next();
        }
        report("after forward check");
        cur = m_blockArray.at(m_environment->space_division()-1);
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
        for (uint64_t i = 0; i < m_environment->space_division(); i++)
        {
            m_blockArray.at(i)->write(cData(1,1,1,1,1,1,1,1,1,1));
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
