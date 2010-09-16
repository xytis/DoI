/*
 * =====================================================================================
 *
 *       Filename:  cBlock.cpp
 *
 *    Description:  implementation file
 *
 *        Version:  2.0
 *        Created:  2010-04-13-18.23.43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
#include "cBlock.h"

namespace DoI
{

    cBlock::
    cBlock(const cData & data, cConstants * C):
        m_data(data),
        m_n_buffer(0),
        m_p_buffer(0),
        m_current(0),
        m_C(C)
    {
        //EMPTY
    }

    IBlock * cBlock::
    next()
    {
        return reinterpret_cast<IBlock *>(m_next);
    }

    IBlock * cBlock::
    prev()
    {
        return reinterpret_cast<IBlock *>(m_prev);
    }

    void cBlock::
    set_next(IBlock * next)
    {
        m_next = reinterpret_cast<cBlock *>(next);
    }

    void cBlock::
    set_prev(IBlock * prev)
    {
        m_prev = reinterpret_cast<cBlock *>(prev);
    }

    //TODO: Čia guls visi skaičiavimai.

    void cBlock::
    recombination()
    {
        double pairs = m_data.m_n * m_data.m_p * m_C->c_beta * m_C->m_dt / m_C->c_S / m_data.m_width;
        if ((pairs > m_data.m_n)||(pairs > m_data.m_p))
            throw exception::TimeIntervalTooLarge(10, "Recombination");
        if (pairs > m_C->c_MIN)
        {
            m_data.m_n -= pairs;
            m_data.m_p -= pairs;
        }
    }

    void cBlock::
    glue_unglue()
    {
        //TODO: wtf???
    }

    void cBlock::
    relax()
    {
        //Čia rekombinacija ir krūvininkų prilipimas
        //Injekcija tik pas kontaktus
        recombination();
        glue_unglue();
    }

    void cBlock::
    field(double & E, double & U)
    {
        //Čia el. lauko skaičiavimas.
        double dE = - m_C->c_k * m_C->c_q *    \
                    ((m_data.m_p + m_data.m_p_stuck) - \
                    (m_data.m_n + m_data.m_n_stuck));

        E += dE;
        m_data.m_E = E;
        U += dE*m_data.m_width;
    }

    void cBlock::
    applyVoltage(double dE)
    {
        m_data.m_E += dE;
    }

    double cBlock::
    diffusion(const double & interest,const double & neighbour, const double & D, std::string place)
    {
        double dc;
        dc = (interest - neighbour)*D*m_C->m_dt / (m_data.m_width*m_data.m_width);  //dc = D * deltaC * dt / dx^2
        if (dc>interest/5)
            throw exception::TimeIntervalTooLarge(10, std::string("Diffusion ") + place);
        if (dc < m_C->c_MIN)
            dc = 0;
        return dc;
    }

    double cBlock::
    drift(const double & interest, const double & field, const double & miu, std::string place)
    {
        double dc;
        dc = miu * field * m_C->m_dt / m_data.m_width * interest;
        if (dc > interest/5)
            throw exception::TimeIntervalTooLarge(10, std::string("Drift ") + place);
        if (dc < m_C->c_MIN)
            dc = 0;
        return dc;
    }

    void cBlock::
    emit()
    {
        //Čia kumuliuojama srovė.
        m_current = 0;
        //Iš čia siunčiami krūvininkai kaimynams.

        double dn, dp;
        //Elektronai einantys á kairæ
        dn = diffusion(m_data.m_n, prev()->read().m_n, m_C->c_n_D, "1");
        //Skylës einanèios á kairæ
        dp = diffusion(m_data.m_p, prev()->read().m_p, m_C->c_p_D, "2");
        //Judėjimas dėl dreifo į kairę
        dp += drift(m_data.m_p, m_data.m_E, m_C->c_p_miu, "1");
        //Elektronai juda į kairę kai laukas priešingas
        dn += drift(m_data.m_n, -m_data.m_E, m_C->c_n_miu, "2");

        //Isimam daleles.
        m_n_buffer -= dn;
        m_p_buffer -= dp;

        prev()->receive(dn,dp);
        //srovė kuri sukuriama iš šio bloko:
        m_current += dp-dn; //Ji lokaliai gali būti ir neigiama.

        //Elektronai einantys á deðinæ
        dn = diffusion(m_data.m_n, next()->read().m_n, m_C->c_n_D, "3");
        //Skylës einanèios á deðinæ
        dp = diffusion(m_data.m_p, next()->read().m_p, m_C->c_p_D, "4");
        //Judėjimas dėl dreifo į dešinę
        dn += drift(m_data.m_n, m_data.m_E, m_C->c_n_miu, "3");
        //Skylės juda į dešinę kai laukas priešingas
        dp += drift(m_data.m_p, -m_data.m_E, m_C->c_p_miu, "4");

        //Isimam daleles.
        m_n_buffer -= dn;
        m_p_buffer -= dp;

        next()->receive(dn,dp);
        //srovė kuri sukuriama iš šio bloko:
        m_current += dn-dp; //Ji lokaliai gali būti ir neigiama.

        return ;
    }

    void cBlock::
    flush()
    {
        //Čia NETIKRINAMA ar gautas logiškas kiekis krūvininkų.
        //Čia jie tiesiog išsaugomi.
        m_data.m_n += m_n_buffer;
        m_data.m_p += m_p_buffer;
        m_n_buffer = 0;
        m_p_buffer = 0;
    }

    void cBlock::
    receive(const double & n, const double & p)
    {
        m_n_buffer += n;
        m_p_buffer += p;
    }


    const cData & cBlock::
    read()
    {
        return m_data;
    }

    const cData & cBlock::
    write(const cData & data)
    {
        m_data = data;
        return m_data;
    }

    const eStatus cBlock::
    check()
    {
        return OK;
    }

    const double cBlock::
    current()
    {
        return m_current;
    }

    /**cContact*/

    cContact::
    cContact(eContactType type, const cData & data, cConstants * C):
        m_type(type),
        m_data(data),
        m_n_buffer(0),
        m_p_buffer(0),
        m_current(0),
        m_C(C)
    {

    }

    IBlock * cContact::
    next()
    {
        if (m_type == LEFT)
            return reinterpret_cast<IBlock *>(m_block);
        else
            return NULL;
    }

    IBlock * cContact::
    prev()
    {
        if (m_type == RIGHT)
            return reinterpret_cast<IBlock *>(m_block);
        else
            return NULL;
    }

    void cContact::
    set_next(IBlock * next)
    {
        assert (m_type == LEFT);
        m_block = reinterpret_cast<cBlock *>(next);

    }

    void cContact::
    set_prev(IBlock * prev)
    {
        assert (m_type == RIGHT);
        m_block = reinterpret_cast<cBlock *>(prev);
    }

    void cContact::
    injection()
    {
        double dc;
        //Sąlyga: kairėje elektronai, dešinėje skylės
        if (m_type == LEFT)
        {
            dc = m_data.m_E / m_C->c_k / m_C->c_q;
            //Tikslas padaryti kad el. laukas būtų nulinis
            if (dc < m_C->c_MIN)
            {
                //Jei dalelių sk neigiamas ar mažesnis nei minimumas, tai...
                dc = 0;
            }
            m_data.m_n += dc;
        }
        if (m_type == RIGHT)
        {
            dc = (m_C->m_U/m_data.m_width/m_C->m_size - m_data.m_E) / m_C->c_k / m_C->c_q;
            //Tikslas padaryti kad el. laukas būtų lygus įtampai.
            if (dc < m_C->c_MIN)
            {
                //Jei dalelių sk neigiamas ar mažesnis nei minimumas, tai...
                dc = 0;
            }
            m_data.m_p += dc;
        }
    }

    void cContact::
    recombination()
    {
        double pairs = m_data.m_n * m_data.m_p * m_C->c_beta * m_C->m_dt / m_C->c_S / m_data.m_width;
        if ((pairs > m_data.m_n)||(pairs > m_data.m_p))
            throw exception::TimeIntervalTooLarge(10, "Recombination");
        if (pairs > m_C->c_MIN)
        {
            m_data.m_n -= pairs;
            m_data.m_p -= pairs;
        }
    }

    void cContact::
    glue_unglue()
    {
        //TODO: wtf???
    }

    void cContact::
    relax()
    {
        //Čia rekombinacija ir krūvininkų prilipimas ir injekcija
        //injekcija
        injection();
        //rekombinacija irgi vyksta.
        recombination();
        //prilipimas, atlipimas... HMM???
        glue_unglue();
    }

    void cContact::
    field(double & E, double & U)
    {
        //Čia el. lauko skaičiavimas.
        double dE = m_C->c_k * m_C->c_q *    \
                    ((m_data.m_p + m_data.m_p_stuck) - \
                    (m_data.m_n + m_data.m_n_stuck));

        E += dE;
        m_data.m_E = E;
        U += dE*m_data.m_width;
    }

    void cContact::
    applyVoltage(double dE)
    {
        m_data.m_E += dE;
    }

    double cContact::
    diffusion(const double & interest,const double & neighbour, const double & D, std::string place)
    {
        double dc;
        dc = (interest - neighbour)*D*m_C->m_dt / (m_data.m_width*m_data.m_width);  //dc = D * deltaC * dt / dx^2
        if (dc>interest/5)
            throw exception::TimeIntervalTooLarge(10, std::string("Diffusion ") + place);
        if (dc < m_C->c_MIN)
            dc = 0;
        return dc;
    }

    double cContact::
    drift(const double & interest, const double & field, const double & miu, std::string place)
    {
        double dc;
        dc = miu * field * m_C->m_dt / m_data.m_width * interest;
        if (dc > interest/5)
            throw exception::TimeIntervalTooLarge(10, std::string("Drift ") + place);
        if (dc < m_C->c_MIN)
            dc = 0;
        return dc;
    }

    void cContact::
    emit()
    {
        m_current = 0;

        double  dn, dp;
        //Elektronai einantys į kaimyną
        dn = diffusion(m_data.m_n, m_block->read().m_n, m_C->c_n_D, "1");
        //Skylës einanèios į kaimyną
        dp = diffusion(m_data.m_p, m_block->read().m_p, m_C->c_p_D, "2");

        //Dreifas pagal kontaktą:
        if (m_type == LEFT)
        {
            //Juda elektronai
            dn += drift(m_data.m_n, m_data.m_E, m_C->c_n_miu, "1");
        }
        if (m_type == RIGHT)
        {
            //Juda skylės
            dp += drift(m_data.m_p, m_data.m_E, m_C->c_p_miu, "2");
        }

        //Isimam daleles.
        m_n_buffer -= dn;
        m_p_buffer -= dp;

        m_block->receive(dn,dp);
        //srovė pagal kontaktą:
        if (m_type == LEFT)
        {
            m_current+=dn-dp;
        }
        if (m_type == RIGHT)
        {
            m_current+=dp-dn;
        }

        return ;
    }

    void cContact::
    flush()
    {
        m_data.m_n += m_n_buffer;
        m_data.m_p += m_p_buffer;
        m_n_buffer = 0;
        m_p_buffer = 0;
    }

    void cContact::
    receive(const double & n, const double & p)
    {
        m_n_buffer += n;
        m_p_buffer += p;
    }


    const cData & cContact::
    read()
    {
        return m_data;
    }

    const cData & cContact::
    write(const cData & data)
    {
        m_data = data;
        return m_data;
    }

    const eStatus cContact::
    check()
    {
        return OK;
    }

    const double cContact::
    current()
    {
        return m_current;
    }
}
