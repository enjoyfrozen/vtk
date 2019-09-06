/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef vtkHyperTreeGridTools_h
#define vtkHyperTreeGridTools_h


namespace vtk {
namespace hypertreegrid {

template < class T >
bool HasTree( const T & e )
{
  return e.GetTree() != 0;
}

}// namespace hypertreegrid
}// namespace vtk

#endif // vtHyperTreeGridTools_h
// VTK-HeaderTest-Exclude: vtkHyperTreeGridTools.h
