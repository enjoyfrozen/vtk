/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
#ifndef vtkOTIncludes_h
#define vtkOTIncludes_h

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
 #pragma GCC diagnostic ignored "-Woverloaded-virtual"
 #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include "openturns/DistributionFactoryImplementation.hxx"
#include "openturns/DistributionImplementation.hxx"
#include "openturns/Epanechnikov.hxx"
#include "openturns/KernelSmoothing.hxx"
#include "openturns/Point.hxx"
#include "openturns/ResourceMap.hxx"
#include "openturns/Sample.hxx"
#include "openturns/Triangular.hxx"

#if defined(__GNUC__) && !defined(__INTEL_COMPILER)
 #pragma GCC diagnostic pop
#endif

#endif
// VTK-HeaderTest-Exclude: vtkOTIncludes.h
