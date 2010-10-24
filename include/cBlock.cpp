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

    /**cField**/

    cField::
    cField():
        m_next(NULL),
        m_prev(NULL),
        m_left(NULL),
        m_right(NULL),
        dE(0),
        supE(0),
        infE(0),
        E(0)
    {

    }

    cField * cField::
    next()
    {
        return m_next;
    }

    cField * cField::
    prev()
    {
        return m_prev;
    }

    void cField::
    set_next(cField * next)
    {
        m_next = next;
    }

    void cField::
    set_prev(cField * prev)
    {
        m_prev = prev;
    }

    void cField::
    attach_left(IBlock * left)
    {
        m_left = left;
    }

    void cField::
    attach_right(IBlock * right)
    {
        m_right = right;
    }

    double cField::
    calculate(double cumulative)
    {
        //Va tai tau. Einam nuo pradžios iki pabaigos ir suintegruojam.
        E += cumulative;
        if (m_next == NULL)
            return E;
        else
            return m_next->calculate(E);
    }

    double cField::
    integrate()
    {
        if (m_next == NULL)
            return E;
        else
            return E + m_next->integrate();
    }

    void cField::
    force_raise(double dE)
    {
        E += dE;
        supE += dE;
        infE += dE;
        if (m_next == NULL)
            return ;
        else
            return m_next->force_raise(dE);
    }

    void cField::
    iprint(std::ostream & out)
    {
        out << E << ' ';
        return m_next->iprint(out);
    }

    std::ostream & operator << (std::ostream & out, cField & F)
    {
        out << F.E << ' ' << F.supE << ' ' << F.infE << ' ' << F.dE;
        return out;
    }

    // Helpers for formulaes

    double diffusion(const double & interest,const double & neighbour, const double & D,
                      const cConstants * C, const cData & data, std::string place)
    {
/*
        double dc;
        dc = (interest - neighbour)*D*C->m_dt / (data.m_width*data.m_width);  //dc = D * deltaC * dt / dx^2
        if (dc>interest/5)
            throw exception::TimeIntervalTooLarge(10, std::string("Diffusion ") + place);
        if (dc < C->c_MIN)
            dc = 0;
        return dc;
*/
        return 0; //No diff
    }

    double drift(const double & interest, const double & field, const double & miu,
                  const cConstants * C, const cData & data, std::string place)
    {

        double dc;
        dc = miu * field * C->m_dt / data.m_width * interest;
        if (dc > interest/5)
            throw exception::TimeIntervalTooLarge(10, std::string("Drift ") + place);
        if (dc < C->c_MIN)
            dc = 0;
        return dc;

        //return 0;   //no drift
    }

    double recombine(const cConstants * C, const cData & data, std::string place)
    {
        double pairs;
        pairs = data.m_n * data.m_p * C->c_beta * C->m_dt / C->c_S / data.m_width;
        if ((pairs > data.m_n)||(pairs > data.m_p))
            throw exception::TimeIntervalTooLarge(10, std::string("Recombination ") + place);
        if (pairs < C->c_MIN)
        {
            pairs = 0;
        }
        return pairs;
    }

    /**cBlock**/


    cBlock::
    cBlock(const cData & data, cConstants * C):
        m_data(data),
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

    cField * cBlock::
    E_next()
    {
        return m_E_next;
    }

    cField * cBlock::
    E_prev()
    {
        return m_E_prev;
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

    void cBlock::
    set_E_next(cField * next)
    {
        m_E_next = next;
    }

    void cBlock::
    set_E_prev(cField * prev)
    {
        m_E_prev = prev;
    }

    //TODO: Čia guls visi skaičiavimai.

    void cBlock::
    recombination()
    {
        double pairs = recombine(m_C, m_data, "cBlock");
        m_data.m_n -= pairs;
        m_data.m_p -= pairs;
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
    field_create()
    {
        //Elektrinis laukas -- išdidus paukštis =]
        //Laukas įtakojamas pusės laukelio. Taip padariau kad nesidubliuotų krūvininkai.

        //Čia el. lauko, kurį sukuria blokelis, skaičiavimas.
        double dE = - m_C->c_k * m_C->c_q *    \
                    ((m_data.m_p + m_data.m_p_stuck) - \
                    (m_data.m_n + m_data.m_n_stuck));

        //Dabar dE turime padalinti per du kaimyninius E masyvo narius.
        //Tariame kad laukelis dešinėj turi būti nunulintas =]
        m_E_prev->dE += dE/2;
        m_E_next->dE = dE/2;
        //Integravimas turi įvykti pats =]
    }



    void cBlock::
    emit()
    {
        //Čia kumuliuojama srovė.
        m_current = 0;
        //Iš čia siunčiami krūvininkai kaimynams.

        double dn, dp;
        //Elektronai einantys á kairæ
        dn = diffusion(m_data.m_n, prev()->read().m_n, m_C->c_n_D, m_C, m_data, "1");
        //Skylës einanèios á kairæ
        dp = diffusion(m_data.m_p, prev()->read().m_p, m_C->c_p_D, m_C, m_data, "2");
        //Judėjimas dėl dreifo į kairę
        dp += drift(m_data.m_p, (m_E_prev->E + m_E_next->E)/2, m_C->c_p_miu, m_C, m_data, "1");
        //Elektronai juda į kairę kai laukas priešingas
        dn += drift(m_data.m_n, -(m_E_prev->E + m_E_next->E)/2, m_C->c_n_miu, m_C, m_data, "2");

        //Isimam daleles.
        m_data.m_n_buffer -= dn;
        m_data.m_p_buffer -= dp;

        prev()->receive(dn,dp);
        //srovė kuri sukuriama iš šio bloko:
        m_current += dp-dn; //Ji lokaliai gali būti ir neigiama.

        //Elektronai einantys á deðinæ
        dn = diffusion(m_data.m_n, next()->read().m_n, m_C->c_n_D, m_C, m_data, "3");
        //Skylës einanèios á deðinæ
        dp = diffusion(m_data.m_p, next()->read().m_p, m_C->c_p_D, m_C, m_data, "4");
        //Judėjimas dėl dreifo į dešinę
        dn += drift(m_data.m_n, (m_E_prev->E + m_E_next->E)/2, m_C->c_n_miu, m_C, m_data, "3");
        //Skylės juda į dešinę kai laukas priešingas
        dp += drift(m_data.m_p, -(m_E_prev->E + m_E_next->E)/2, m_C->c_p_miu, m_C, m_data, "4");

        //Isimam daleles.
        m_data.m_n_buffer -= dn;
        m_data.m_p_buffer -= dp;

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
        m_data.m_n += m_data.m_n_buffer;
        m_data.m_p += m_data.m_p_buffer;
        m_data.m_n_buffer = 0;
        m_data.m_p_buffer = 0;
    }

    void cBlock::
    receive(const double & n, const double & p)
    {
        m_data.m_n_buffer += n;
        m_data.m_p_buffer += p;
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

    /**cContact**/

    cContact::
    cContact(eContactType type, const cData & data, cConstants * C):
        m_type(type),
        m_data(data),
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

    cField * cContact::
    E_next()
    {
        return m_E_next;
    }

    cField * cContact::
    E_prev()
    {
        return m_E_prev;
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
    set_E_next(cField * next)
    {
        m_E_next = next;
    }

    void cContact::
    set_E_prev(cField * prev)
    {
        m_E_prev = prev;
    }

    void cContact::
    injection()
    {
        //INJEKCIJA
        //Dabar vyksta nevaldomai ir neaiškiai.

        double dc;
        //Sąlyga: kairėje elektronai, dešinėje skylės
        if (m_type == LEFT)
        {
            //Imame lauką kontakto kairėj, verčiam jį pasidaryti 0.
            //Imame lauką, kuris buvo skaičiuotas iš dešinės į kairę.
            dc = (m_E_prev->infE) / m_C->c_k / m_C->c_q;
            //Tikslas padaryti kad el. laukas būtų nulinis

            //m_E_prev->E = 0;

            if (dc < m_C->c_MIN)
            {
                //Jei dalelių sk neigiamas ar mažesnis nei minimumas, tai...
                dc = 0;
            }

            m_data.m_n_buffer += dc;
        }
        if (m_type == RIGHT)
        {
            //Imame lauką kontakto dešinėje, verčiam jį pasidaryti 0.
            //Imame lauką, kuris buvo skaičiuotas iš kairės į dešinę.
            dc = (m_E_next->supE) / m_C->c_k / m_C->c_q;

            //m_E_next->E = 0;

            if (dc < m_C->c_MIN)
            {
                //Jei dalelių sk neigiamas ar mažesnis nei minimumas, tai...
                dc = 0;
            }

            m_data.m_p_buffer += dc;
        }


    }

    void cContact::
    recombination()
    {
        double pairs = recombine(m_C, m_data, "cContact");
        m_data.m_n -= pairs;
        m_data.m_p -= pairs;
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
    field_create()
    {
        //Čia el. lauko skaičiavimas.
        double dE = - m_C->c_k * m_C->c_q *    \
                    ((m_data.m_p + m_data.m_p_stuck) - \
                    (m_data.m_n + m_data.m_n_stuck));

        //Laukas priklausomai nuo pusės kurion skaičiuoja =]
        //Skirtumas kad kairys kontaktas "nusinulina" lauko vertę
        if (m_type == LEFT)
        {
            m_E_prev->dE = dE/2;
            m_E_next->dE = dE/2;
        }
        if (m_type == RIGHT)
        {
            m_E_prev->dE += dE/2;
            m_E_next->dE = dE/2;
        }
    }


    void cContact::
    emit()
    {
        m_current = 0;

        double  dn, dp;
        //Elektronai einantys į kaimyną
        dn = diffusion(m_data.m_n, m_block->read().m_n, m_C->c_n_D, m_C, m_data, "1");
        //Skylës einanèios į kaimyną
        dp = diffusion(m_data.m_p, m_block->read().m_p, m_C->c_p_D, m_C, m_data, "2");

        //Dreifas pagal kontaktą:
        if (m_type == LEFT)
        {
            //Juda elektronai
            dn += drift(m_data.m_n, (m_E_prev->E + m_E_next->E)/2, m_C->c_n_miu, m_C, m_data, "1");
        }
        if (m_type == RIGHT)
        {
            //Juda skylės
            dp += drift(m_data.m_p, (m_E_prev->E + m_E_next->E)/2, m_C->c_p_miu, m_C, m_data, "2");
        }

        //Isimam daleles.
        m_data.m_n_buffer -= dn;
        m_data.m_p_buffer -= dp;

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
        m_data.m_n += m_data.m_n_buffer;
        m_data.m_p += m_data.m_p_buffer;
        m_data.m_n_buffer = 0;
        m_data.m_p_buffer = 0;
    }

    void cContact::
    receive(const double & n, const double & p)
    {
        m_data.m_n_buffer += n;
        m_data.m_p_buffer += p;
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
