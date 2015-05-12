/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.cpp
 * Creation :   May 11th 2015
 * Purpose  :   I3C Renderer (GPU Oriented)
 * Lisence  :   GNU General Public License
 *
 * Description:
 * TODO
 * *********************************************************/

#include "i3crenderingengine.h"

I3CRenderingEngine::I3CRenderingEngine()
{
}

I3CRenderingEngine::~I3CRenderingEngine()
{
}

void I3CRenderingEngine::openFile(std::string filename)
{

}

void I3CRenderingEngine::setFOV(float up, float down, float right, float left, int eye)
{

}

void I3CRenderingEngine::setPosition(float x, float y, float z)
{

}

void I3CRenderingEngine::setOrientation(float yaw, float pitch, float roll)
{

}

void I3CRenderingEngine::render(GLuint texId, int eye)
{
    glBindTexture(GL_TEXTURE_2D, texId);

    //Get Dimensions
    int w, h;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);

    float *arr = new float[w*h*4];
    for(int i = 0 ; i < w*h; i++){
        arr[i*4] = 0.5;
        arr[i*4+1] = 0.5;
        arr[i*4+2] = 0.5;
        arr[i*4+3] = 1.0;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, arr);
    delete[] arr;
}
