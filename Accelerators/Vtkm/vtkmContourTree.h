#ifndef vtkmContourTree_h
#define vtkmContourTree_h

#include "vtkPolyDataAlgorithm.h"
#include "vtkAcceleratorsVTKmModule.h" //required for correct implementation

#include <vector> // For std::vector<> IsoValues

class vtkMultiProcessController;

class VTKACCELERATORSVTKM_EXPORT vtkmContourTree : public vtkPolyDataAlgorithm
{
public:
  vtkTypeMacro(vtkmContourTree, vtkPolyDataAlgorithm)

  void PrintSelf(ostream& os, vtkIndent indent) override;
  static vtkmContourTree* New();

  //@{
  /**
   * By default this filter uses the global controller,
   * but this method can be used to set another instead.
   */
  virtual void SetController(vtkMultiProcessController*);
  vtkGetObjectMacro(Controller, vtkMultiProcessController);
  //@}

  vtkSetMacro(NumberOfIsoValues, int);
  vtkGetMacro(NumberOfIsoValues, int);

  vtkSetMacro(IsoValuesSelectMethod, int);
  vtkGetMacro(IsoValuesSelectMethod, int);

  vtkSetMacro(IsoValuesType, int);
  vtkGetMacro(IsoValuesType, int);

  const std::vector<double>& GetIsoValues() const
  {
    return this->IsoValues;
  }

  //@{
  /**
   * Set/Get the computation of normals. Normal computation is fairly
   * expensive in both time and storage. If the output data will be
   * processed by filters that modify topology or geometry, it may be
   * wise to turn Normals and Gradients off.
   */
  vtkSetMacro(ComputeNormals,bool);
  vtkGetMacro(ComputeNormals,bool);
  vtkBooleanMacro(ComputeNormals,bool);
  //@}

  //@{
  /**
   * Set/Get the computation of gradients. Gradient computation is
   * fairly expensive in both time and storage. Note that if
   * ComputeNormals is on, gradients will have to be calculated, but
   * will not be stored in the output dataset.  If the output data
   * will be processed by filters that modify topology or geometry, it
   * may be wise to turn Normals and Gradients off.
   */
  vtkSetMacro(ComputeGradients,bool);
  vtkGetMacro(ComputeGradients,bool);
  vtkBooleanMacro(ComputeGradients,bool);
  //@}

  //@{
  /**
   * Set/Get the computation of scalars.
   */
  vtkSetMacro(ComputeScalars,bool);
  vtkGetMacro(ComputeScalars,bool);
  vtkBooleanMacro(ComputeScalars,bool);
  //@}

protected:
  vtkmContourTree();
  ~vtkmContourTree();

  int FillInputPortInformation(int, vtkInformation *) override;
  int RequestData(vtkInformation *, vtkInformationVector **,
                  vtkInformationVector *) override;

  vtkMultiProcessController *Controller;

  int NumberOfIsoValues;
  int IsoValuesSelectMethod;
  int IsoValuesType;
  std::vector<double> IsoValues;

  bool ComputeNormals;
  bool ComputeGradients;
  bool ComputeScalars;

private:
  vtkmContourTree(const vtkmContourTree&) = delete;
  void operator=(const vtkmContourTree&) = delete;
};

#endif // vtkmContourTree_h
// VTK-HeaderTest-Exclude: vtkmContourTree.h
