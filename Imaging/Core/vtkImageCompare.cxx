#include "vtkImageCompare.h"
#include "vtkDataObject.h"
#include "vtkImageData.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkSMPTools.h"
#include "vtkStreamingDemandDrivenPipeline.h"

vtkStandardNewMacro(vtkImageCompare);

class vtkImageCompare::ComputeDifferenceImage
{
public:
  unsigned char* in1Pixels;
  unsigned char* in2Pixels;
  unsigned char* outPixels;
  int* extent;
  vtkIdType *in1Incs, *in2Incs, *outIncs;
  double fuzzyThreshold;

  ComputeDifferenceImage(unsigned char* in1Pixels, unsigned char* in2Pixels,
    unsigned char* outPixels, int* extent, vtkIdType* in1Incs,
    vtkIdType* in2Incs, vtkIdType* outIncs, double fuzzyThreshold)
    : in1Pixels(in1Pixels)
    , in2Pixels(in2Pixels)
    , outPixels(outPixels)
    , extent(extent)
    , in1Incs(in1Incs)
    , in2Incs(in2Incs)
    , outIncs(outIncs)
    , fuzzyThreshold(fuzzyThreshold)
  {
  }
  void operator()(vtkIdType row, vtkIdType endRow)
  {
    for (; row < endRow; row++)
    {
      for (int col = extent[0]; col <= extent[1]; col++)
      {
        unsigned char* in1Pixel =
          in1Pixels + row * in1Incs[1] + col * in1Incs[0];
        unsigned char* in2Pixel =
          in2Pixels + row * in2Incs[1] + col * in2Incs[0];
        bool different = false;
        for (int channel = 0; channel < 3; channel++)
        {
          double in1Color = static_cast<int>(in1Pixel[channel]) / 255.;
          double in2Color = static_cast<int>(in2Pixel[channel]) / 255.;
          double distance = in1Color - in2Color;
          if (distance * distance > fuzzyThreshold)
          {
            different = true;
            break;
          }
        }
        unsigned char* outPixel =
          outPixels + row * outIncs[1] + col * outIncs[0];
        if (different)
        {
          outPixel[0] = static_cast<unsigned char>(0xf1);
          outPixel[1] = static_cast<unsigned char>(0x00);
          outPixel[2] = static_cast<unsigned char>(0x1e);
        }
        else
        {
          outPixel[0] = static_cast<unsigned char>(0x88);
          outPixel[1] = static_cast<unsigned char>(0x88);
          outPixel[2] = static_cast<unsigned char>(0x88);
        }
      }
    }
  }
};

class vtkImageCompare::ComputeErrorFunctor
{
public:
  unsigned char* in1Pixels;
  unsigned char* in2Pixels;
  int* extent;
  vtkIdType *in1Incs, *in2Incs;
  double* error;
  vtkSMPThreadLocal<double> localError;

  ComputeErrorFunctor(unsigned char* in1Pixels, unsigned char* in2Pixels,
    int* extent, vtkIdType* in1Incs, vtkIdType* in2Incs, double* error)
    : in1Pixels(in1Pixels)
    , in2Pixels(in2Pixels)
    , extent(extent)
    , in1Incs(in1Incs)
    , in2Incs(in2Incs)
    , error(error)
  {
  }

  void Initialize()
  {
    double& e = localError.Local();
    e = 0.;
  }

  virtual void Reduce() = 0;

  virtual void operator()(vtkIdType row, vtkIdType endRow) = 0;
};

