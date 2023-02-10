## Add 3D Cursor Widget

The vtk3DCursorWidget and vtk3DCursorRepresentation are added to VTK.
The 3D cursor follows the mouse. It's placed on the surface of the actors in the scene if
the mouse hovers them, or on the focal plane of the camera otherwise. Do not work with volumes.
This widget is intended to be used when using VTK with a stereo display, in place of the 2D mouse cursor.
