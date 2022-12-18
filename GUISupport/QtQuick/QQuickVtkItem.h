#ifndef QQuickVtkItem_h
#define QQuickVtkItem_h

#include <QtQuick/QQuickItem>

#include <QtCore/QScopedPointer>

#include <functional>

#include "vtkGUISupportQtQuickModule.h" // for export macro
#include "vtkSmartPointer.h"

VTK_ABI_NAMESPACE_BEGIN

class vtkRenderWindow;
class vtkObject;

class QQuickVtkItemPrivate;
class VTKGUISUPPORTQTQUICK_EXPORT QQuickVtkItem : public QQuickItem
{
  Q_OBJECT

public:
  explicit QQuickVtkItem(QQuickItem* parent = nullptr);
  ~QQuickVtkItem() override;

  using vtkUserData = vtkSmartPointer<vtkObject>;

  /**
   * Set up the graphics surface format and api.
   *
   * This method sets the graphics API to OpenGLRhi and sets up the surface format for intermixed
   * VTK and QtQuick rendering. Use this method before instantiating a QApplication/QGuiApplication
   * typically this will be in your main.cpp file.
   */
  static void setGraphicsApi();

  ///@{
  /**
   * This is where the VTK initializiation should be done including creating a pipeline and
   * attaching it to the window
   *
   * \note All VTK objects are owned by and run on the QML render thread!!  This means you CAN NOT
   * touch any VTK state from any place other than in this method or in your dispatch_async()
   * functions!!
   *
   * \note All VTK objects must be stored in the vtkUserData object returned from this method.
   *       They will be destroyed if the underlaying QSGNode (which must contain all VTK objects) is
   * destroyed.
   *
   * \note At any moment the QML SceneGraph can decide to delete the underlying QSGNode.
   *       If this happens this method will be called again to (re)create all VTK objects used by
   * this node
   *
   * \note Because of this you must be prepared to reset all the VTK state associated with any QML
   * property you have attached to this node during this execution of this method.
   *
   * \note At the time of this method execution, the GUI thread is blocked. Hence, it is safe to
   *       perform state synchronization between the GUI elements and the VTK classes here.
   *
   * \param renderWindow, the VTK render window that creates this object's pixels for display
   *
   * \return The vtkUserData object associated with the VTK render window
   */
  virtual vtkUserData initializeVTK(vtkRenderWindow* renderWindow)
  {
    Q_UNUSED(renderWindow) return {};
  }
  ///@}

  ///@{
  /**
   * At any moment the QML SceneGraph can decide to delete the underlying QSGNode.
   * If this happens this method will be called before the VTK objects used by
   * this node are destroyed
   *
   * \note All VTK objects are owned by and run on the QML render thread!!  This means you CAN NOT
   * touch any VTK state from any place other than in this method, the initializeVTK method, or in
   * your dispatch_async() functions!!
   *
   * \note At the time of this method execution, the GUI thread is blocked. Hence, it is safe to
   *       perform state synchronization between the GUI elements and the VTK classes here.
   *
   * \param renderWindow, the VTK render window that creates this object's pixels for display
   *
   * \param vtkUserData, the object associated with the VTK render window
   */
  virtual void destroyingVTK(vtkRenderWindow* renderWindow, vtkUserData userData)
  {
    Q_UNUSED(renderWindow);
    Q_UNUSED(userData);
  }

  ///@{
  /**
   * This is the function that enqueues an async command that will be executed just before VTK
   * renders
   *
   * \note All VTK objects are owned by and run on the QML render thread!!  This means you CAN NOT
   * touch any VTK state from any place other than in your function object passed as a parameter
   * here or initializeVTK()!!
   *
   * \note This function should only be called from the qt-gui-thread, eg. from a QML button
   * click-handler
   *
   * \note At the time of the async command execution, the GUI thread is blocked. Hence, it is safe
   * to perform state synchronization between the GUI elements and the VTK classes in the async
   * command function.
   *
   * \param renderWindow, the VTK render window that creates this object's pixels for display
   * \param userData An optional User Data object associated with the VTK render window
   */
  void dispatch_async(std::function<void(vtkRenderWindow* renderWindow, vtkUserData userData)>);
  ///@}

protected:
  void scheduleRender();

protected:
  bool event(QEvent*) override;

protected:
  QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
  bool isTextureProvider() const override;
  QSGTextureProvider* textureProvider() const override;
  void releaseResources() override;

private Q_SLOTS:
  void invalidateSceneGraph();

private:
  Q_DISABLE_COPY(QQuickVtkItem)
  Q_DECLARE_PRIVATE(QQuickVtkItem)
  QScopedPointer<QQuickVtkItemPrivate> d_ptr;
};

VTK_ABI_NAMESPACE_END
#endif // QQuickVtkItem_h
