/*=========================================================================

Program:   Visualization Toolkit
Module:    vtkCocoaWebGPURenderWindow.mm

Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
All rights reserved.
See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#import "vtkCocoaMacOSXSDKCompatibility.h" // Needed to support old SDKs

#import "vtkCocoaWebGPURenderWindow.h"
#import "vtkCocoaWebGPUView.h"
#import "vtkCommand.h"
#import "vtkIdList.h"
#import "vtkObjectFactory.h"
#import "vtkRenderWindowInteractor.h"
#import "vtkRendererCollection.h"

VTK_ABI_NAMESPACE_BEGIN
vtkStandardNewMacro(vtkCocoaWebGPURenderWindow);
VTK_ABI_NAMESPACE_END

//----------------------------------------------------------------------------
// This is a private class and an implementation detail, do not use it.
// For fullscreen, an NSWindow that captures key events even when borderless
@interface vtkCocoaFullScreenWindow : NSWindow
{
}
@end

@implementation vtkCocoaFullScreenWindow
- (BOOL)canBecomeKeyWindow
{
  return YES;
}
@end

//----------------------------------------------------------------------------
// This is a private class and an implementation detail, do not use it.
// It manages the NSView/NSWindow. It observes for the NSView's frame changing
// position or size. It observes for the NSWindow closing.
//----------------------------------------------------------------------------
@interface vtkCocoaServer : NSObject
{
@private
  vtkCocoaWebGPURenderWindow* _renWin;
}

// Designated initializer
- (id)initWithRenderWindow:(vtkCocoaWebGPURenderWindow*)inRenderWindow;

- (void)startObservations;
- (void)stopObservations;

@end

//----------------------------------------------------------------------------
@implementation vtkCocoaServer

//----------------------------------------------------------------------------
- (id)initWithRenderWindow:(vtkCocoaWebGPURenderWindow*)inRenderWindow
{
  self = [super init];
  if (self)
  {
    _renWin = inRenderWindow;
  }
  return self;
}

//----------------------------------------------------------------------------
- (void)startObservations
{
  assert(_renWin);

  vtkTypeBool windowCreated = _renWin->GetWindowCreated();
  NSWindow* win = reinterpret_cast<NSWindow*>(_renWin->GetRootWindow());
  if (windowCreated && win)
  {
    // Receive notifications of this, and only this, window's closing.
    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc addObserver:self
           selector:@selector(windowWillClose:)
               name:NSWindowWillCloseNotification
             object:win];
  }

  NSView* view = reinterpret_cast<NSView*>(_renWin->GetWindowId());
  if (view)
  {
    // Receive notifications of this, and only this, view's frame changing.
    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc addObserver:self
           selector:@selector(viewFrameDidChange:)
               name:NSViewFrameDidChangeNotification
             object:view];
  }
}

//----------------------------------------------------------------------------
- (void)stopObservations
{
  assert(_renWin);

  vtkTypeBool windowCreated = _renWin->GetWindowCreated();
  NSWindow* win = reinterpret_cast<NSWindow*>(_renWin->GetRootWindow());
  if (windowCreated && win)
  {
    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc removeObserver:self name:NSWindowWillCloseNotification object:win];
  }

  NSView* view = reinterpret_cast<NSView*>(_renWin->GetWindowId());
  if (view)
  {
    NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
    [nc removeObserver:self name:NSViewFrameDidChangeNotification object:view];
  }
}

//----------------------------------------------------------------------------
- (void)windowWillClose:(NSNotification*)aNotification
{
  // We should only get here if it was us that created the NSWindow.
  assert(_renWin);
  assert(_renWin->GetWindowCreated());

  // We should only have observed our own NSWindow.
  assert([aNotification object] == _renWin->GetRootWindow());
  (void)aNotification;

  // Stop observing because the window is closing.
  [self stopObservations];

  // The NSWindow is closing, so prevent anyone from accidentally using it.
  _renWin->SetRootWindow(nullptr);

  // Tell interactor to stop the NSApplication's run loop
  vtkRenderWindowInteractor* interactor = _renWin->GetInteractor();
  vtkTypeBool windowCreated = _renWin->GetWindowCreated();
  if (interactor && windowCreated)
  {
    interactor->TerminateApp();
  }
}

//----------------------------------------------------------------------------
- (void)viewFrameDidChange:(NSNotification*)aNotification
{
  // We should only have observed our own NSView.
  assert(_renWin);
  assert([aNotification object] == _renWin->GetWindowId());
  (void)aNotification;

  // Retrieve the Interactor.
  vtkRenderWindowInteractor* interactor = _renWin->GetInteractor();
  if (!interactor || !interactor->GetEnabled())
  {
    return;
  }

  // Get the NSView's new frame size (in points).
  NSView* view = reinterpret_cast<NSView*>(_renWin->GetWindowId());
  assert(view);
  NSRect viewRect = [view frame];

  // Convert from points to pixels.
  NSRect backingViewRect = [view convertRectToBacking:viewRect];

  int newWidth = static_cast<int>(NSWidth(backingViewRect));
  int newHeight = static_cast<int>(NSHeight(backingViewRect));

  // Get the interactor's current cache of the size.
  int size[2];
  interactor->GetSize(size);

  if (newWidth != size[0] || newHeight != size[1])
  {
    // Process the size change, this sends vtkCommand::WindowResizeEvent.
    interactor->UpdateSize(newWidth, newHeight);

    // Send vtkCommand::ConfigureEvent from the Interactor.
    interactor->InvokeEvent(vtkCommand::ConfigureEvent, nullptr);
  }
}

@end

VTK_ABI_NAMESPACE_BEGIN
//----------------------------------------------------------------------------
vtkCocoaWebGPURenderWindow::vtkCocoaWebGPURenderWindow()
{
  // First, create the cocoa objects manager. The dictionary is empty so
  // essentially all objects are initialized to NULL.
  NSMutableDictionary* cocoaManager = [NSMutableDictionary dictionary];

  // SetCocoaManager works like an Obj-C setter, so do like Obj-C and
  // init the ivar to null first.
  this->CocoaManager = nullptr;
  this->SetCocoaManager(reinterpret_cast<void*>(cocoaManager));
  [cocoaManager self]; // prevent premature collection under GC.

  this->WindowCreated = 0;
  this->ViewCreated = 0;
  this->SetWindowName("Visualization Toolkit - Cocoa");
  this->CursorHidden = 0;
  this->OnScreenInitialized = 0;
}

//----------------------------------------------------------------------------
vtkCocoaWebGPURenderWindow::~vtkCocoaWebGPURenderWindow()
{
  // If we created a vtkCocoaWebGPUView, clear its reference back to us.
  if (this->GetViewCreated())
  {
    NSView* webGPUView = (NSView*)this->GetWindowId();
    if ([webGPUView isKindOfClass:[vtkCocoaWebGPUView class]])
    {
      [(vtkCocoaWebGPUView*)webGPUView setVTKRenderWindow:nullptr];
    }
  }

  if (this->CursorHidden)
  {
    this->ShowCursor();
  }
  this->Finalize();

  vtkRenderer* ren;
  vtkCollectionSimpleIterator rit;
  this->Renderers->InitTraversal(rit);
  while ((ren = this->Renderers->GetNextRenderer(rit)))
  {
    ren->SetRenderWindow(nullptr);
  }

  this->SetCocoaServer(nullptr);
  this->SetRootWindow(nullptr);
  this->SetWindowId(nullptr);
  this->SetParentId(nullptr);

  // Release the cocoa object manager.
  this->SetCocoaManager(nullptr);
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::Finalize()
{
  if (this->IsInitialized())
  {
    this->WGPUFinalize();
  }

  if (this->OnScreenInitialized)
  {
    this->OnScreenInitialized = 0;
    this->DestroyWindow();
  }
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::DestroyWindow()
{
  // release resources
  this->ReleaseGraphicsResources(this);

  vtkCocoaServer* server = (vtkCocoaServer*)this->GetCocoaServer();
  [server stopObservations];
  this->SetCocoaServer(nullptr);

  // If we created it, close the NSWindow.
  if (this->WindowCreated)
  {
    NSWindow* window = (NSWindow*)this->GetRootWindow();
    [window close];
  }

  this->SetWindowId(nullptr);
  this->SetParentId(nullptr);
  this->SetRootWindow(nullptr);
  this->WindowCreated = 0;
  this->ViewCreated = 0;
  this->Mapped = 0;
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetWindowName(const char* arg)
{
  vtkWindow::SetWindowName(arg);
  NSWindow* win = (NSWindow*)this->GetRootWindow();
  if (win)
  {
    NSString* winTitleStr = arg ? [NSString stringWithUTF8String:arg] : @"";
    [win setTitle:winTitleStr];
  }
}

//----------------------------------------------------------------------------
bool vtkCocoaWebGPURenderWindow::InitializeFromCurrentContext()
{
  return this->Superclass::InitializeFromCurrentContext();
}

//----------------------------------------------------------------------------
vtkTypeBool vtkCocoaWebGPURenderWindow::GetEventPending()
{
  return 0;
}

//----------------------------------------------------------------------------
const char* vtkCocoaWebGPURenderWindow::ReportCapabilities()
{
  return "";
}

//----------------------------------------------------------------------------
vtkTypeBool vtkCocoaWebGPURenderWindow::IsDirect()
{
  this->UsingHardware = 0;
  if (this->GetDisplayId() && this->GetWindowId())
  {
    this->UsingHardware = true;
  }
  return this->UsingHardware;
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetSize(int width, int height)
{
  static bool resizing = false;

  if ((this->Size[0] != width) || (this->Size[1] != height) || this->GetParentId())
  {
    this->Superclass::SetSize(width, height);

    if (this->Mapped && !this->UseOffScreenBuffers && this->GetParentId() && this->GetWindowId() &&
      this->GetViewCreated())
    {
      // Set the NSView size, not the window size.
      if (!resizing)
      {
        resizing = true;

        // Get the NSView's current frame (in points).
        NSView* theView = (NSView*)this->GetWindowId();
        NSRect viewRect = [theView frame];

        // Convert the given new size from pixels to points.
        NSSize backingNewSize = NSMakeSize((CGFloat)width, (CGFloat)height);
        NSSize newSize = [theView convertSizeFromBacking:backingNewSize];

        // Test that there's actually a change so as not to recurse into viewFrameDidChange:.
        if (!NSEqualSizes(newSize, viewRect.size))
        {
          // Update the view's frame (in points) keeping the bottom-left
          // corner in the same place.
          CGFloat oldHeight = NSHeight(viewRect);
          CGFloat xpos = NSMinX(viewRect);
          CGFloat ypos = NSMinY(viewRect) - (newSize.height - oldHeight);
          NSRect newRect = NSMakeRect(xpos, ypos, newSize.width, newSize.height);
          [theView setFrame:newRect];
          [theView setNeedsDisplay:YES];
        }

        resizing = false;
      }
    }
    else if (this->Mapped && !this->UseOffScreenBuffers && this->GetRootWindow() &&
      this->GetWindowCreated())
    {
      if (!resizing)
      {
        resizing = true;

        NSWindow* window = (NSWindow*)this->GetRootWindow();
        NSView* theView = (NSView*)this->GetWindowId();

        // Convert the given new size from pixels to points.
        NSRect backingNewRect = NSMakeRect(0.0, 0.0, (CGFloat)width, (CGFloat)height);
        NSRect newRect = [window convertRectFromBacking:backingNewRect];

        // Test that there's actually a change so as not to recurse into viewFrameDidChange:.
        if (!NSEqualSizes(newRect.size, [window frame].size))
        {
          // Set the window size and the view size.
          [window setContentSize:newRect.size];
          [theView setFrame:newRect];
          [theView setNeedsDisplay:YES];
        }

        resizing = false;
      }
    }
  }
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetPosition(int x, int y)
{
  static bool resizing = false;

  if ((this->Position[0] != x) || (this->Position[1] != y) || this->GetParentId())
  {
    this->Modified();
    this->Position[0] = x;
    this->Position[1] = y;
    if (this->Mapped && this->GetParentId() && this->GetWindowId() && this->GetViewCreated())
    {
      // Set the NSView position relative to the parent
      if (!resizing)
      {
        resizing = true;

        // Get the NSView's current frame (in points).
        NSView* parent = (NSView*)this->GetParentId();
        NSRect parentRect = [parent frame];

        NSView* theView = (NSView*)this->GetWindowId();
        NSRect viewRect = [theView frame];

        // Convert the given new position from pixels to points.
        NSPoint backingNewPosition = NSMakePoint((CGFloat)x, (CGFloat)y);
        NSPoint newPosition = [theView convertPointFromBacking:backingNewPosition];

        // Update the view's frameOrigin (in points) keeping the bottom-left
        // corner in the same place.
        CGFloat parentHeight = NSHeight(parentRect);
        CGFloat height = NSHeight(viewRect);
        CGFloat xpos = newPosition.x;
        CGFloat ypos = parentHeight - height - newPosition.y;
        NSPoint origin = NSMakePoint(xpos, ypos);

        // Test that there's actually a change so as not to recurse into viewFrameDidChange:.
        if (!NSEqualPoints(viewRect.origin, origin))
        {
          [theView setFrameOrigin:origin];
          [theView setNeedsDisplay:YES];
        }

        resizing = false;
      }
    }
    else if (this->Mapped && this->GetRootWindow() && this->GetWindowCreated())
    {
      if (!resizing)
      {
        resizing = true;

        NSWindow* window = (NSWindow*)this->GetRootWindow();

        // Convert the given new position from pixels to points.
        // We use a dummy NSRect because NSWindow doesn't have convertPointFromBacking: before
        // macOS 10.14.
        NSRect backingNewPosition = NSMakeRect((CGFloat)x, (CGFloat)y, 0.0, 0.0);
        NSRect newPosition = [window convertRectFromBacking:backingNewPosition];

        // Test that there's actually a change so as not to recurse into viewFrameDidChange:.
        if (!NSEqualPoints([window frame].origin, newPosition.origin))
        {
          [window setFrameOrigin:newPosition.origin];
        }

        resizing = false;
      }
    }
  }
}

//----------------------------------------------------------------------------
// End the rendering process and display the image.
void vtkCocoaWebGPURenderWindow::Frame()
{
  if (!this->AbortRender && this->WindowId != 0)
  {
    this->Superclass::Frame();
  }
}

//----------------------------------------------------------------------------
// Specify various window parameters.
void vtkCocoaWebGPURenderWindow::WindowConfigure()
{
  // this is all handled by the desiredVisualInfo method
}

//----------------------------------------------------------------------------
// Initialize the window for rendering.
void vtkCocoaWebGPURenderWindow::CreateAWindow()
{
  static unsigned count = 1;

  // As vtk is both crossplatform and a library, we don't know if it is being
  // used in a 'regular Cocoa application' or as a 'pure vtk application'.
  // By the former I mean a regular Cocoa application that happens to have
  // a vtkCocoaWebGPUView, by the latter I mean an application that only uses
  // vtk APIs (which happen to use Cocoa as an implementation detail).
  // Specifically, we can't know if NSApplicationMain() was ever called
  // (which is usually done in main()), nor whether the NSApplication exists.
  //
  // So here we call +sharedApplication which will create the NSApplication
  // if it does not exist.  If it does exist, this does nothing.
  // This call is intentionally delayed until this CreateAWindow call
  // to prevent Cocoa-window related stuff from happening in scenarios
  // where vtkRenderWindows are created but never shown.
  NSApplication* app = [NSApplication sharedApplication];

  // create an NSWindow only if neither an NSView nor an NSWindow have
  // been specified already.  This is the case for a 'pure vtk application'.
  // If you are using vtk in a 'regular Mac application' you should call
  // SetRootWindow() and SetWindowId() so that a window is not created here.
  if (!this->GetRootWindow() && !this->GetWindowId() && !this->GetParentId())
  {
    // Ordinarily, only .app bundles get proper mouse and keyboard interaction,
    // but here we change the 'activation policy' to behave as if we were a
    // .app bundle (which we may or may not be).
    (void)[app setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSWindow* theWindow = nil;

    // Revert to main screen if specified screen isn't available.
    NSScreen* screen;
    NSArray* allScreens = [NSScreen screens];
    if (this->DisplayIndex >= 0 && (NSUInteger)this->DisplayIndex < [allScreens count])
    {
      screen = [allScreens objectAtIndex:this->DisplayIndex];
    }
    else
    {
      screen = [NSScreen mainScreen];
    }

    // Get the screen's size (in points).  (If there's no screen, the
    // rectangle will become all zeros and not used anyway.)
    NSRect screenRect = [screen frame];

    // Convert from points to pixels.
    NSRect backingScreenRect = [screen convertRectToBacking:screenRect];

    if (this->FullScreen && screen)
    {
      this->Size[0] = static_cast<int>(NSWidth(backingScreenRect));
      this->Size[1] = static_cast<int>(NSHeight(backingScreenRect));

      // Create an NSWindow with the screen's full size (in points, not pixels).
      theWindow = [[vtkCocoaFullScreenWindow alloc] initWithContentRect:screenRect
                                                              styleMask:NSWindowStyleMaskBorderless
                                                                backing:NSBackingStoreBuffered
                                                                  defer:NO];

      // This will hide the menu and the dock
      [theWindow setLevel:NSMainMenuWindowLevel + 1];
      // This will show the menu and the dock
      //[theWindow setLevel:NSFloatingWindowLevel];
    }
    else
    {
      if ((this->Size[0] == 0) && (this->Size[1] == 0))
      {
        this->Size[0] = 300;
        this->Size[1] = 300;
      }
      if ((this->Position[0] == 0) && (this->Position[1] == 0))
      {
        this->Position[0] = 50;
        this->Position[1] = 50;
      }

      NSRect backingContentRect = NSMakeRect((CGFloat)this->Position[0], (CGFloat)this->Position[1],
        (CGFloat)this->Size[0], (CGFloat)this->Size[1]);

      // Convert from pixels to points.
      NSRect contentRect;
      if (screen)
      {
        contentRect = [screen convertRectFromBacking:backingContentRect];
      }
      else
      {
        contentRect = backingContentRect;
      }

      NSWindowStyleMask styleMask = (NSWindowStyleMaskTitled | NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable);
      theWindow = [[NSWindow alloc] initWithContentRect:contentRect
                                              styleMask:styleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO];
    }

    if (!theWindow)
    {
      vtkErrorMacro("Could not create window, serious error!");
      return;
    }

    this->SetRootWindow(theWindow);
    this->WindowCreated = 1;

    // Since we created the NSWindow, give it a title.
    NSString* winName = [NSString stringWithFormat:@"Visualization Toolkit - Cocoa #%u", count++];
    this->SetWindowName([winName UTF8String]);

    // makeKeyAndOrderFront: will show the window
    if (this->ShowWindow)
    {
      [theWindow makeKeyAndOrderFront:nil];
      [theWindow setAcceptsMouseMovedEvents:YES];
    }
  }

  // create an NSView if one has not been specified
  if (!this->GetWindowId())
  {
    // For NSViews that display OpenGL, the OS defaults to drawing magnified,
    // not in high resolution. There is a tradeoff here between better visual
    // quality vs memory use and processing time. VTK decides on the opposite
    // default and enables best resolution by default. It does so partly because
    // the system python sets NSHighResolutionCapable in this file:
    // /System/Library/Frameworks/Python.framework/Versions/2.7/Resources/Python.app/Contents/Info.plist
    // If you want magnified drawing instead, call SetWantsBestResolution(false)
    bool wantsBest = this->GetWantsBestResolution();

    if (this->GetParentId())
    {
      // Get the NSView's current frame (in points).
      NSView* parent = (NSView*)this->GetParentId();
      NSRect parentRect = [parent frame];
      CGFloat parentHeight = NSHeight(parentRect);
      CGFloat parentWidth = NSWidth(parentRect);

      // Convert from pixels to points.
      NSWindow* window = [parent window];
      assert(window);
      NSRect backingViewRect = NSMakeRect((CGFloat)this->Position[0], (CGFloat)this->Position[1],
        (CGFloat)this->Size[0], (CGFloat)this->Size[1]);
      NSRect viewRect = [window convertRectFromBacking:backingViewRect];

      CGFloat width = NSWidth(viewRect);
      CGFloat height = NSHeight(viewRect);
      CGFloat x = NSMinX(viewRect);
      CGFloat y = parentHeight - height - NSMinY(viewRect);

      // A whole bunch of sanity checks: frame must be inside parent
      if (x > parentWidth - 1)
      {
        x = parentWidth - 1;
      }
      if (y > parentHeight - 1)
      {
        y = parentHeight - 1;
      }
      if (x < 0.0)
      {
        x = 0.0;
      }
      if (y < 0.0)
      {
        y = 0.0;
      }
      if (x + width > parentWidth)
      {
        width = parentWidth - x;
      }
      if (y + height > parentWidth)
      {
        height = parentHeight - y;
      }

      // Don't use vtkCocoaWebGPUView, because if we are in Tk (which is what
      // SetParentId() was added for) then the Tk superview handles the events.
      NSRect viewRect = NSMakeRect(x, y, width, height);
      NSView* webGPUView = [[NSView alloc] initWithFrame:viewRect];
      [parent addSubview:webGPUView];
      this->SetWindowId(webGPUView);
      this->ViewCreated = 1;

#if !VTK_OBJC_IS_ARC
      [webGPUView release];
#endif
    }
    else
    {
      NSRect backingViewRect = NSMakeRect(0.0, 0.0, (CGFloat)this->Size[0], (CGFloat)this->Size[1]);

      // Convert from points to pixels.
      NSWindow* window = (NSWindow*)this->GetRootWindow();
      assert(window);
      NSRect viewRect = [window convertRectFromBacking:backingViewRect];

      // Create a vtkCocoaWebGPUView.
      vtkCocoaWebGPUView* webGPUView = [[vtkCocoaWebGPUView alloc] initWithFrame:viewRect];
      [window setContentView:webGPUView];
      // We have to set the frame's view rect again to work around rounding
      // that occurs when setting the window's content view.
      [webGPUView setFrame:viewRect];
      this->SetWindowId(webGPUView);
      this->ViewCreated = 1;
      [webGPUView setVTKRenderWindow:this];

#if !VTK_OBJC_IS_ARC
      [webGPUView release];
#endif
    }
  }

  // wipe out any existing display lists
  vtkRenderer* renderer = nullptr;
  vtkCollectionSimpleIterator rsit;

  for (this->Renderers->InitTraversal(rsit); (renderer = this->Renderers->GetNextRenderer(rsit));)
  {
    renderer->SetRenderWindow(nullptr);
    renderer->SetRenderWindow(this);
  }
  this->Mapped = 1;

  // Now that the NSView and NSWindow exist, the vtkCocoaServer can start its observations.
  vtkCocoaServer* server = [[vtkCocoaServer alloc] initWithRenderWindow:this];
  this->SetCocoaServer(reinterpret_cast<void*>(server));
  [server startObservations];
#if !VTK_OBJC_IS_ARC
  [server release];
#endif
}

//----------------------------------------------------------------------------
// Initialize the rendering window.
void vtkCocoaWebGPURenderWindow::Initialize()
{
  if (!this->OnScreenInitialized)
  {
    this->OnScreenInitialized = 1;
    this->CreateAWindow();
  }

  if (this->WGPUInit())
  {
    if (this->GetViewCreated())
    {
      NSView* webGPUView = (NSView*)this->GetWindowId();
      if ([webGPUView isKindOfClass:[vtkCocoaWebGPUView class]])
      {
        auto surfDesc = [(vtkCocoaWebGPUView*)webGPUView getSurfaceDescriptor];
        this->Surface = vtkWGPUContext::CreateSurface(*cocoaSurfDesc);
      }
    }
  }
}

//----------------------------------------------------------------------------
int* vtkCocoaWebGPURenderWindow::GetSize()
{
  // if we aren't mapped then just call super
  if (this->Mapped && !this->UseOffScreenBuffers)
  {
    // We want to return the size of 'the window'.  But the term 'window'
    // is overloaded. It's really the NSView that vtk draws into, so we
    // return its size. If there's no NSView, it will result in zeros.
    NSView* view = (NSView*)this->GetWindowId();

    // Get the NSView's current frame (in points).
    NSRect viewRect = [view frame];

    // Convert from points to pixels.
    NSRect backingViewRect = [view convertRectToBacking:viewRect];

    // Update the ivar.
    this->Size[0] = static_cast<int>(NSWidth(backingViewRect));
    this->Size[1] = static_cast<int>(NSHeight(backingViewRect));
  }

  return this->Superclass::GetSize();
}

//----------------------------------------------------------------------------
// Get the current size of the screen in pixels.
int* vtkCocoaWebGPURenderWindow::GetScreenSize()
{
  // Get the NSScreen that the NSView is mostly on.  Either could be nil.
  NSView* view = (NSView*)this->GetWindowId();
  NSWindow* window = [view window];
  NSScreen* screen = [window screen];

  // If screen is nil, then fall back to the screen with index DisplayIndex,
  // which CreateAWindow() also uses (it could be nil too).
  if (!screen)
  {
    NSArray* allScreens = [NSScreen screens];
    if (this->DisplayIndex >= 0 && (NSUInteger)this->DisplayIndex < [allScreens count])
    {
      screen = [allScreens objectAtIndex:this->DisplayIndex];
    }
    else
    {
      screen = [NSScreen mainScreen];
    }
  }

  if (!screen)
  {
    // Revert to main screen if specified screen isn't available.
    screen = [NSScreen mainScreen];
  }

  // Get the screen's size (in points).
  NSRect screenRect = [screen frame];

  // Convert from points to pixels.
  NSRect backingScreenRect = [screen convertRectToBacking:screenRect];

  this->ScreenSize[0] = static_cast<int>(NSWidth(backingScreenRect));
  this->ScreenSize[1] = static_cast<int>(NSHeight(backingScreenRect));

  return this->ScreenSize;
}

//----------------------------------------------------------------------------
// Get the position in screen coordinates of the window.
int* vtkCocoaWebGPURenderWindow::GetPosition()
{
  // if we aren't mapped then just return the ivar
  if (!this->Mapped)
  {
    return this->Position;
  }

  NSView* parent = (NSView*)this->GetParentId();
  NSView* view = (NSView*)this->GetWindowId();
  if (parent && view)
  {
    // Get display position of the NSView within its parent (in points).
    NSRect parentRect = [parent frame];
    NSRect viewRect = [view frame];

    // Convert from points to pixels.
    NSRect backingParentRect = [parent convertRectToBacking:parentRect];
    NSRect backingViewRect = [view convertRectToBacking:viewRect];

    this->Position[0] = static_cast<int>(NSMinX(backingViewRect));
    this->Position[1] = static_cast<int>(
      NSHeight(backingParentRect) - NSHeight(backingViewRect) - NSMinY(backingViewRect));
  }
  else
  {
    // We want to return the position of 'the window'.  But the term 'window'
    // is overloaded. In this case, it's the position of the NSWindow itself
    // on the screen that we return. We don't much care where the NSView is
    // within the NSWindow.
    NSWindow* window = (NSWindow*)this->GetRootWindow();
    if (window)
    {
      // Get the NSWindow's current frame (in points).
      NSRect windowRect = [window frame];

      // Convert from points to pixels.
      NSRect backingWindowRect = [window convertRectToBacking:windowRect];

      this->Position[0] = static_cast<int>(NSMinX(backingWindowRect));
      this->Position[1] = static_cast<int>(NSMinY(backingWindowRect));
    }
  }

  return this->Position;
}

//----------------------------------------------------------------------------
// Change the window to fill the entire screen.
void vtkCocoaWebGPURenderWindow::SetFullScreen(vtkTypeBool arg)
{
  if (this->FullScreen == arg)
  {
    return;
  }

  if (!this->Mapped)
  {
    this->FullScreen = arg;
    return;
  }

  // set the mode
  this->FullScreen = arg;
  if (this->FullScreen <= 0)
  {
    this->Position[0] = this->OldScreen[0];
    this->Position[1] = this->OldScreen[1];
    this->Size[0] = this->OldScreen[2];
    this->Size[1] = this->OldScreen[3];
    this->Borders = this->OldScreen[4];
  }
  else
  {
    // if window already up get its values
    if (this->GetRootWindow())
    {
      const int* pos = this->GetPosition();
      this->OldScreen[0] = pos[0];
      this->OldScreen[1] = pos[1];

      this->OldScreen[4] = this->Borders;
      this->PrefFullScreen();
    }
  }

  // remap the window
  this->WindowRemap();

  this->Modified();
}

//----------------------------------------------------------------------------
//
// Set the variable that indicates that we want a stereo capable window
// be created. This method can only be called before a window is realized.
//
void vtkCocoaWebGPURenderWindow::SetStereoCapableWindow(vtkTypeBool capable)
{
  if (this->GetWindowId() == nullptr)
  {
    vtkRenderWindow::SetStereoCapableWindow(capable);
  }
  else
  {
    vtkWarningMacro(<< "Requesting a StereoCapableWindow must be performed "
                    << "before the window is realized, i.e. before a render.");
  }
}

//----------------------------------------------------------------------------
// Set the preferred window size to full screen.
void vtkCocoaWebGPURenderWindow::PrefFullScreen()
{
  const int* size = this->GetScreenSize();
  vtkWarningMacro(<< "Can only set FullScreen before showing window: " << size[0] << 'x' << size[1]
                  << ".");
}

//----------------------------------------------------------------------------
// Remap the window.
void vtkCocoaWebGPURenderWindow::WindowRemap()
{
  vtkWarningMacro(<< "Can't remap the window.");
  // Acquire the display and capture the screen.
  // Create the full-screen window.
  // Add the context.
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "CocoaManager: " << this->GetCocoaManager() << endl;
  os << indent << "RootWindow (NSWindow): " << this->GetRootWindow() << endl;
  os << indent << "WindowId (NSView): " << this->GetWindowId() << endl;
  os << indent << "ParentId (NSView): " << this->GetParentId() << endl;
  os << indent << "PixelFormat: " << this->GetPixelFormat() << endl;
  os << indent << "WindowCreated: " << (this->GetWindowCreated() ? "Yes" : "No") << endl;
  os << indent << "ViewCreated: " << (this->GetViewCreated() ? "Yes" : "No") << endl;
     << endl;
}

//----------------------------------------------------------------------------
// Returns the NSWindow* associated with this vtkRenderWindow.
void* vtkCocoaWebGPURenderWindow::GetRootWindow()
{
  NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
  return reinterpret_cast<void*>([manager objectForKey:@"RootWindow"]);
}

//----------------------------------------------------------------------------
// Sets the NSWindow* associated with this vtkRenderWindow.
void vtkCocoaWebGPURenderWindow::SetRootWindow(void* arg)
{
  if (arg != nullptr)
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager setObject:reinterpret_cast<id>(arg) forKey:@"RootWindow"];
  }
  else
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager removeObjectForKey:@"RootWindow"];
  }
}

//----------------------------------------------------------------------------
// Returns the NSView* associated with this vtkRenderWindow.
void* vtkCocoaWebGPURenderWindow::GetWindowId()
{
  NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
  return reinterpret_cast<void*>([manager objectForKey:@"WindowId"]);
}

//----------------------------------------------------------------------------
// Sets the NSView* associated with this vtkRenderWindow.
void vtkCocoaWebGPURenderWindow::SetWindowId(void* arg)
{
  if (arg != nullptr)
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager setObject:reinterpret_cast<id>(arg) forKey:@"WindowId"];
  }
  else
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager removeObjectForKey:@"WindowId"];
  }
}

//----------------------------------------------------------------------------
// Returns the NSView* that is the parent of this vtkRenderWindow.
void* vtkCocoaWebGPURenderWindow::GetParentId()
{
  NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
  return reinterpret_cast<void*>([manager objectForKey:@"ParentId"]);
}

//----------------------------------------------------------------------------
// Sets the NSView* that this vtkRenderWindow should use as a parent.
void vtkCocoaWebGPURenderWindow::SetParentId(void* arg)
{
  if (arg != nullptr)
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager setObject:reinterpret_cast<id>(arg) forKey:@"ParentId"];
  }
  else
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager removeObjectForKey:@"ParentId"];
  }
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetCocoaServer(void* server)
{
  if (server != nullptr)
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager setObject:reinterpret_cast<vtkCocoaServer*>(server) forKey:@"CocoaServer"];
  }
  else
  {
    NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
    [manager removeObjectForKey:@"CocoaServer"];
  }
}

//----------------------------------------------------------------------------
void* vtkCocoaWebGPURenderWindow::GetCocoaServer()
{
  NSMutableDictionary* manager = reinterpret_cast<NSMutableDictionary*>(this->GetCocoaManager());
  return reinterpret_cast<void*>([manager objectForKey:@"CocoaServer"]);
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetCocoaManager(void* manager)
{
  NSMutableDictionary* currentCocoaManager =
    reinterpret_cast<NSMutableDictionary*>(this->CocoaManager);
  NSMutableDictionary* newCocoaManager = reinterpret_cast<NSMutableDictionary*>(manager);

  if (currentCocoaManager != newCocoaManager)
  {
    // Why not use Cocoa's retain and release?  Without garbage collection
    // (GC), the two are equivalent anyway because of 'toll free bridging',
    // so no problem there.  With GC, retain and release do nothing, but
    // CFRetain and CFRelease still manipulate the internal reference count.
    // We need that, since we are not using strong references (we don't want
    // it collected out from under us!).
    if (currentCocoaManager)
    {
      CFRelease(currentCocoaManager);
    }
    if (newCocoaManager)
    {
      this->CocoaManager = const_cast<void*>(CFRetain(newCocoaManager));
    }
    else
    {
      this->CocoaManager = nullptr;
    }
  }
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetWindowInfo(const char* info)
{
  // The parameter is an ASCII string of a decimal number representing
  // a pointer to the window. Convert it back to a pointer.
  ptrdiff_t tmp = 0;
  if (info)
  {
    (void)sscanf(info, "%tu", &tmp);
  }

  this->SetWindowId(reinterpret_cast<void*>(tmp));
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetParentInfo(const char* info)
{
  // The parameter is an ASCII string of a decimal number representing
  // a pointer to the window. Convert it back to a pointer.
  ptrdiff_t tmp = 0;
  if (info)
  {
    (void)sscanf(info, "%tu", &tmp);
  }

  this->SetParentId(reinterpret_cast<void*>(tmp));
}

//----------------------------------------------------------------------------
void* vtkCocoaWebGPURenderWindow::GetCocoaManager()
{
  return this->CocoaManager;
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::HideCursor()
{
  if (this->CursorHidden)
  {
    return;
  }
  this->CursorHidden = 1;

  [NSCursor hide];
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::ShowCursor()
{
  if (!this->CursorHidden)
  {
    return;
  }
  this->CursorHidden = 0;

  [NSCursor unhide];
}

// ---------------------------------------------------------------------------
vtkTypeBool vtkCocoaWebGPURenderWindow::GetViewCreated()
{
  return this->ViewCreated;
}

// ---------------------------------------------------------------------------
vtkTypeBool vtkCocoaWebGPURenderWindow::GetWindowCreated()
{
  return this->WindowCreated;
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetCursorPosition(int x, int y)
{
  // The given coordinates are from the bottom left of the view.
  NSPoint newViewPoint = NSMakePoint(x, y);

  // Convert to screen coordinates.
  NSView* view = (NSView*)this->GetWindowId();
  if (view)
  {
    NSPoint screenPoint = [view convertPoint:newViewPoint toView:nil];
    CGPoint newCursorPosition = NSPointToCGPoint(screenPoint);

    // Move the cursor there.
    (void)CGWarpMouseCursorPosition(newCursorPosition);
  }
}

//----------------------------------------------------------------------------
void vtkCocoaWebGPURenderWindow::SetCurrentCursor(int shape)
{
  if (this->InvokeEvent(vtkCommand::CursorChangedEvent, &shape))
  {
    return;
  }
  this->Superclass::SetCurrentCursor(shape);

  if (!this->Mapped)
  {
    return;
  }

  NSCursor* cursor = nil;
  switch (shape)
  {
    case VTK_CURSOR_DEFAULT:
    case VTK_CURSOR_CUSTOM:
    case VTK_CURSOR_ARROW:
      cursor = [NSCursor arrowCursor];
      break;
    case VTK_CURSOR_SIZENS:
      cursor = [NSCursor resizeUpDownCursor];
      break;
    case VTK_CURSOR_SIZEWE:
      cursor = [NSCursor resizeLeftRightCursor];
      break;
    case VTK_CURSOR_HAND:
      cursor = [NSCursor pointingHandCursor];
      break;
    case VTK_CURSOR_CROSSHAIR:
      cursor = [NSCursor crosshairCursor];
      break;

    // NSCursor does not have cursors for these.
    case VTK_CURSOR_SIZENE:
    case VTK_CURSOR_SIZESW:
    case VTK_CURSOR_SIZENW:
    case VTK_CURSOR_SIZESE:
    case VTK_CURSOR_SIZEALL:
      cursor = [NSCursor arrowCursor];
      break;
  }

  [cursor set];
}

//----------------------------------------------------------------------------
VTK_ABI_NAMESPACE_END
