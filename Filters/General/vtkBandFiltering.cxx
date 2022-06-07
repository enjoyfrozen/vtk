/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkBandFiltering.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkBandFiltering.h"

#include "vtkDoubleArray.h"
#include "vtkObjectFactory.h"
#include "vtkSMPTools.h"
#include "vtkTable.h"
#include "vtkTableFFT.h"

#include <string>

//------------------------------------------------------------------------------
vtkStandardNewMacro(vtkBandFiltering);

//----------------------------------------------------------------------------
void vtkBandFiltering::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ApplyFFT: " << this->ApplyFFT << std::endl;
  os << indent << "DefaultSamplingRate: " << this->DefaultSamplingRate << std::endl;
  os << indent << "WindowType: " << this->WindowType << std::endl;
  os << indent << "BandFilteringMode: " << this->BandFilteringMode << std::endl;
  os << indent << "ReferenceValue: " << this->ReferenceValue << std::endl;
  os << indent << "OutputInDecibel: " << this->OutputInDecibel << std::endl;
}

//----------------------------------------------------------------------------
int vtkBandFiltering::GenerateOctaveBands(double fmin, double fmax,
  std::vector<double>& lowerFrequencies, std::vector<double>& centerFrequencies,
  std::vector<double>& upperFrequencies)
{
  double ratioFrequency = std::pow(10, 3.0 / 10.0);
  double baseFrequency = 1000.0;
  double bandWidth = 1.0;
  if (this->BandFilteringMode == vtkBandFiltering::THIRD_OCTAVE)
  {
    bandWidth = 3;
  }

  double lowestBand =
    std::floor(bandWidth * std::log10(fmin / baseFrequency) / std::log10(ratioFrequency) + 0.5) + 1;
  double highestBand =
    std::floor(bandWidth * std::log10(fmax / baseFrequency) / std::log10(ratioFrequency) + 0.5) + 1;
  if (highestBand < lowestBand)
  {
    return 0;
  }

  std::vector<double> bandIndices;
  for (double x = lowestBand; x < highestBand; x++)
  {
    bandIndices.push_back(x);
  }

  for (int i = 0; i < bandIndices.size(); i++)
  {
    lowerFrequencies.push_back(
      baseFrequency * std::pow(ratioFrequency, (bandIndices[i] - 0.5) / bandWidth));
    centerFrequencies.push_back(
      baseFrequency * std::pow(ratioFrequency, (bandIndices[i]) / bandWidth));
    upperFrequencies.push_back(
      baseFrequency * std::pow(ratioFrequency, (bandIndices[i] + 0.5) / bandWidth));
  }

  return 1;
}

