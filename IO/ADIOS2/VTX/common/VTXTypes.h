/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef VTK_IO_ADIOS2_VTX_COMMON_VTXTypes_h
#define VTK_IO_ADIOS2_VTX_COMMON_VTXTypes_h

#include <map>
#include <vector>

#include "VTXDataArray.h"

#include <adios2.h>

namespace vtx
{
namespace types
{

/** key: variable name, value: DataArray */
using DataSet = std::map<std::string, DataArray>;

enum class DataSetType
{
    CellData,
    PointData,
    Points,
    Coordinates,
    Cells,
    Verts,
    Lines,
    Strips,
    Polys
};

using Piece = std::map<DataSetType, DataSet>;

#define VTK_IO_ADIOS2_VTX_ARRAY_TYPE(MACRO)                                    \
    MACRO(int32_t)                                                             \
    MACRO(uint32_t)                                                            \
    MACRO(int64_t)                                                             \
    MACRO(uint64_t)                                                            \
    MACRO(float)                                                               \
    MACRO(double)

} // end namespace types
} // end namespace vtx

#endif /* VTK_IO_ADIOS2_VTX_COMMON_VTXTypes_h */
