#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include "oculus.h"

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
    void on_pushButtonGo_clicked();

private:
    Ui::MainWindow *ui;

    Oculus *m_Oculus;
};

#endif // MAINWINDOW_H
