#ifndef vtkDGOperation_h
#define vtkDGOperation_h

#include "vtkDGCell.h"

VTK_ABI_NAMESPACE_BEGIN

class vtkDoubleArray;

/**\brief Invoke an operator on DG cells/sides.
  *
  */
class vtkDGOperation
{
public:
  /// Construct an operation object.
  vtkDGOperation();
  vtkDGOperation(vtkDGCell* cellType, vtkCellAttribute* cellAttribute, vtkStringToken operationName);

  /// Signature for a method to evaluate data on a single vtkDGCell::Source instance.
  using CellRangeEvaluator =
    std::function<void(vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result,
      vtkTypeUInt64 begin, vtkTypeUInt64 end)>;

  /// Prepare this instance of vtkDGOperation to evaluate \a operationName on the given \a cellType
  /// and \a cellAttribute.
  ///
  /// This populates the Evaluators ivar with functors valid for a range of cells corresponding
  /// to non-blanked vtkDGCell::Source instances.
  ///
  /// This method returns true upon success and false otherwise.
  /// This method returns false if \a operationName does not name an operator; other inputs
  /// are null pointers; or the cell attribute does not provide information for the given
  /// cell type.
  ///
  /// You should not call Evaluate() if Prepare() returns false.
  ///
  /// If \a includeShape is true (the default), then any transformation of \a cellAttribute
  /// by the grid's shape-attribute that is needed will be factored into the evaluators.
  /// This avoids a double-lookup expense (i.e., once for \a cellAttribute and again for the
  /// shape attribute) when determining which entry in this->Evaluators to invoke for each ID.
  ///
  /// For HGrad function spaces, \a includeShape has no effect. For HDiv and HCurl function
  /// spaces, this will transform vector values.
  bool Prepare(
    vtkDGCell* cellType, vtkCellAttribute* cellAttribute, vtkStringToken operationName,
    bool includeShape = true);

  /// Evaluate the prepared operator on the given \a cellIds at the given \a rst parameters,
  /// storing results in the \a result array.
  ///
  /// This method returns true upon success and false otherwise.
  /// If false is returned, partial results may have been written to \a result
  /// (for example, if an invalid cell ID is encountered after others have been
  /// processed).
  bool Evaluate(vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result);

  /// Return a function that can be called on the named \a sideSpecId.
  ///
  /// This method may return a null evaluator if \a sideSpecId does not exist
  /// or if the \a sideSpecId is blanked.
  ///
  /// If \a sideSpecId == -1, then a function for \a cell->GetCellSpec()
  /// is returned (assuming the cells are not blanked).
  CellRangeEvaluator GetEvaluatorForSideSpec(vtkDGCell* cell, int sideSpecId);

protected:
  struct RangeKey
  {
    vtkTypeUInt64 Begin;
    vtkTypeUInt64 End;
    bool Contains(vtkTypeUInt64 cellId) const;
    bool ContainedBy(const RangeKey& other) const;
    bool operator < (const RangeKey& other) const;
  };

  void AddSource(
    vtkCellGrid* grid,
    vtkDGCell* cellType, std::size_t sideSpecIdx,
    vtkCellAttribute* cellAtt, const vtkCellAttribute::CellTypeInfo& cellTypeInfo,
    vtkDGOperatorEntry& op, bool includeShape);

  /// The integer key of each entry specifies the starting range
  std::map<RangeKey, CellRangeEvaluator> Evaluators;
};

VTK_ABI_NAMESPACE_END
#endif // vtkDGOperation_h
