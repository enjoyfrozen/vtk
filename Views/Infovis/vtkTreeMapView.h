/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkTreeMapView
 * @brief   Displays a tree as a tree map.
 *
 *
 * vtkTreeMapView shows a vtkTree in a tree map, where each vertex in the
 * tree is represented by a box.  Child boxes are contained within the
 * parent box, and may be colored and sized by various parameters.
*/

#ifndef vtkTreeMapView_h
#define vtkTreeMapView_h

#include "vtkViewsInfovisModule.h" // For export macro
#include "vtkTreeAreaView.h"

class vtkBoxLayoutStrategy;
class vtkSliceAndDiceLayoutStrategy;
class vtkSquarifyLayoutStrategy;

class VTKVIEWSINFOVIS_EXPORT vtkTreeMapView : public vtkTreeAreaView
{
public:
  static vtkTreeMapView *New();
  vtkTypeMacro(vtkTreeMapView, vtkTreeAreaView);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Sets the treemap layout strategy
   */
  void SetLayoutStrategy(vtkAreaLayoutStrategy* s) override;
  virtual void SetLayoutStrategy(const char* name);
  virtual void SetLayoutStrategyToBox();
  virtual void SetLayoutStrategyToSliceAndDice();
  virtual void SetLayoutStrategyToSquarify();
  //@}

  //@{
  /**
   * The sizes of the fonts used for labeling.
   */
  virtual void SetFontSizeRange(
    const int maxSize, const int minSize, const int delta=4);
  virtual void GetFontSizeRange(int range[3]);
  //@}

protected:
  vtkTreeMapView();
  ~vtkTreeMapView() override;

  vtkSmartPointer<vtkBoxLayoutStrategy> BoxLayout;
  vtkSmartPointer<vtkSliceAndDiceLayoutStrategy> SliceAndDiceLayout;
  vtkSmartPointer<vtkSquarifyLayoutStrategy> SquarifyLayout;

private:
  vtkTreeMapView(const vtkTreeMapView&) = delete;
  void operator=(const vtkTreeMapView&) = delete;
};

#endif
