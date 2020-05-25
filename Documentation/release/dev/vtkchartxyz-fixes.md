# vtkChartXYZ fixes

1. Allow removing plots without segfaulting later on.
2. Fix distortion when rotating (box was not parallel to the content anymore ...)
3. Allow to pan, zoom in on some detail, then rotate while still keeping focus on that same detail.
4. Allow to disable clipping panes, this way data doesnt hide out of sight.
5. Trigger repaint when setting geometry.
6. Made automatic rescaling of the axes optional. It can be seen as confusing when the whole scales up or down when rotating.
7. Widget actually shows and properly updates when the view changes.
8. Allow for scaling the plot together with the box, less confusing for the eye.
9. Allow the user to set margins (in which axis decorations can appear) by setting geometry.


First implemented and tested on vtk 8.2 using qtopenglnativewidget, works great there.
Works on master too BUT somehow all my text turned into black boxes, my surface-plot doesnt want to be opaque anymore ... and clicking on the widget makes the whole qtwindow draggable. Those issues are probably unrelated.
