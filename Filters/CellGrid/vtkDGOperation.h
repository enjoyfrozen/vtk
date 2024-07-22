#ifndef vtkDGOperation_h
#define vtkDGOperation_h

#include "vtkDGCell.h"

#include <array>

VTK_ABI_NAMESPACE_BEGIN

class vtkDoubleArray;

/**\brief Invoke an operator on DG cells/sides.
  *
  */
class VTKFILTERSCELLGRID_EXPORT vtkDGOperation
{
public:
  /// A range of cell IDs handled by a vtkDGCell::Source instance.
  struct RangeKey
  {
    vtkTypeUInt64 Begin;
    vtkTypeUInt64 End;
    bool Contains(vtkTypeUInt64 cellId) const;
    bool ContainedBy(const RangeKey& other) const;
    bool operator < (const RangeKey& other) const;
  };

  /// Signature for a method to evaluate data on a single vtkDGCell::Source instance.
  using CellRangeEvaluator =
    std::function<void(vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result,
      vtkTypeUInt64 begin, vtkTypeUInt64 end)>;

  struct EvaluatorEntry;

  /// Encapsulate the state required to evaluate DG cell-attributes.
  ///
  /// This object holds input vtkDGOperatorEntry objects, input array
  /// pointers, and working-space tuples (std::vector/std::array ivars)
  /// required to evaluate a single vtkCellAttribute on cells corresponding
  /// to a single vtkDGCell::Source entry. vtkDGOperation holds one instance
  /// of EvaluationState for each vtkDGCell::Source entry in a particular
  /// vtkDGCell instance with arrays populated by a single vtkCellAttribute.
  class EvaluationState
  {
  public:
    EvaluationState(
      // Attribute arrays/operation
      vtkDGOperatorEntry& op,
      vtkDataArray* connectivity,
      vtkDataArray* values,
      vtkDataArray* sideConn,
      vtkTypeUInt64 offset,
      // Shape arrays/operation
      vtkDGOperatorEntry shapeGradient = vtkDGOperatorEntry(),
      vtkDataArray* shapeConnectivity = nullptr,
      vtkDataArray* shapeValues = nullptr)
      : OpEntry(op)
      , CellConnectivity(connectivity)
      , CellValues(values)
      , SideConnectivity(sideConn)
      , Offset(offset)
      , ShapeGradientEntry(shapeGradient)
      , ShapeConnectivity(shapeConnectivity)
      , ShapeValues(shapeValues)
    {
    }
    EvaluationState(const EvaluationState& other) = default;

    virtual void CloneInto(EvaluatorEntry& entry) const = 0;

    vtkDGOperatorEntry OpEntry;
    vtkDataArray* CellConnectivity;
    vtkDataArray* CellValues;
    vtkDataArray* SideConnectivity;
    vtkTypeUInt64 Offset;
    mutable std::array<vtkTypeUInt64, 2> SideTuple;
    mutable std::array<double, 3> RST{{ 0, 0, 0 }};
    mutable std::vector<vtkTypeUInt64> ConnTuple;
    mutable std::vector<double> ValueTuple;
    mutable std::vector<double> BasisTuple;
    mutable vtkTypeUInt64 LastCellId{ ~0ULL };
    mutable int NumberOfValuesPerFunction{ 0 };

    vtkDGOperatorEntry ShapeGradientEntry;
    vtkDataArray* ShapeConnectivity;
    vtkDataArray* ShapeValues;
    mutable std::vector<vtkTypeUInt64> ShapeConnTuple;
    mutable std::vector<double> ShapeValueTuple;
    mutable std::vector<double> ShapeBasisTuple;
    mutable std::vector<double> Jacobian;
    mutable int NumberOfShapeValuesPerFunction{ 0 };
    mutable vtkTypeUInt64 LastShapeCellId{ ~0ULL };
  };

  struct EvaluatorEntry
  {
    EvaluatorEntry() = default;
    EvaluatorEntry(const EvaluatorEntry& other);
    EvaluatorEntry& operator = (EvaluatorEntry&& other) = default;
    EvaluatorEntry& operator = (const EvaluatorEntry& other);
    std::unique_ptr<EvaluationState> State;
    CellRangeEvaluator Function;
  };

  /// Container for functions that evaluate data on a single vtkDGCell::Source instance.
  using EvaluatorMap = std::map<RangeKey, EvaluatorEntry>;

  /// Construct an operation object.
  ///
  /// The copy-constructor variant is what allows you to use
  /// vtkSMPThreadLocal<vtkDGOperation> in vtkSMPTools workers.
  vtkDGOperation();
  vtkDGOperation(const vtkDGOperation& other);
  vtkDGOperation(vtkDGCell* cellType, vtkCellAttribute* cellAttribute, vtkStringToken operationName);

  virtual void PrintSelf(std::ostream& os, vtkIndent indent);

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

  /// Return the number of values generated per tuple each time an input cell-id and parameter-value are evaluated.
  int GetNumberOfResultComponents() const { return this->NumberOfResultComponents; }

protected:
  void AddSource(
    vtkCellGrid* grid,
    vtkDGCell* cellType, std::size_t sideSpecIdx,
    vtkCellAttribute* cellAtt, const vtkCellAttribute::CellTypeInfo& cellTypeInfo,
    vtkDGOperatorEntry& op, bool includeShape);

  int NumberOfResultComponents{ 0 };
  EvaluatorMap Evaluators;
};

VTK_ABI_NAMESPACE_END
#endif // vtkDGOperation_h
