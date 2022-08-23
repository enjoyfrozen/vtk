## Add Rotation and Dolly Models to vtkInteractorStyleTrackballCamera

Adds new rotation and dolly models. New trackball rotations allow for
new ways of mapping user screen movement into a trackball rotation.

The constrained modes are the result of user testing which shows
it is easier for a user to visualize their rotation and know how to
move within 3D space if the rotations are partially constrained. It
also makes it easier to move the model to a desired location without
accidentally adding an unwanted roll during the rotation.

There is also now a targetted dolly model where the camera will dolly
relative to the user's mouse location instead of the screen centre.

Some of these routines are implemented as utilities so they can be used
outside of an iteraction . This allows a developer to use rotations
and animation as part of more complex user interaction within their
application.

Single click interactions have also been added so more complex user
interaction can be combined into a single operating mode. If the mouse
has not moved between click and release, the user could be selecting
or interacting with an object instead of rotating. By combining this
with the rotations, we make user interaction more seamless by reducing
the number of operating modes a user must switch between to engage
with their model.
