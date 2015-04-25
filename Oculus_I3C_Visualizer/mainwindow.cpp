#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedWidth(400);
    this->setFixedHeight(200);

    m_Oculus = new Oculus();
    initOculus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    if(m_bOculusFound){
        m_Oculus->shutdownOculus();
    }
    delete m_Oculus;
}

//Warning: Must be called at least once in the execution
void MainWindow::initOculus()
{
    int error = m_Oculus->initOculus();
    if(error == OCULUS_NO_ERROR){
        m_bOculusFound = true;
        ui->labelStatus->setText("Device Found");
    }
    else{
        m_bOculusFound = false;
        ui->labelStatus->setText("Device Not Found");
    }
}

void MainWindow::on_refresh_clicked()
{
    if(m_bOculusFound){
        m_Oculus->shutdownOculus();
    }
    initOculus();
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
    string filePath = ui->lineEditPath->text().toStdString();
    m_Oculus->render(filePath);
}
