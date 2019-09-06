/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef VTK_IO_ADIOS2_VTX_SCHEMA_VTK_VTXvtkBase_h
#define VTK_IO_ADIOS2_VTX_SCHEMA_VTK_VTXvtkBase_h

#include "VTX/common/VTXTypes.h"
#include "VTX/schema/VTXSchema.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace vtx
{
namespace schema
{

class VTXvtkBase : public VTXSchema
{
public:
  VTXvtkBase(
    const std::string type, const std::string& schema, adios2::IO& io, adios2::Engine& engine);

  // can't use = default, due to forward class not defined
  virtual ~VTXvtkBase();

protected:
  std::vector<types::Piece> Pieces;

  const static std::set<std::string> TIMENames;
  const static std::set<std::string> SpecialNames;
  const static std::map<types::DataSetType, std::string> DataSetTypes;

  virtual void DoFill(vtkMultiBlockDataSet* multiBlock, const size_t step) = 0;
  virtual void ReadPiece(const size_t step, const size_t pieceID) = 0;

  bool ReadDataSets(const types::DataSetType type, const size_t step, const size_t pieceID);

  virtual void Init() = 0;
  void InitTimes() final;

  std::string DataSetType(const types::DataSetType type) const noexcept;
};

} // end namespace schema
} // end namespace vtx

#endif /* VTK_IO_ADIOS2_VTX_SCHEMA_VTK_VTXvtkBase_h */
