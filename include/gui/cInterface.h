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

#include "../cMaterial.h"

#include "ui_gui.h"
#include <QtGui>
#include <QApplication>


namespace DoI
{
    class cInterface : public QWidget, private Ui::Form
    {
        Q_OBJECT

        private:
            QApplication * app;

            cMaterial * m_object;


        public:
            cInterface(QWidget *parent = 0);
            void connect(cMaterial *);
            void init(int *, char ***);
            int lounch();


        public slots:
            QString getPath();
            void doSomething();
            void clear();
            void about();
    };
};

#endif // CINTERFACE_H_INCLUDED
