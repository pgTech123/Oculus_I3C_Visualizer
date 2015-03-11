#include "renderingwidget.h"

RenderingWidget::RenderingWidget(QWidget *parent) :
    QWidget(parent)
{
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
}

bool RenderingWidget::openFile(const char* filename)
{
    cout << "Filename: " << filename << endl;
    return true;
}

void RenderingWidget::setRotation(double yaw, double pitch, double roll)
{
    //DEBUG
    cout << "yaw: "   << yaw << endl;
    cout << "pitch: " << pitch << endl;
    cout << "roll: "  << roll  << endl;
}


void RenderingWidget::setLeftEyePosition(double x, double y, double z)
{
    cout << "LX: " << x << endl;
    cout << "LY: " << y << endl;
    cout << "LZ: " << z << endl;
}

void RenderingWidget::setRightEyePosition(double x, double y, double z)
{
    cout << "RX: " << x << endl;
    cout << "RY: " << y << endl;
    cout << "RZ: " << z << endl;
}

void RenderingWidget::renderLeftEye()
{
    this->show();
}

void RenderingWidget::renderRightEye()
{
    this->show();
}

