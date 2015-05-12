/* ********************************************************
 * Author   :   Pascal Gendron
 * Filename :   I3CRenderingEngine.h
 * Creation :   May 11th 2015
 * Purpose  :   I3C Renderer (GPU Oriented)
 * Lisence  :   GNU General Public License
 *
 * Description:
 * TODO
 * *********************************************************/

#ifndef I3CRENDERINGENGINE_H
#define I3CRENDERINGENGINE_H

#include <string>
#include <GL/glut.h>

//DEBUG
#include <iostream>
//ENDDEBUG

#define LEFT_EYE    0
#define RIGHT_EYE   1

class I3CRenderingEngine
{
public:
    I3CRenderingEngine();
    ~I3CRenderingEngine();

    void openFile(std::string filename);

    void setFOV(float up, float down, float right, float left, int eye = 0);

    void setPosition(float x, float y, float z);
    void setOrientation(float yaw, float pitch, float roll);

    void render(GLuint texId, int eye = 0);
};

#endif // I3CRENDERINGENGINE_H
