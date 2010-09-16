/*
 * =====================================================================================
 *
 *       Filename:  cInterface.h
 *
 *    Description:  Class which draws the pretty pictures =]
 *
 *        Version:  2.0
 *        Created:  2010-09-14-22.04
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Vytis Valentinavičius (), vytis.valentinavicius@gmail.com
 *        Company:  VU FF
 *
 * =====================================================================================
 */
#ifndef CINTERFACE_H_INCLUDED
#define CINTERFACE_H_INCLUDED

#include "cMaterial.h"

namespace DoI
{
    class cInterface
    {
        private:
            cMaterial * m_object;

        protected:
        public:
            cInterface();
            void connect(cMaterial *);
            void init();
            void redraw();
            ~cInterface();
    };
};

#endif // CINTERFACE_H_INCLUDED
