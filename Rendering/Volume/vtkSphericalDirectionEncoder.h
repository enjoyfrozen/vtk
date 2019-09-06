/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/
/**
 * @class   vtkSphericalDirectionEncoder
 * @brief   A direction encoder based on spherical coordinates
 *
 * vtkSphericalDirectionEncoder is a direction encoder which uses spherical
 * coordinates for mapping (nx, ny, nz) into an azimuth, elevation pair.
 *
 * @sa
 * vtkDirectionEncoder
*/

#ifndef vtkSphericalDirectionEncoder_h
#define vtkSphericalDirectionEncoder_h

#include "vtkRenderingVolumeModule.h" // For export macro
#include "vtkDirectionEncoder.h"

class VTKRENDERINGVOLUME_EXPORT vtkSphericalDirectionEncoder : public vtkDirectionEncoder
{
public:
  vtkTypeMacro(vtkSphericalDirectionEncoder,vtkDirectionEncoder);
  void PrintSelf( ostream& os, vtkIndent indent ) override;

  /**
   * Construct the object. Initialize the index table which will be
   * used to map the normal into a patch on the recursively subdivided
   * sphere.
   */
  static vtkSphericalDirectionEncoder *New();


  /**
   * Given a normal vector n, return the encoded direction
   */
  int GetEncodedDirection( float n[3] ) override;

  /**
   * / Given an encoded value, return a pointer to the normal vector
   */
  float *GetDecodedGradient( int value ) VTK_SIZEHINT(3) override;

  /**
   * Return the number of encoded directions
   */
  int GetNumberOfEncodedDirections( void ) override { return 65536; }

  /**
   * Get the decoded gradient table. There are
   * this->GetNumberOfEncodedDirections() entries in the table, each
   * containing a normal (direction) vector. This is a flat structure -
   * 3 times the number of directions floats in an array.
   */
  float *GetDecodedGradientTable( void ) override
  {
      return &(this->DecodedGradientTable[0]);
  }


protected:
  vtkSphericalDirectionEncoder();
  ~vtkSphericalDirectionEncoder() override;

  static float DecodedGradientTable[65536*3];

  //@{
  /**
   * Initialize the table at startup
   */
  static void InitializeDecodedGradientTable();
  static int DecodedGradientTableInitialized;
  //@}

private:
  vtkSphericalDirectionEncoder(const vtkSphericalDirectionEncoder&) = delete;
  void operator=(const vtkSphericalDirectionEncoder&) = delete;
};


#endif


