#include "renderingwidget.h"

RenderingWidget::RenderingWidget(QWidget *parent) :
    QWidget(parent)
{
    //Must be done here cause and in destructor cause other calls are not in the
    //same thread
    m_I3COculusEngine = new I3COculusEngine();
    m_LabelRight = new QLabel();
    m_LabelLeft = new QLabel();
    m_HorizontalLayout = new QHBoxLayout();
    this->show();
}

RenderingWidget::~RenderingWidget()
{
    delete m_I3COculusEngine;
    delete m_LabelLeft;
    delete m_LabelRight;
    delete m_HorizontalLayout;
}

void RenderingWidget::setScreenResolution(int width, int height)
{
    m_iEyeWidth = width/2;
    m_iEyeHeight = height;
    if(m_I3COculusEngine->setImageSize(m_iEyeWidth, m_iEyeHeight)){
        cout << "OK!" << endl;
    }
    else{
        cout << "NOT OK :(" << endl;
        return;
    }

    //This part of code considers that the second screen is the oculus
    //This can be optimized to be more flexible.
    QRect screenres = QApplication::desktop()->screenGeometry(1);
    this->move(QPoint(screenres.x(), screenres.y()));

    m_LabelRight->setFixedSize(m_iEyeWidth, m_iEyeHeight);
    m_LabelLeft->setFixedSize(m_iEyeWidth, m_iEyeHeight);

    QPixmap leftPixmap(m_iEyeWidth,m_iEyeHeight);
    leftPixmap.fill(Qt::black);
    m_LabelLeft->setPixmap(leftPixmap);

    QPixmap rightPixmap(m_iEyeWidth,m_iEyeHeight);
    rightPixmap.fill(Qt::red);
    m_LabelRight->setPixmap(rightPixmap);

    m_HorizontalLayout->addWidget(m_LabelLeft);
    m_HorizontalLayout->addWidget(m_LabelRight);
    this->setLayout(m_HorizontalLayout);

    this->showFullScreen();

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
    //m_I3COculusEngine->generateImage();
    //TODO: get data
}

void RenderingWidget::renderRightEye()
{

}

