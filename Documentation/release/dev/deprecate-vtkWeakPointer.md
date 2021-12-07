## vtkWeakPointer deprecation

The `vtkWeakPointer` class is on its way to deprecation due to its
thread-unsafety. The following APIs have also been deprecated so that they may
be used in a thread-safe manner. The returned pointers cannot be returned with
new references because the callers would not know whether to call
`::UnRegister()` on them or not. Instead, replacement methods which return
`vtkSmartPointer` are available.

* `vtkWeakReference::Get()` is now `vtkWeakReference::GetOwned()`
* `vtkObjectIdMap::GetActiveObject()` is now `vtkObjectIdMap::GetActiveObjectOwned()`
* `vtkMultiProcessController::GetGlobalController()` is now `vtkMultiProcessController::GetGlobalControllerOwned()`
* `vtkMultiProcessController::GetLocalController()` is now `vtkMultiProcessController::GetLocalControllerOwned()`
* `QQuickVTKInteractiveWidget::widget()` is now `QQuickVTKInteractiveWidget::widgetOwned()`
* `vtkChartLegend::GetChart()` is now `vtkChartLegend::GetChartOwned()`
* `vtkScatterPlotMatrix::GetMainChart()` is now `vtkScatterPlotMatrix::GetMainChartOwned()`
