/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   mainwindow.h
 * Creation :   May 9th 2015
 * Purpose  :   Manage main UI
 * Lisence  :   GNU General Public License
 *
 * Description:
 * This class (MainWindow) is used to manage the UI and call
 * appropriate functions depending on the user actions.
 * *********************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QCloseEvent>
#include <string>
#include "oculusapp.h"

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

private:
    //Try to find Oculus Device and update UI if found/not found
    void initOculusDevice();

    //Shutdown cleanly the Oculus Software
    void shutdownOculusDevice();

private slots:
    void on_refresh_clicked();
    void on_load_clicked();
    void on_pushButtonGo_clicked();

private:
    Ui::MainWindow *ui;

    OculusApp *m_OculusApp;
};

#endif // MAINWINDOW_H
