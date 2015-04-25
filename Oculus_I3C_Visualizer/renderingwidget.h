#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QWidget>
#include <QDesktopWidget>
#include <QApplication>
#include <QRect>
#include <QLabel>
#include <QHBoxLayout>
#include <QImage>
#include <QPixmap>
#include <math.h>

#include "I3COculusEngine/i3coculusengine.h"


#include <iostream>
#include <string>
using namespace std;


#define FOCAL_LENGTH            600
#define MULTIPLICATION_FACTOR   600

//TODO: on_escape_click -> close this window

class RenderingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderingWidget(QWidget *parent = 0);
    virtual ~RenderingWidget();

    void setScreenResolution(int width, int height);    //WARNING: MUST BE CALLED BEFORE setFOV___()
    void setFilename(string filename);
    void setFOVLeft(float down, float up, float right, float left);
    void setFOVRight(float down, float up, float right, float left);

    bool launchOculusEngine();
    void destroyOculusEngine();

    void setRotation(double yaw, double pitch, double roll);

    void setLeftEyePosition(double x, double y, double z);
    void setRightEyePosition(double x, double y, double z);

    void renderLeftEye();
    void renderRightEye();

signals:

public slots:

private:
    I3COculusEngine *m_I3COculusEngine;
    RenderingScreen m_RenderingScrLeftEye;
    RenderingScreen m_RenderingScrRightEye;
    RenderingScreen m_CurrentRenderingScreen;

    QHBoxLayout *m_HorizontalLayout;
    QLabel *m_LabelRight;
    QLabel *m_LabelLeft;

    int m_iEyeWidth;
    int m_iEyeHeight;

    string m_filename;

    float m_FOV_IN;
    float m_FOV_OUT;
    float m_FOV_UP;
    float m_FOV_DOWN;

};

#endif // RENDERINGWIDGET_H
