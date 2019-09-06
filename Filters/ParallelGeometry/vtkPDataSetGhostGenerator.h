/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkPDataSetGhostGenerator
 *
 *
 *  An abstract class that provides common functionality and implements an
 *  interface for all parallel ghost data generators.
 *
 * @sa
 * vtkDataSetGhostGenerator, vtkPUniformGridGhostDataGenerator,
 * vtkPStructuredGridGhostDataGenerator, vtkPRectilinearGridGhostDataGenerator
*/

#ifndef vtkPDataSetGhostGenerator_h
#define vtkPDataSetGhostGenerator_h

#include "vtkFiltersParallelGeometryModule.h" // For export macro
#include "vtkDataSetGhostGenerator.h"

class vtkMultiProcessController;
class vtkMultiBlockDataSet;

class VTKFILTERSPARALLELGEOMETRY_EXPORT vtkPDataSetGhostGenerator :
  public vtkDataSetGhostGenerator
{
public:
   vtkTypeMacro(vtkPDataSetGhostGenerator,vtkDataSetGhostGenerator);
   void PrintSelf(ostream& os, vtkIndent indent) override;

   //@{
   /**
    * Get/Set macro for the multi-process controller. If a controller is not
    * supplied, then, the global controller is assumed.
    */
   vtkSetMacro(Controller, vtkMultiProcessController*);
   vtkGetMacro(Controller, vtkMultiProcessController*);
   //@}

   /**
    * Initializes
    */
   void Initialize();

   /**
    * Barrier synchronization
    */
   void Barrier();

protected:
  vtkPDataSetGhostGenerator();
  ~vtkPDataSetGhostGenerator() override;

  /**
   * Creates ghost layers. Implemented by concrete implementations.
   */
  virtual void GenerateGhostLayers(
      vtkMultiBlockDataSet *in, vtkMultiBlockDataSet *out) override = 0;

  int Rank;
  bool Initialized;
  vtkMultiProcessController *Controller;

private:
  vtkPDataSetGhostGenerator(const vtkPDataSetGhostGenerator&) = delete;
  void operator=(const vtkPDataSetGhostGenerator&) = delete;
};

#endif /* vtkPDataSetGhostGenerator_h */
