/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/


#ifndef VTK_IO_ADIOS2_VTX_SCHEMA_VTXSchema_h
#define VTK_IO_ADIOS2_VTX_SCHEMA_VTXSchema_h

#include <map>
#include <string>

#include "vtkMultiBlockDataSet.h"

#include <adios2.h>

#include "VTX/common/VTXDataArray.h"
#include "VTX/common/VTXTypes.h"

namespace vtx
{
/**
 * Abstract common class to supported ADIOS2 schemas
 */
class VTXSchema
{
public:
  /** carries schema type from derived class */
  const std::string Type;

  /** schema contents as a single string */
  std::string Schema;

  /**
   * Stored times and corresponding steps
   * <pre>
   * 	key: physical times
   * 	value: adios2 step
   * </pre>
   */
  std::map<double, size_t> Times;

  /**
   * Generic base constructor
   * @param type from derived class
   * @param schema as input
   * @param io manages IO input containing variable information
   * @param engine manages stream input
   */
  VTXSchema(
    const std::string type, const std::string& schema, adios2::IO& io, adios2::Engine& engine);

  // can't use = default, due to forward class not defined
  virtual ~VTXSchema();

  /**
   * Fills multiblock data from request steps
   * @param multiBlock output structure
   * @param step input adios2 step
   */
  void Fill(vtkMultiBlockDataSet* multiBlock, const size_t step = 0);

protected:
  adios2::IO& IO;
  adios2::Engine& Engine;

  virtual void Init() = 0;
  virtual void InitTimes() = 0;

  virtual void DoFill(vtkMultiBlockDataSet* multiBlock, const size_t step) = 0;
  virtual void ReadPiece(const size_t step, const size_t pieceID) = 0;

  void GetTimes(const std::string& variableName = "");
  void GetDataArray(
    const std::string& variableName, types::DataArray& dataArray, const size_t step = 0);

#define declare_type(T)                                                                            \
  virtual void SetDimensions(                                                                      \
    adios2::Variable<T> variable, const types::DataArray& dataArray, const size_t step);           \
                                                                                                   \
  virtual void SetBlocks(                                                                          \
    adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step);

  VTK_IO_ADIOS2_VTX_ARRAY_TYPE(declare_type)
#undef declare_type

private:
  template<class T>
  void GetDataArrayCommon(
    adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step);

  template<class T>
  void GetDataArrayGlobal(
    adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step);

  template<class T>
  void GetDataArrayLocal(
    adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step);

  template<class T>
  void GetDataValueGlobal(
    adios2::Variable<T> variable, types::DataArray& dataArray, const size_t step);

  template<class T>
  void GetTimesCommon(const std::string& variableName);

  template<class T>
  void InitDataArray(const std::string& name, const size_t elements, const size_t components,
    types::DataArray& dataArray);
};

} // end namespace vtx

#endif /* VTK_IO_ADIOS2_VTX_SCHEMA_VTXSchema_h */
