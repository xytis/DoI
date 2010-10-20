#include "cInterface.h"


namespace DoI
{
    cInterface::
    cInterface(QWidget *parent):
    app(NULL)
    {
        setupUi(this); // this sets up GUI

        // signals/slots mechanism in action
        QObject::connect( pushButton_browse, SIGNAL( clicked() ), this, SLOT( getPath() ) );
        QObject::connect( pushButton_run, SIGNAL( clicked() ), this, SLOT( doSomething() ) );
        QObject::connect( pushButton_clear, SIGNAL( clicked() ), this, SLOT( clear() ) );
        QObject::connect( pushButton_about, SIGNAL( clicked() ), this, SLOT( about() ) );
    }

    void cInterface::
    connect(cMaterial * object)
    {
        m_object = object;
    }







    QString cInterface::
    getPath()
    {
        QString path;

        path = QFileDialog::getOpenFileName(
            this,
            "Choose a constants or dump file to open",
            QString::null,
            QString::null);

        lineEdit->setText( path );
        return path;
    }


    void cInterface::
    doSomething()
    {
        int value1, value2;
        Qt::CheckState state;
        QString str;

        textEdit->append( "Path to file: " + lineEdit->text() );

        value1 = spinBox1->value();
        value2 = spinBox2->value();

        textEdit->append( "Number 1 value: " + QString::number(value1) );
        textEdit->append( "Number 2 value: " + QString::number(value2) );

        state = checkBox->checkState();

        str = "Checkbox says: ";
        if ( state == Qt::Checked ) str += "yes";
        else str += "no";
        textEdit->append( str );

        textEdit->append( "ComboBox current text: " + comboBox->currentText() );
        textEdit->append( "ComboBox current item: " + QString::number(comboBox->currentIndex()) );
    }


    void cInterface::
    clear()
    {
        textEdit->clear();
    }


    void cInterface::
    about()
    {
        QMessageBox::about(this,"About XSim",
                    "This app was coded for educational purposes.\n"
                    "Number 1 is: " + QString::number(spinBox1->value()) + "\n\n"
                    "Bye.\n");
    }
};
