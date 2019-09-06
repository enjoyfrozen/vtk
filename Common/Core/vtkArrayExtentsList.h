/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

/**
 * @class   vtkArrayExtentsList
 * @brief   Stores a collection of vtkArrayExtents objects.
 *
 *
 * vtkArrayExtentsList provides storage for a collection of vtkArrayExtents
 * instances.  Constructors are provided for creating collections
 * containing one, two, three, or four slices.  To work with larger
 * numbers of slices, use the default constructor, the SetCount() method,
 * and operator[].
 *
 * vtkArrayExtentsList is most commonly used with the vtkInterpolate()
 * function, which is used to computed weighted sums of vtkArray slices.
 *
 * @sa
 * vtkArray, vtkExtents
 *
 * @par Thanks:
 * Developed by Timothy M. Shead (tshead@sandia.gov) at Sandia National
 * Laboratories.
*/

#ifndef vtkArrayExtentsList_h
#define vtkArrayExtentsList_h

#include "vtkCommonCoreModule.h" // For export macro
#include "vtkArrayExtents.h"
#include <vector> // STL Header

class VTKCOMMONCORE_EXPORT vtkArrayExtentsList
{
public:
  /**
   * Creates an empty collection of slices.
   */
  vtkArrayExtentsList();

  /**
   * Creates a collection containing one slice.
   */
  vtkArrayExtentsList(const vtkArrayExtents& i);

  /**
   * Creates a collection containing two slices.
   */
  vtkArrayExtentsList(const vtkArrayExtents& i, const vtkArrayExtents& j);

  /**
   * Creates a collection containing three slices.
   */
  vtkArrayExtentsList(const vtkArrayExtents& i, const vtkArrayExtents& j, const vtkArrayExtents& k);

  /**
   * Creates a collection containing four slices.
   */
  vtkArrayExtentsList(const vtkArrayExtents& i, const vtkArrayExtents& j, const vtkArrayExtents& k, const vtkArrayExtents& l);

  /**
   * Returns the number of slices stored in this collection.
   */
  vtkIdType GetCount() const;

  /**
   * Sets the number of extents stored in this collection.  Note: all
   * extents will be empty after calling SetCount(), use operator[]
   * to assign extents to each item in the collection.
   */
  void SetCount(vtkIdType count);


  /**
   * Accesses the i-th slice.
   */
  vtkArrayExtents& operator[](vtkIdType i);

  /**
   * Accesses the i-th slice.
   */
  const vtkArrayExtents& operator[](vtkIdType i) const;

private:

  std::vector<vtkArrayExtents> Storage;

};

#endif

// VTK-HeaderTest-Exclude: vtkArrayExtentsList.h
