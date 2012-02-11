/*
 * =====================================================================================
 *
 *       Filename:  cEnvironment.cpp
 *
 *    Description:  implementation file
 *
 *        Version:  2.0
 *        Created:  2011-10-18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
#include "cEnvironment.h"
namespace DoI
{
    cEnvironment::cEnvironment(cConstants * constants, double time_step, uint64_t space_division, double width, CONTACTS_TYPE contacts, double capacity_n, double capacity_p, double time_offset):
    m_constants(constants), m_time_step(time_step), m_space_division(space_division), m_width(width), m_contacts(contacts), m_capacity_n(capacity_n), m_capacity_p(capacity_p),
    m_time(time_offset), m_backup_time(0), m_voltage(0)
    {

    }

    cConstants * cEnvironment::C()
    {
        return m_constants;
    }

    double cEnvironment::time_step()
    {
        return m_time_step;
    }

    void cEnvironment::time_step(double time_step)
    {
        m_time_step = time_step;
    }

    uint64_t cEnvironment::space_division()
    {
        return m_space_division;
    }

    void cEnvironment::space_division(uint64_t space_division)
    {
        m_space_division = space_division;
    }

    double cEnvironment::width()
    {
        return m_width;
    }
    void cEnvironment::width(double width)
    {
        m_width = width;
    }

    CONTACTS_TYPE cEnvironment::contacts()
    {
        return m_contacts;
    }

    void cEnvironment::contacts(CONTACTS_TYPE contacts)
    {
        m_contacts = contacts;
    }

    double cEnvironment::voltage()
    {
        return m_voltage;   //TODO =], get voltage from scenario parser.
    }

    void cEnvironment::voltage(double voltage)
    {
        m_voltage = voltage;
    }

    double cEnvironment::capacity_n()
    {
        return m_capacity_n;
    }

    double cEnvironment::capacity_p()
    {
        return m_capacity_p;
    }
}
