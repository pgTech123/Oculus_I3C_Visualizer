/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   mainwindow.cpp
 * Creation :   May 9th 2015
 * Purpose  :   Manage main UI
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class (MainWindow) is used to manage the UI and call
 * appropriate functions depending on the user actions.
 * *********************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedWidth(400);
    this->setFixedHeight(200);

    ui->pushButtonGo->setEnabled(false);

    m_OculusApp = new OculusApp();
    this->initOculusDevice();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    this->shutdownOculusDevice();
}

void MainWindow::initOculusDevice()
{
    int error = m_OculusApp->initOculusDevice();
    if(error == OCULUS_NO_ERROR){
        ui->pushButtonGo->setEnabled(true);
        ui->labelStatus->setText("Device Found");
    }
    else{
        ui->pushButtonGo->setEnabled(false);
        ui->labelStatus->setText("Device Not Found");
    }
}

void MainWindow::shutdownOculusDevice()
{
    if(m_OculusApp != NULL){
        if(ui->pushButtonGo->isEnabled()){
            m_OculusApp->shutdownOculusDevice();
        }
        delete m_OculusApp;
        m_OculusApp = NULL;
    }
}

void MainWindow::on_refresh_clicked()
{
    if(ui->pushButtonGo->isEnabled()){
        m_OculusApp->shutdownOculusDevice();
    }
    this->initOculusDevice();
}

void MainWindow::on_load_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Load 3D Image", QString(),
                                                    "3D Image(*.i3c)");
    if(path == NULL){   //Cancel Button Pressed
        return;
    }
    ui->lineEditPath->setText(path);
}

void MainWindow::on_pushButtonGo_clicked()
{
    std::string filePath = ui->lineEditPath->text().toStdString();
    m_OculusApp->startRendering(filePath);
}
