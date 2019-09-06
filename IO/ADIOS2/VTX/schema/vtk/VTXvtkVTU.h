/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef VTK_IO_ADIOS2_VTX_SCHEMA_VTK_VTXxmlVTU_h
#define VTK_IO_ADIOS2_VTX_SCHEMA_VTK_VTXxmlVTU_h

#include <map>
#include <string>
#include <vector>

#include "vtkNew.h"
#include "vtkUnstructuredGrid.h"

#include "VTX/schema/vtk/VTXvtkBase.h"

namespace vtx
{
namespace schema
{
class VTXvtkVTU : public VTXvtkBase
{
public:
  VTXvtkVTU(const std::string& schema, adios2::IO& io, adios2::Engine& engine);
  ~VTXvtkVTU();

private:
  /** Could be extended in a container, this is a per-rank ImageData */
  vtkNew<vtkUnstructuredGrid> UnstructuredGrid;

  /** BlockIDs carried by current rank */
  std::vector<size_t> BlockIDs;

  void DoFill(vtkMultiBlockDataSet* multiBlock, const size_t step) final;
  void ReadPiece(const size_t step, const size_t pieceID) final;

  void Init() final;

#define declare_type(T)                                                                            \
  void SetBlocks(adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step)     \
    final;
  VTK_IO_ADIOS2_VTX_ARRAY_TYPE(declare_type)
#undef declare_type

  template<class T>
  void SetBlocksCommon(
    adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step);
};

} // end namespace schema
} // end namespace vtx

#endif /* VTK_IO_ADIOS2_VTX_SCHEMA_VTK_VTXxmlVTU_h */
