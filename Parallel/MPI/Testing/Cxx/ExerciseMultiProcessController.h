/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

class vtkMultiProcessController;

// Runs the given multi process controller through the ropes.  Returns
// value is 0 on success (so that it may be passed back from the main
// application.
int ExerciseMultiProcessController(vtkMultiProcessController *controller);