//----------------------------------------------------------------------------
int vtkBandFiltering::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  auto input = vtkTable::GetData(inputVector[0]);
  auto output = vtkTable::GetData(outputVector);
  if (!input || !output)
  {
    return 0;
  }
  if (input->GetNumberOfColumns() <= 0)
  {
    return 1;
  }

  bool haveTimeColumn = false;
  for (vtkIdType col = 0; col < input->GetNumberOfColumns(); col++)
  {
    vtkAbstractArray* array = input->GetColumn(col);
    const char* arrayName = array->GetName();
    if (vtksys::SystemTools::Strucmp(arrayName, "time") != 0)
    {
      haveTimeColumn = true;
      output->AddColumn(input->GetColumnByName(arrayName));
    }
    if (vtksys::SystemTools::Strucmp(arrayName, this->ProcessedColumnName.c_str()) != 0)
    {
      output->AddColumn(input->GetColumnByName(arrayName));
    }
  }
  if (output->GetNumberOfColumns() == 0)
  {
    return 0;
  }

  if (this->ApplyFFT)
  {
    vtkNew<vtkTableFFT> tableFFT;
    tableFFT->SetInputData(output);
    tableFFT->CreateFrequencyColumnOn();
    tableFFT->SetWindowingFunction(this->WindowType);
    if (!haveTimeColumn)
    {
      tableFFT->SetDefaultSampleRate(this->DefaultSamplingRate);
    }

    tableFFT->Update();
    output->ShallowCopy(tableFFT->GetOutput());
    if (!output)
    {
      vtkErrorMacro("Failed to apply an FFT with vtkTableFFT filter.");
      return 0;
    }
  }

  vtkDoubleArray* fftValues =
    vtkDoubleArray::SafeDownCast(output->GetColumnByName(this->ProcessedColumnName.c_str()));
  if (!fftValues)
  {
    vtkErrorMacro("Can't find processed column in the FFT output.");
    return 0;
  }
  // vtkTableFFT should to produce complex values
  if (fftValues->GetNumberOfComponents() != 2)
  {
    vtkErrorMacro("Processed column by a FFT isn't an array of complex value.");
    return 0;
  }

  // Compute in amplitudes from the complex array
  vtkNew<vtkDoubleArray> amplitudes;
  amplitudes->SetNumberOfComponents(1);
  amplitudes->SetNumberOfTuples(1 + fftValues->GetNumberOfTuples() / 2);

  for (vtkIdType i = 0; i < amplitudes->GetNumberOfTuples(); i++)
  {
    double* complex = fftValues->GetTuple2(i);
    double magnitude = vtkMath::Norm2D(complex);
    amplitudes->InsertValue(i, magnitude);
  }

  if (this->OutputInDecibel)
  {
    vtkSMPTools::For(0, amplitudes->GetNumberOfTuples(),
      [&](vtkIdType begin, vtkIdType end)
      {
        for (vtkIdType id = begin; id < end; id++)
        {
          double value = amplitudes->GetValue(id);
          amplitudes->SetValue(id, 20.0 * std::log10(value / ReferenceValue));
        }
      });
  }

  // Frequency sample
  // Frequence Range in our case will only be inside [0, fs/2]
  double maxFreqSample = this->DefaultSamplingRate / 2;
  double frequencySpacing = maxFreqSample / amplitudes->GetNumberOfTuples();
  vtkNew<vtkDoubleArray> octaveBand;
  octaveBand->SetNumberOfComponents(1);
  if (this->OutputInDecibel)
  {
    octaveBand->SetName("Decibel (dB)");
  }
  else
  {
    octaveBand->SetName(this->ProcessedColumnName.c_str());
  }

  // Generate each frequency bands
  std::vector<double> lowerFrequencies;
  std::vector<double> centerFrequencies;
  std::vector<double> upperFrequencies;
  double fmin = VTK_DOUBLE_MAX;
  double fmax = VTK_DOUBLE_MIN;
  auto* frequencies = vtkDoubleArray::SafeDownCast(output->GetColumnByName("Frequency"));
  if (frequencies)
  {
    for (vtkIdType i = 0; i < frequencies->GetNumberOfValues() / 2; i++)
    {
      if (frequencies->GetValue(i) == 0)
      {
        continue;
      }

      if (frequencies->GetValue(i) < fmin)
      {
        fmin = frequencies->GetValue(i);
      }
      if (frequencies->GetValue(i) > fmax)
      {
        fmax = frequencies->GetValue(i);
      }
    }
  }
  else
  {
    // Without frequency column generate by vtkTableFFT, we used by default the standard sound
    // pressure value used for these bands
    fmin = 16.0;
    if (this->BandFilteringMode == vtkBandFiltering::OCTAVE)
    {
      fmax = 16000;
    }
    else
    {
      fmax = 20000.0;
    }
  }
  this->GenerateOctaveBands(fmin, fmax, lowerFrequencies, centerFrequencies, upperFrequencies);

  // Skip the second half of the fft because it is mirrored
  vtkIdType maxIdFreq = frequencies->GetNumberOfValues() / 2 + 1;
  vtkIdType currentIdFreq = 0;
  double currentFreq = frequencies->GetValue(currentIdFreq);
  while (currentFreq < lowerFrequencies[0] && currentIdFreq < maxIdFreq)
  {
    currentIdFreq++;
    currentFreq = frequencies->GetValue(currentIdFreq);
  }

  // Each octave band will just be the mean of all amplitude in the Hz range
  int numberOfValueInBand = 0;
  double acc = 0.0;
  vtkSMPTools::For(0, centerFrequencies.size(),
    [&](vtkIdType begin, vtkIdType end)
    {
      for (vtkIdType i = begin; i < end; i++)
      {
        while (currentIdFreq < maxIdFreq && currentFreq >= lowerFrequencies[i] &&
          currentFreq < upperFrequencies[i])
        {
          acc += amplitudes->GetValue(currentIdFreq);
          numberOfValueInBand++;
          currentIdFreq++;
          currentFreq = frequencies->GetValue(currentIdFreq);
        }

        if (numberOfValueInBand == 0)
        {
          octaveBand->InsertNextTuple1(0);
        }
        else
        {
          acc = acc / numberOfValueInBand;
          octaveBand->InsertNextTuple1(acc);
        }

        acc = 0.0;
        numberOfValueInBand = 0;
      }
    });

  vtkNew<vtkDoubleArray> xAxis;
  xAxis->SetNumberOfComponents(1);
  for (vtkIdType i = 0; i < centerFrequencies.size(); i++)
  {
    xAxis->InsertNextTuple1(centerFrequencies[i]);
  }
  xAxis->SetName("Frequency");

  output->RemoveAllColumns();
  output->AddColumn(xAxis);
  output->AddColumn(octaveBand);

  return 1;
}
