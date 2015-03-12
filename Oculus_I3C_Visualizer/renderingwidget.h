#ifndef RENDERINGWIDGET_H
#define RENDERINGWIDGET_H

#include <QWidget>
#include <QDesktopWidget>

#include "I3COculusEngine/i3coculusengine.h"

// DEBUG PURPOSE ONLY
#include <iostream>
using namespace std;
// END DEBUG PURPOSE

class RenderingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit RenderingWidget(QWidget *parent = 0);
    virtual ~RenderingWidget();

    bool openFile(const char* filename);

    void setRotation(double yaw, double pitch, double roll);

    void setLeftEyePosition(double x, double y, double z);
    void setRightEyePosition(double x, double y, double z);

    void renderLeftEye();
    void renderRightEye();

signals:

public slots:

private:
    I3COculusEngine *m_I3COculusEngine;

};

#endif // RENDERINGWIDGET_H