class vtkImageCompare::ComputeMeanSquaredError
  : public vtkImageCompare::ComputeErrorFunctor
{
public:
  ComputeMeanSquaredError(unsigned char* in1Pixels, unsigned char* in2Pixels,
    int* extent, vtkIdType* in1Incs, vtkIdType* in2Incs, double* error)
    : vtkImageCompare::ComputeErrorFunctor(
        in1Pixels, in2Pixels, extent, in1Incs, in2Incs, error)
  {
  }

  void Reduce() override
  {
    for (auto it = localError.begin(); it != localError.end(); it++)
    {
      *error += *it;
    }
    *error /= static_cast<double>((extent[1] + 1) * (extent[3] + 1));
  }

  void operator()(vtkIdType row, vtkIdType endRow) override
  {
    double& e = localError.Local();
    for (; row < endRow; row++)
    {
      double rowError = 0.;
      for (int col = extent[0]; col <= extent[1]; col++)
      {
        unsigned char* in1Pixel =
          in1Pixels + row * in1Incs[1] + col * in1Incs[0];
        unsigned char* in2Pixel =
          in2Pixels + row * in2Incs[1] + col * in2Incs[0];
        for (int channel = 0; channel < 3; channel++)
        {
          double in1Color = static_cast<double>(in1Pixel[channel]);
          double in2Color = static_cast<double>(in2Pixel[channel]);
          double distance = in1Color - in2Color;
          rowError += distance * distance;
        }
      }
      e += rowError;
    }
  }
};

class vtkImageCompare::ComputePeakSignalToNoiseRatioError
  : public vtkImageCompare::ComputeErrorFunctor
{
public:
  ComputePeakSignalToNoiseRatioError(unsigned char* in1Pixels,
    unsigned char* in2Pixels, int* extent, vtkIdType* in1Incs,
    vtkIdType* in2Incs, double* error)
    : vtkImageCompare::ComputeErrorFunctor(
        in1Pixels, in2Pixels, extent, in1Incs, in2Incs, error)
  {
  }

  void Reduce() override
  {
    for (auto it = localError.begin(); it != localError.end(); it++)
    {
      *error += *it;
    }
    *error /= static_cast<double>((extent[1] + 1) * (extent[3] + 1));
    *error = -10. * std::log10(*error / (255. * 255.));
  }

  void operator()(vtkIdType row, vtkIdType endRow) override
  {
    double& e = localError.Local();
    for (; row < endRow; row++)
    {
      double rowError = 0.;
      for (int col = extent[0]; col <= extent[1]; col++)
      {
        unsigned char* in1Pixel =
          in1Pixels + row * in1Incs[1] + col * in1Incs[0];
        unsigned char* in2Pixel =
          in2Pixels + row * in2Incs[1] + col * in2Incs[0];
        for (int channel = 0; channel < 3; channel++)
        {
          double in1Color = static_cast<double>(in1Pixel[channel]);
          double in2Color = static_cast<double>(in2Pixel[channel]);
          double distance = in1Color - in2Color;
          rowError += distance * distance;
        }
      }
      e += rowError;
    }
  }
};

vtkImageCompare::vtkImageCompare()
{
  ErrorThreshold = 0.;
  FuzzyThreshold = 0.;
  ErrorMetric = MeanSquaredErrorMetric;
  Error = 0.;
  isEqual = false;
  this->SetNumberOfInputPorts(2);
}

void vtkImageCompare::ComputeError(unsigned char* in1Pixels,
  unsigned char* in2Pixels, int* extent, vtkIdType* in1Incs, vtkIdType* in2Incs)
{
  Error = 0.;
  vtkImageCompare::ComputeErrorFunctor* errorFunctor = nullptr;
  switch (this->ErrorMetric)
  {
    case MeanSquaredErrorMetric:
      errorFunctor = new vtkImageCompare::ComputeMeanSquaredError(
        in1Pixels, in2Pixels, extent, in1Incs, in2Incs, &Error);
      break;
    case PeakAbsoluteErrorMetric:
    default:
      errorFunctor = new vtkImageCompare::ComputePeakSignalToNoiseRatioError(
        in1Pixels, in2Pixels, extent, in1Incs, in2Incs, &Error);
  }
  if (errorFunctor)
  {
    vtkSMPTools::For(extent[2], extent[3] + 1, *errorFunctor);
    delete errorFunctor;
  }
}

