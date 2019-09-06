/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef VTK_IO_ADIOS2_VTX_COMMON_VTXDataArray_h
#define VTK_IO_ADIOS2_VTX_COMMON_VTXDataArray_h

#include "vtkDataArray.h"
#include "vtkSmartPointer.h"

#include <map>

#include <adios2.h>

namespace vtx
{
namespace types
{

class DataArray
{
public:
    std::vector<std::string> VectorVariables;
    vtkSmartPointer<vtkDataArray> Data;

    // required for global arrays
    adios2::Dims Shape;
    adios2::Dims Start;
    adios2::Dims Count;

    // required for local arrays, using maps for now
    /** key: blockID, value: block count */
    std::map<size_t, adios2::Dims> BlockCounts;

    /** true : uses the special vtkIdType for indexing
     *  false : uses other VTK supported type */
    bool IsIdType = false;

    /** true: tuples > 1, false: tuples = 1 */
    bool HasTuples = false;

    /**
     *  true: if variable doesn't exist in a step don't attempt to read,
     *  use the latest known values
     *  false: attempt to read always
     */
    bool Persist = false;

    DataArray() = default;
    ~DataArray() = default;

    bool IsScalar() const noexcept;
};

} // end namespace types
} // end namespace vtx

#endif /* VTK_IO_ADIOS2_VTX_COMMON_VTXDataArray_h */
