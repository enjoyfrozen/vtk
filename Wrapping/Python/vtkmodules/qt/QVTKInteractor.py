"""An interactor for QVTKOpenGLNativeWidget.

NOTE: This class does not support 3DConnexion devices.
"""

from __future__ import annotations

import qtpy
from packaging.version import parse
from qtpy import QtCore
from vtkmodules.vtkCommonCore import vtkCommand
from vtkmodules.vtkRenderingUI import vtkGenericRenderWindowInteractor


class _QVTKInteractorInternal(QtCore.QObject):
    """Internal helper class for ``QVTKInteractor``.

    In the ``VTK`` ``C++`` source, this is private internal helper class following the
    ``PIMPL`` (Pointer to an IMPLementation) idiom. This is consistent with the
    `VTK Coding Standard`_ to limit header inclusion bloat, but is less of a concern in
    Python. It is kept here for consistency, but may be subject to change in the future.

    .. _`VTK Coding Standard`:
        https://docs.google.com/document/d/1nzinw-dR5JQRNi_gb8qwLL5PnkGMK2FETlQGLr10tZw/edit
    """

    def __init__(
        self,
        p: QVTKInteractor,
    ) -> None:
        """Internal class. Do not use.

        Args:
            p (QVTKInteractor): Parent
        """
        QtCore.QObject.__init__(self)

        self.SignalMapper = QtCore.QSignalMapper(self)

        if parse(qtpy.QT_VERSION) >= parse('5.15.0'):
            self.SignalMapper.mappedInt.connect(self.TimerEvent)
        else:
            self.SignalMapper.mapped.connect(self.TimerEvent)

        self.Timers: dict[int, QtCore.QTimer] = {}
        self.Parent = p

    @QtCore.Slot(int)
    def TimerEvent(
        self,
        id: int,
    ) -> None:
        self.Parent.TimerEvent(id)


class QVTKInteractor(vtkGenericRenderWindowInteractor):
    """``QVTKInteractor`` handles relaying ``Qt`` events to ``VTK``.

    See Also:
        - ``QVTKOpenGLNativeWidget``
    """

    vtkCustomEvents = {
        'ContextMenuEvent': int(vtkCommand.UserEvent) + 100,
        'DragEnterEvent': int(vtkCommand.UserEvent) + 101,
        'DragMoveEvent': int(vtkCommand.UserEvent) + 102,
        'DragLeaveEvent': int(vtkCommand.UserEvent) + 103,
        'DropEvent': int(vtkCommand.UserEvent) + 104,
    }

    def __init__(self) -> None:
        self.Internal = _QVTKInteractorInternal(self)

    def Initialize(self) -> None:
        self.Initialized = 1
        self.Enable()

    @QtCore.Slot(int)
    def TimerEvent(
        self,
        timerId: int,
    ) -> None:
        """Timer event slot.

        Handle timer ID.

        Args:
            timerId (int): Timer ID.
        """
        if not self.GetEnabled():
            return

        self.InvokeEvent(vtkCommand.TimerEvent, timerId)

        if self.IsOneShotTimer(timerId):
            self.DestroyTimer(timerId)

    def InternalCreateTimer(
        self,
        timerId: int,
        timerType: int,
        duration: int,
    ) -> int:
        """Create a ``Qt`` timer.

        Creates a ``Qt`` timer with an interval of 10 ms.

        Args:
            timerId (int): Timer ID.
            timerType (int): Timer type. (vtkNotUsed)
            duration (int): Timer duration (ms).

        Returns:
            int: Timer ID.
        """
        timer = QtCore.QTimer(self.Internal)

        timer.start(duration)
        self.Internal.SignalMapper.setMapping(timer, timerId)

        timer.timeout.connect(self.Internal.SignalMapper.map)

        platformTimerId: int = timer.timerId()
        self.Internal.Timers[platformTimerId] = timer

        return platformTimerId

    def InternalDestroyTimer(
        self,
        platformTimerId: int,
    ) -> int:
        """Destroy a ``Qt`` timer.

        Args:
            platformTimerId (int): Platform timer ID.

        Returns:
            int: 1 if successful, 0 otherwise
        """
        keys = list(self.internal.timers)

        timer = self.Internal.Timers[platformTimerId]
        last_timer = self.Internal.Timers[keys[-1]]

        if timer is not last_timer:
            timer.stop()
            timer.deleteLater()
            del self.Internal.Timers[platformTimerId]
            return 1

        return 0
