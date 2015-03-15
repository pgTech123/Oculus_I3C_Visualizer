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

#include "I3COculusEngine/i3coculusengine.h"

// DEBUG PURPOSE ONLY
#include <iostream>
using namespace std;
// END DEBUG PURPOSE

//TODO: on_escape_click -> close this window

class RenderingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderingWidget(QWidget *parent = 0);
    virtual ~RenderingWidget();

    void setScreenResolution(int width, int height);
    void setFilename(const char* filename);
    void setFOV(float down, float up, float right, float left);

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

    QHBoxLayout *m_HorizontalLayout;
    QLabel *m_LabelRight;
    QLabel *m_LabelLeft;

    int m_iEyeWidth;
    int m_iEyeHeight;

    const char *m_filename;

};

#endif // RENDERINGWIDGET_H
