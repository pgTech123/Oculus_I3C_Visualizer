#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QCloseEvent>
#include "oculus.h"

#include <iostream>
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent*);

private slots:
    void on_refresh_clicked();
    void on_load_clicked();
    void on_pushButtonGo_clicked();

private:
    Ui::MainWindow *ui;

    Oculus *m_Oculus;
    bool m_bOculusFound;
};

#endif // MAINWINDOW_H
