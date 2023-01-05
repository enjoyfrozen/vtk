## Add vtkTexturedCapsuleSource

This implementation of the capsule adds texture coords for the capsule. The
current implementation differs from the existing implementation by using a
single sphere where the poles are aligned along the y-axis. The vtkCapsuleSource
uses two spheres which are rotated and each has their own poles parallel to the
normal of the cylinder body. This implementation also deprecates the
LatLongTessellation parameter as it generates the wireframe along the latitude
and longitude of the hemispheres which are connected with triangles. This
implementation is based on the vtkTexturedSphereSource.
