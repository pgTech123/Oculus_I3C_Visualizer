#include "renderingwidget.h"

RenderingWidget::RenderingWidget(QWidget *parent) :
    QWidget(parent)
{
    //Must be done here cause and in destructor cause other calls are not in the
    //same thread
    m_LabelRight = new QLabel();
    m_LabelLeft = new QLabel();
    m_HorizontalLayout = new QHBoxLayout();
    this->show();
}

RenderingWidget::~RenderingWidget()
{
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

    //Find a more appropriate place for this statement
    m_I3COculusEngine->openI3CFile(m_filename.c_str());

}

void RenderingWidget::setFilename(string filename)
{
    //We do not open file here as not in the same thread...
    m_filename = filename;
}

void RenderingWidget::setFOVLeft(float down, float up, float right, float left)
{
    float sinDwn = sin(down);
    float sinUp = sin(up);
    float sinRight = sin(right);
    float sinLeft = sin(left);

    m_RenderingScrLeftEye.left_rightRatio = sinLeft / (sinRight + sinLeft);
    m_RenderingScrLeftEye.up_downRatio = sinUp / (sinDwn + sinUp);
    m_RenderingScrLeftEye.focalLength = FOCAL_LENGTH;
}

void RenderingWidget::setFOVRight(float down, float up, float right, float left)
{
    float sinDwn = sin(down);
    float sinUp = sin(up);
    float sinRight = sin(right);
    float sinLeft = sin(left);

    m_RenderingScrRightEye.left_rightRatio = sinLeft / (sinRight + sinLeft);
    m_RenderingScrRightEye.up_downRatio = sinUp / (sinDwn + sinUp);
    m_RenderingScrRightEye.focalLength = FOCAL_LENGTH;
}

bool RenderingWidget::launchOculusEngine()
{
    m_I3COculusEngine = new I3COculusEngine();
    //TODO: Make this function safe
    return true;
}

void RenderingWidget::destroyOculusEngine()
{
    //TODO: make this function safe
    delete m_I3COculusEngine;
}

void RenderingWidget::setRotation(double yaw, double pitch, double roll)
{
    //DEBUG
    /*cout << "yaw: "   << yaw << endl;
    cout << "pitch: " << pitch << endl;
    cout << "roll: "  << roll  << endl;//*/
    m_I3COculusEngine->setRotation(-pitch, yaw, roll);
}


void RenderingWidget::setLeftEyePosition(double x, double y, double z)
{
    /*cout << "LX: " << x << endl;
    cout << "LY: " << y << endl;
    cout << "LZ: " << z << endl;//*/
    m_I3COculusEngine->setPosition(-x * MULTIPLICATION_FACTOR,
                                   y * MULTIPLICATION_FACTOR,
                                   z * MULTIPLICATION_FACTOR);
}

void RenderingWidget::setRightEyePosition(double x, double y, double z)
{
    /*cout << "RX: " << x << endl;
    cout << "RY: " << y << endl;
    cout << "RZ: " << z << endl;*/
    m_I3COculusEngine->setPosition(-x * MULTIPLICATION_FACTOR,
                                   y * MULTIPLICATION_FACTOR,
                                   z * MULTIPLICATION_FACTOR);
}

void RenderingWidget::renderLeftEye()
{
    m_I3COculusEngine->generateImage(&m_RenderingScrLeftEye);

    unsigned char *imageData = m_I3COculusEngine->getData();

    // 2 pixels width, 2 pixels height, 6 bytes per line, RGB888 format
    QImage img(imageData, m_iEyeWidth, m_iEyeHeight, 3*m_iEyeWidth, QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img); // Create pixmap from image

    // Set the this Class as the Pixel Map of the Rendered Image
    m_LabelRight->setPixmap(pix);
}

void RenderingWidget::renderRightEye()
{
    m_I3COculusEngine->generateImage(&m_RenderingScrRightEye);

    unsigned char *imageData = m_I3COculusEngine->getData();

    // 2 pixels width, 2 pixels height, 6 bytes per line, RGB888 format
    QImage img(imageData, m_iEyeWidth, m_iEyeHeight, 3*m_iEyeWidth, QImage::Format_RGB888);
    QPixmap pix = QPixmap::fromImage(img); // Create pixmap from image

    // Set the this Class as the Pixel Map of the Rendered Image
    m_LabelLeft->setPixmap(pix);
}

