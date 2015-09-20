# Oculus_I3C_Visualizer
This is a pilot project trying to display 3D image (.i3c) through the Oculus Rift with a Qt UI.

This project is developed on Windows 7 using:
- Oculus SDK 0.5.0 Beta for Windows
- Qt MVC2010_Opengl Version 5.2.1
- OpenCL 1.2


Instructions: 
Move cl_program.cl next to the executable.

Known issue: 
- OpenCL is hardcoded to start on the first platform available; it might not be the one on which OpenGL runs. If it's not the one on which OpenGL runs, the program will crash. 
- Problem in the rendering: reason still unknown...
- Approximation in the rendering (that needs to be considered after all): cubes are not necessarily ordered uniform across the image. This might cause weird parts of the rendered image.
