/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#include "vtkSimpleImageFilterExample.h"

#include "vtkImageData.h"
#include "vtkObjectFactory.h"

vtkStandardNewMacro(vtkSimpleImageFilterExample);

// The switch statement in Execute will call this method with
// the appropriate input type (IT). Note that this example assumes
// that the output data type is the same as the input data type.
// This is not always the case.
template <class IT>
void vtkSimpleImageFilterExampleExecute(vtkImageData* input,
                                        vtkImageData* output,
                                        IT* inPtr, IT* outPtr)
{
  int dims[3];
  input->GetDimensions(dims);
  if (input->GetScalarType() != output->GetScalarType())
  {
    vtkGenericWarningMacro(<< "Execute: input ScalarType, " << input->GetScalarType()
    << ", must match out ScalarType " << output->GetScalarType());
    return;
  }

  int size = dims[0]*dims[1]*dims[2];

  for(int i=0; i<size; i++)
  {
    outPtr[i] = inPtr[i];
  }
}

void vtkSimpleImageFilterExample::SimpleExecute(vtkImageData* input,
                                                vtkImageData* output)
{

  void* inPtr = input->GetScalarPointer();
  void* outPtr = output->GetScalarPointer();

  switch(output->GetScalarType())
  {
    // This is simply a #define for a big case list. It handles all
    // data types VTK supports.
    vtkTemplateMacro(
      vtkSimpleImageFilterExampleExecute(input, output,
                                         static_cast<VTK_TT *>(inPtr),
                                         static_cast<VTK_TT *>(outPtr)));
    default:
      vtkGenericWarningMacro("Execute: Unknown input ScalarType");
      return;
  }
}
