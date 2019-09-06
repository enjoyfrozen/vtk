/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

// Forward declarations
class vtkIOSRenderWindow;
class vtkRenderer;
class vtkIOSRenderWindowInteractor;
class vtkPlaneWidget;
class vtkPolyDataMapper;
class vtkProbeFilter;
class vtkTPWCallback;

@interface VTKViewController : GLKViewController
{
@private
  vtkIOSRenderWindow *RenderWindow;
  vtkPlaneWidget *PlaneWidget;
  vtkRenderer *Renderer;
  vtkProbeFilter *Probe;
  vtkPolyDataMapper *OutlineMapper;
  vtkPolyDataMapper *ProbeMapper;
  vtkTPWCallback *PlaneCallback;
}

@property (nonatomic, strong) UIWindow *window;

- (void)setProbeEnabled:(bool)val;
- (bool)getProbeEnabled;

- (void)setNewDataFile:(NSURL *)url;

- (vtkIOSRenderWindowInteractor *)getInteractor;

@end
