#ifndef vtkDGOperationStateEntry_h
#define vtkDGOperationStateEntry_h

#include "vtkDGOperationState.h"
#include "vtkType.h" // For vtkTypeUInt64.

#include <functional> // For std::function<>.
#include <memory> // For std::unique_ptr<>.

VTK_ABI_NAMESPACE_BEGIN

class vtkDataArray;
class vtkDoubleArray;

/// Signature for a method to evaluate data on a single vtkDGCell::Source instance.
using vtkDGCellRangeEvaluator =
  std::function<void(vtkDataArray* cellIds, vtkDataArray* rst, vtkDoubleArray* result,
    vtkTypeUInt64 begin, vtkTypeUInt64 end)>;

/**@class vtkDGOperationStateEntry
  * Encapsulate the state required to evaluate DG cell-attributes.
  *
  */
class VTKFILTERSCELLGRID_EXPORT vtkDGOperationStateEntry
{
public:
  vtkDGOperationStateEntry() = default;
  vtkDGOperationStateEntry(const vtkDGOperationStateEntry& other);
  vtkDGOperationStateEntry& operator = (vtkDGOperationStateEntry&& other) = default;
  vtkDGOperationStateEntry& operator = (const vtkDGOperationStateEntry& other);
  std::unique_ptr<vtkDGOperationState> State;
  vtkDGCellRangeEvaluator Function;
};

VTK_ABI_NAMESPACE_END

#endif // vtkDGOperationStateEntry_h