int vtkImageCompare::RequestInformation(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the info objects
  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  vtkInformation* inInfo1 = inputVector[0]->GetInformationObject(0);
  vtkInformation* inInfo2 = inputVector[1]->GetInformationObject(0);

  int* in1Ext = inInfo1->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());
  int* in2Ext = inInfo2->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT());

  int i;
  if (in1Ext[0] != in2Ext[0] || in1Ext[1] != in2Ext[1] ||
    in1Ext[2] != in2Ext[2] || in1Ext[3] != in2Ext[3] ||
    in1Ext[4] != in2Ext[4] || in1Ext[5] != in2Ext[5])
  {
    this->Error = 1000.0;

    vtkErrorMacro("ExecuteInformation: Input are not the same size.\n"
      << " Input1 is: " << in1Ext[0] << "," << in1Ext[1] << "," << in1Ext[2]
      << "," << in1Ext[3] << "," << in1Ext[4] << "," << in1Ext[5] << "\n"
      << " Input2 is: " << in2Ext[0] << "," << in2Ext[1] << "," << in2Ext[2]
      << "," << in2Ext[3] << "," << in2Ext[4] << "," << in2Ext[5]);
  }

  // We still need to set the whole extent to be the intersection.
  // Otherwise the execute may crash.
  int ext[6];
  for (i = 0; i < 3; ++i)
  {
    ext[i * 2] = in1Ext[i * 2];
    if (ext[i * 2] < in2Ext[i * 2])
    {
      ext[i * 2] = in2Ext[i * 2];
    }
    ext[i * 2 + 1] = in1Ext[i * 2 + 1];
    if (ext[i * 2 + 1] > in2Ext[i * 2 + 1])
    {
      ext[i * 2 + 1] = in2Ext[i * 2 + 1];
    }
  }
  outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), ext, 6);

  return 1;
}

int vtkImageCompare::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkImageData* inData[2];
  vtkImageData* outData;

  // prepare input and output data
  vtkInformation* info = outputVector->GetInformationObject(0);
  outData = vtkImageData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
  outData->SetExtent(
    info->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT()));
  outData->AllocateScalars(info);
  for (int i = 0; i < 2; i++)
  {
    info = inputVector[i]->GetInformationObject(0);
    inData[i] =
      vtkImageData::SafeDownCast(info->Get(vtkDataObject::DATA_OBJECT()));
  }
  if (inData[0]->GetNumberOfScalarComponents() < 3 ||
    inData[1]->GetNumberOfScalarComponents() < 3 ||
    outData->GetNumberOfScalarComponents() < 3)
  {
    vtkErrorMacro("Expecting at least 3 components (RGB or RGBA)");
    return 0;
  }
  // this filter expects that input is the same type as output.
  if (inData[0]->GetScalarType() != VTK_UNSIGNED_CHAR ||
    inData[1]->GetScalarType() != VTK_UNSIGNED_CHAR ||
    outData->GetScalarType() != VTK_UNSIGNED_CHAR)
  {
    vtkErrorMacro("All ScalarTypes must be unsigned char");
    return 0;
  }

  int extent[6];
  outData->GetExtent(extent);
  for (int i = 0; i < 6; i++)
  {
    std::cerr << extent[i] << ' ';
  }
  std::cerr << std::endl;

  auto in1ptr =
    static_cast<unsigned char*>(inData[0]->GetScalarPointerForExtent(extent));
  auto in2ptr =
    static_cast<unsigned char*>(inData[1]->GetScalarPointerForExtent(extent));
  auto outptr =
    static_cast<unsigned char*>(outData->GetScalarPointerForExtent(extent));

  vtkIdType in1Incs[3];
  inData[0]->GetIncrements(in1Incs);

  vtkIdType in2Incs[3];
  inData[1]->GetIncrements(in2Incs);

  vtkIdType outIncs[3];
  outData->GetIncrements(outIncs);

  ComputeError(in1ptr, in2ptr, extent, in1Incs, in2Incs);

  ComputeDifferenceImage diff(in1ptr, in2ptr, outptr, extent, in1Incs, in2Incs,
    outIncs, this->FuzzyThreshold);

  vtkSMPTools::For(extent[2], extent[3] + 1, diff);

  isEqual = Error > ErrorThreshold;

  return 1;
}

vtkImageData* vtkImageCompare::GetImage()
{
  if (this->GetNumberOfInputConnections(1) < 1)
  {
    return nullptr;
  }
  return vtkImageData::SafeDownCast(this->GetExecutive()->GetInputData(1, 0));
}
