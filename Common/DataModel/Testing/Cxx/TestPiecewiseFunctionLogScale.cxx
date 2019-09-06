/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include "vtkPiecewiseFunction.h"

#include "vtkNew.h"

#include <cmath>

// Note that this may evaluate args twice. Use wisely.
#define TEST_ASSERT_FUZZY_EQUAL(expect, actual) \
  if (std::fabs((expect) - (actual)) >= 1e-5) \
  { \
    std::cerr << "Error at line " << __LINE__ \
              << ": Expected value " << (expect) << ", got " \
              << (actual) << "\n"; \
    return EXIT_FAILURE; \
  }

int TestPiecewiseFunctionLogScale(int, char*[])
{
  vtkNew<vtkPiecewiseFunction> func;
  func->UseLogScaleOn();

  // Add some points that will give easily predictable interpolations.
  func->AddPoint( .01, -2.);
  func->AddPoint(-.01, -2.);
  func->AddPoint( 100., 2.);
  func->AddPoint(-100., 2.);

  // Check that the interpolations are correct in logarithmic space.
  TEST_ASSERT_FUZZY_EQUAL(-1., func->GetValue( .1));
  TEST_ASSERT_FUZZY_EQUAL(-1., func->GetValue(-.1));
  TEST_ASSERT_FUZZY_EQUAL(0., func->GetValue( 1.));
  TEST_ASSERT_FUZZY_EQUAL(0., func->GetValue(-1.));
  TEST_ASSERT_FUZZY_EQUAL(1., func->GetValue( 10.));
  TEST_ASSERT_FUZZY_EQUAL(1., func->GetValue(-10.));

  return EXIT_SUCCESS;
}
