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

    m_Oculus->initOculus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent*)
{
    m_Oculus->shutdownOculus();
    delete m_Oculus;
}

void MainWindow::on_pushButtonGo_clicked()
{
    m_Oculus->render("Test");
}
