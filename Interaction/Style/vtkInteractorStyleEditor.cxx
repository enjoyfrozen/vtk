/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkInteractorStyleEditor.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.


  vtkInteractorStyleEditor is an interactor style resembling the behaviour of
  many 3d editors. For example Blender.

  The three mouse buttons are used as follows:

  Left :   selecting objects
  Middle : navigation
  Right :  reserved for context menu

  although the user can override this if required.

  The navigation assumes a clear vertical axis (0,0,1) that remains vertical. For
  many users this feels natural.

  This style borrows/collects many elements from exising functionality:
  - box select and area picker
  - pan
  - ....

  The only truely new routine is the camera rotation with is implemented in Rotate and
  is invoked via VTKIS_ROTATE

=========================================================================*/
#include "vtkInteractorStyleEditor.h"

#include "vtkCamera.h"
#include "vtkObjectFactory.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"


VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkInteractorStyleEditor);


//------------------------------------------------------------------------------
void vtkInteractorStyleEditor::Rotate()
{
    if (this->CurrentRenderer == nullptr )
    {
        return;
    }

   

    vtkRenderWindowInteractor* rwi = this->Interactor;

    const int dx{ rwi->GetEventPosition()[0] - rwi->GetLastEventPosition()[0] };
    const int dy{ rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1] };

    const int width{ this->CurrentRenderer->GetSize()[0] };
    const int height{ this->CurrentRenderer->GetSize()[1] };

    const double delta_elevation{ -20. / height };
    const double delta_azimuth{ -20. / width };

    const double MOUSE_MOTION_FACTOR{ 20 };

    RotateTurntableBy(
        dx * delta_azimuth * MOUSE_MOTION_FACTOR, dy * delta_elevation * MOUSE_MOTION_FACTOR);
}


void vtkInteractorStyleEditor::RotateTurntableBy(float rxf, float ryf)
{
  /*
  * rfx is rotation about the global Z vector (turn-table mode)
  * rfy is rotation about the side vector
  *
  * CurrentRenderer = self.GetCurrentRenderer()
        rwi = self.GetInteractor()
  */
 
  vtkCamera* camera = this->CurrentRenderer->GetActiveCamera();

  double* campos{ camera->GetPosition() };
  double* focal{ camera->GetFocalPoint() };

  double* up{ camera->GetViewUp() };
  double upside_down_factor{ up[2] < 0 ? -1. : 1. };

  // P = campos - focal  # camera position

  const double Px{ campos[0] - focal[0] };
  const double Py{ campos[1] - focal[1] };
  const double Pz{ campos[2] - focal[2] };

  const double H{ sqrt(pow(Px, 2) + pow(Py, 2)) };
  const double elev{ atan2(Pz, H) };

  //      # Rotate left/right about the global Z axis
  //      H = np.linalg.norm(P[:2])     # horizontal distance of camera to focal point
  //      elev = np.arctan2(P[2], H)    # elevation

  //      # if the camera is near the poles, then derive the azimuth from the up-vector

  const double sin_elev{ sin(elev) };
  double azi{ 0 };

  if (abs(sin_elev) < 0.8)
  {
    azi = atan2(Py, Px);
  }
  else
  {
    if (sin_elev < -0.8)
    {
      azi = atan2(upside_down_factor * up[1], upside_down_factor * up[0]);
    }
    else
    {
      azi = atan2(-upside_down_factor * up[1], -upside_down_factor * up[0]);
    }
  }

  // distance from focal point to camera

  const double D{ sqrt(pow(Px, 2) + pow(Py, 2) + pow(Pz, 2)) };

  //    # apply the change in azimuth and elevation
  const double azi_new {  azi + rxf / 60.};

  const double elev_new{ elev + upside_down_factor * ryf / 60. };


        //# the changed elevation changes H (D stays the same)
  const double H_new{ D * cos(elev_new) };


        //# calculate new camera position relative to focal point
    //Pnew = np.array((Hnew * np.cos(azi_new), Hnew * np.sin(azi_new), D * np.sin(elev_new)))

    const double Px_new{ H_new * cos(azi_new) };
    const double Py_new{ H_new * sin(azi_new) };
    const double Pz_new{ D * sin(elev_new) };


    // # calculate the up-direction of the camera
    const double up_z{ upside_down_factor * cos(elev_new) };  // z follows directly from elevation
    const double up_h{ upside_down_factor * sin(elev_new) };  // horizontal component
        


    camera->SetViewUp(-up_h * cos(azi_new), -up_h * sin(azi_new), up_z);
    camera->SetPosition(focal[0] + Px_new, 
        focal[1] + Py_new, 
        focal[2] + Pz_new);
  

    camera->OrthogonalizeViewUp();


  vtkRenderWindowInteractor* rwi = this->Interactor;


    if (this->AutoAdjustCameraClippingRange)
    {
      this->CurrentRenderer->ResetCameraClippingRange();
    }

    rwi->Render();
  }


VTK_ABI_NAMESPACE_END
