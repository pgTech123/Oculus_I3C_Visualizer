#include "renderingwidget.h"

RenderingWidget::RenderingWidget(QWidget *parent) :
    QWidget(parent)
{
    m_I3COculusEngine = new I3COculusEngine();

    //Fill the oculus
    //TODO
    this->setFixedHeight(100);
    this->setFixedWidth(100);

    //Display in the Oculus
    //TODO
    QDesktopWidget asd;
    cout << asd.primaryScreen() << endl;
}

RenderingWidget::~RenderingWidget()
{
    delete m_I3COculusEngine;
}

bool RenderingWidget::openFile(const char* filename)
{
    cout << "Filename: " << filename << endl;
    m_I3COculusEngine->openI3CFile(filename);
    return true;
}

void RenderingWidget::setRotation(double yaw, double pitch, double roll)
{
    //DEBUG
    cout << "yaw: "   << yaw << endl;
    cout << "pitch: " << pitch << endl;
    cout << "roll: "  << roll  << endl;
    m_I3COculusEngine->setRotation(yaw, pitch, roll);
}


void RenderingWidget::setLeftEyePosition(double x, double y, double z)
{
    cout << "LX: " << x << endl;
    cout << "LY: " << y << endl;
    cout << "LZ: " << z << endl;
    //TODO
}

void RenderingWidget::setRightEyePosition(double x, double y, double z)
{
    cout << "RX: " << x << endl;
    cout << "RY: " << y << endl;
    cout << "RZ: " << z << endl;
    //TODO
}

void RenderingWidget::renderLeftEye()
{
    m_I3COculusEngine->generateImage();
    //TODO: get data
    this->show();
}

void RenderingWidget::renderRightEye()
{
    this->show();
}

