"""A ``QEvent`` translator.

``QVTKInteractorAdapter`` translates ``QEvents`` and sends them to a
``vtkRenderWindowInteractor``.
"""

from __future__ import annotations

import sys

import qtpy
from packaging.version import parse
from qtpy import QtCore, QtWidgets
from vtkmodules.vtkCommonCore import vtkCommand
from vtkmodules.vtkRenderingCore import VTKI_MAX_POINTERS, vtkRenderWindowInteractor

from .QVTKInteractor import QVTKInteractor

ASCII_TO_KEY_SYM = {
    0: None,
    1: None,
    2: None,
    3: None,
    4: None,
    5: None,
    6: None,
    7: None,
    8: None,
    9: 'Tab',
    10: None,
    11: None,
    12: None,
    13: None,
    14: None,
    15: None,
    16: None,
    17: None,
    18: None,
    19: None,
    20: None,
    21: None,
    22: None,
    23: None,
    24: None,
    25: None,
    26: None,
    27: None,
    28: None,
    29: None,
    30: None,
    31: None,
    32: 'space',
    33: 'exclam',
    34: 'quotedbl',
    35: 'numbersign',
    36: 'dollar',
    37: 'percent',
    38: 'ampersand',
    39: 'quoteright',
    40: 'parenleft',
    41: 'parenright',
    42: 'asterisk',
    43: 'plus',
    44: 'comma',
    45: 'minus',
    46: 'period',
    47: 'slash',
    48: '0',
    49: '1',
    50: '2',
    51: '3',
    52: '4',
    53: '5',
    54: '6',
    55: '7',
    56: '8',
    57: '9',
    58: 'colon',
    59: 'semicolon',
    60: 'less',
    61: 'equal',
    62: 'greater',
    63: 'question',
    64: 'at',
    65: 'A',
    66: 'B',
    67: 'C',
    68: 'D',
    69: 'E',
    70: 'F',
    71: 'G',
    72: 'H',
    73: 'I',
    74: 'J',
    75: 'K',
    76: 'L',
    77: 'M',
    78: 'N',
    79: 'O',
    80: 'P',
    81: 'Q',
    82: 'R',
    83: 'S',
    84: 'T',
    85: 'U',
    86: 'V',
    87: 'W',
    88: 'X',
    89: 'Y',
    90: 'Z',
    91: 'bracketleft',
    92: 'backslash',
    93: 'bracketright',
    94: 'asciicircum',
    95: 'underscore',
    96: 'quoteleft',
    97: 'a',
    98: 'b',
    99: 'c',
    100: 'd',
    101: 'e',
    102: 'f',
    103: 'g',
    104: 'h',
    105: 'i',
    106: 'j',
    107: 'k',
    108: 'l',
    109: 'm',
    110: 'n',
    111: 'o',
    112: 'p',
    113: 'q',
    114: 'r',
    115: 's',
    116: 't',
    117: 'u',
    118: 'v',
    119: 'w',
    120: 'x',
    121: 'y',
    122: 'z',
    123: 'braceleft',
    124: 'bar',
    125: 'braceright',
    126: 'asciitilde',
    127: 'Delete',
}


QT_KEY_TO_KEY_SYM = {
    QtCore.Qt.Key.Key_Backspace: 'Backspace',
    QtCore.Qt.Key.Key_Tab: 'Tab',
    QtCore.Qt.Key.Key_Backtab: 'Tab',
    QtCore.Qt.Key.Key_Return: 'Return',
    QtCore.Qt.Key.Key_Enter: 'Return',
    QtCore.Qt.Key.Key_Shift: 'Shift_L',
    QtCore.Qt.Key.Key_Control: 'Control_L',
    QtCore.Qt.Key.Key_Alt: 'Alt_L',
    QtCore.Qt.Key.Key_Pause: 'Pause',
    QtCore.Qt.Key.Key_CapsLock: 'Caps_Lock',
    QtCore.Qt.Key.Key_Escape: 'Escape',
    QtCore.Qt.Key.Key_Space: 'space',
    QtCore.Qt.Key.Key_End: 'End',
    QtCore.Qt.Key.Key_Home: 'Home',
    QtCore.Qt.Key.Key_Left: 'Left',
    QtCore.Qt.Key.Key_Up: 'Up',
    QtCore.Qt.Key.Key_Right: 'Right',
    QtCore.Qt.Key.Key_Down: 'Down',
    QtCore.Qt.Key.Key_SysReq: 'Snapshot',
    QtCore.Qt.Key.Key_Insert: 'Insert',
    QtCore.Qt.Key.Key_Delete: 'Delete',
    QtCore.Qt.Key.Key_Help: 'Help',
    QtCore.Qt.Key.Key_0: ('KP_0', '0'),
    QtCore.Qt.Key.Key_1: ('KP_1', '1'),
    QtCore.Qt.Key.Key_2: ('KP_2', '2'),
    QtCore.Qt.Key.Key_3: ('KP_3', '3'),
    QtCore.Qt.Key.Key_4: ('KP_4', '4'),
    QtCore.Qt.Key.Key_5: ('KP_5', '5'),
    QtCore.Qt.Key.Key_6: ('KP_6', '6'),
    QtCore.Qt.Key.Key_7: ('KP_7', '7'),
    QtCore.Qt.Key.Key_8: ('KP_8', '8'),
    QtCore.Qt.Key.Key_9: ('KP_9', '9'),
    QtCore.Qt.Key.Key_A: 'a',
    QtCore.Qt.Key.Key_B: 'b',
    QtCore.Qt.Key.Key_C: 'c',
    QtCore.Qt.Key.Key_D: 'd',
    QtCore.Qt.Key.Key_E: 'e',
    QtCore.Qt.Key.Key_F: 'f',
    QtCore.Qt.Key.Key_G: 'g',
    QtCore.Qt.Key.Key_H: 'h',
    QtCore.Qt.Key.Key_I: 'i',
    QtCore.Qt.Key.Key_J: 'j',
    QtCore.Qt.Key.Key_K: 'k',
    QtCore.Qt.Key.Key_L: 'l',
    QtCore.Qt.Key.Key_M: 'm',
    QtCore.Qt.Key.Key_N: 'n',
    QtCore.Qt.Key.Key_O: 'o',
    QtCore.Qt.Key.Key_P: 'p',
    QtCore.Qt.Key.Key_Q: 'q',
    QtCore.Qt.Key.Key_R: 'r',
    QtCore.Qt.Key.Key_S: 's',
    QtCore.Qt.Key.Key_T: 't',
    QtCore.Qt.Key.Key_U: 'u',
    QtCore.Qt.Key.Key_V: 'v',
    QtCore.Qt.Key.Key_W: 'w',
    QtCore.Qt.Key.Key_X: 'x',
    QtCore.Qt.Key.Key_Y: 'y',
    QtCore.Qt.Key.Key_Z: 'z',
    QtCore.Qt.Key.Key_Asterisk: 'asterisk',
    QtCore.Qt.Key.Key_Plus: 'plus',
    QtCore.Qt.Key.Key_Minus: 'minus',
    QtCore.Qt.Key.Key_Period: 'period',
    QtCore.Qt.Key.Key_Slash: 'slash',
    QtCore.Qt.Key.Key_F1: 'F1',
    QtCore.Qt.Key.Key_F2: 'F2',
    QtCore.Qt.Key.Key_F3: 'F3',
    QtCore.Qt.Key.Key_F4: 'F4',
    QtCore.Qt.Key.Key_F5: 'F5',
    QtCore.Qt.Key.Key_F6: 'F6',
    QtCore.Qt.Key.Key_F7: 'F7',
    QtCore.Qt.Key.Key_F8: 'F8',
    QtCore.Qt.Key.Key_F9: 'F9',
    QtCore.Qt.Key.Key_F10: 'F10',
    QtCore.Qt.Key.Key_F11: 'F11',
    QtCore.Qt.Key.Key_F12: 'F12',
    QtCore.Qt.Key.Key_F13: 'F13',
    QtCore.Qt.Key.Key_F14: 'F14',
    QtCore.Qt.Key.Key_F15: 'F15',
    QtCore.Qt.Key.Key_F16: 'F16',
    QtCore.Qt.Key.Key_F17: 'F17',
    QtCore.Qt.Key.Key_F18: 'F18',
    QtCore.Qt.Key.Key_F19: 'F19',
    QtCore.Qt.Key.Key_F20: 'F20',
    QtCore.Qt.Key.Key_F21: 'F21',
    QtCore.Qt.Key.Key_F22: 'F22',
    QtCore.Qt.Key.Key_F23: 'F23',
    QtCore.Qt.Key.Key_F24: 'F24',
    QtCore.Qt.Key.Key_NumLock: 'Num_Lock',
    QtCore.Qt.Key.Key_ScrollLock: 'Scroll_Lock',
}


class QVTKInteractorAdapter(QtCore.QObject):

    DevicePixelRatioTolerance: float = 1e-5

    def __init__(
        self,
        parent: QtCore.QObject | None = None,
    ) -> None:
        QtCore.QObject.__init__(self, parent)

        self.AccumulatedDelta: int = 0
        self.DevicePixelRatio: float = 1.0

    def GetDevicePixelRatio(self) -> float:
        """Return the device pixel ratio.

        Returns:
            float: Device pixel ratio.
        """
        return self.DevicePixelRatio

    def SetDevicePixelRatio(
        self,
        ratio: float,
        iren: vtkRenderWindowInteractor | None = None,
    ) -> None:
        """Set the device pixel ratio.

        This defaults to 1.0, but in ``Qt5`` it can be not equal to 1.0.

        Args:
            ratio (float): Pixel ratio.
            iren (vtkRenderWindowInteractor | None, optional): Render window interactor.
                Defaults to None.
        """
        if ratio != self.DevicePixelRatio:
            if iren is not None:
                width, height = iren.GetSize()
                if ratio == 1.0:
                    iren.SetSize(width / 2, height / 2)
                else:
                    iren.SetSize(
                        width * ratio + self.DevicePixelRatio,
                        height * ratio + self.DevicePixelRatio,
                    )
            self.DevicePixelRatio = ratio

    def _get_ctrl_shift(
        self,
        event: QtCore.QEvent,
    ) -> tuple[int, int]:
        """Helper function to return whether control and/or shift keys are pressed.

        Args:
            event (QtCore.QEvent): Keyboard event.

        Returns:
            tuple[int, int]: (ctrl, shift)
        """
        ctrl, shift = 0, 0

        if hasattr(event, 'modifiers'):
            if event.modifiers() & QtCore.Qt.KeyboardModifier.ShiftModifier:
                shift = 1
            if event.modifiers() & QtCore.Qt.KeyboardModifier.ControlModifier:
                ctrl = 1

        return ctrl, shift

    def _get_key_char_and_sym(
        self,
        event: QtCore.QEvent,
    ) -> tuple[str, str]:
        """Helper function to return the key character and symbol from a key press event.

        Args:
            event (QtCore.QEvent): Key press event.

        Returns:
            tuple[str, str]: (ascii_key, keysym)
        """
        if not hasattr(event, 'text'):
            return '\0', 'None'

        text = event.text()
        ascii_key = text if len(text) != 0 else '\0'

        keysym = ASCII_TO_KEY_SYM.get(
            ord(ascii_key),
            QT_KEY_TO_KEY_SYM.get(
                event.key(),
                'None',
            ),
        )

        modifiers = event.modifiers()
        if modifiers is QtCore.Qt.KeypadModifier:
            if isinstance(keysym, tuple):
                keypad_num, key_num = keysym
                keysym = keypad_num if modifiers & QtCore.Qt.KeypadModifier else key_num

        return ascii_key, keysym

    def _process_hover_leave_event(
        self,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process hover leave events.

        Args:
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        iren.InvokeEvent(vtkCommand.HoverEvent, None)

    def _set_event_info_flip_y(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
        position: QtCore.QPointF,
        index: int | None = None,
    ) -> None:
        """Helper function to set event information flip y.

        This calls ``VTK`` method ``SetEventInformationFlipY``.

        Args:
            event (QtCore.QEvent): Event
            iren (vtkRenderWindowInteractor): Render window interactor
            position (QtCore.QPoint | QtCore.QPointF): (x, y)
            index (int | None, optional): Position index. Defaults to None.
        """
        x, y = position.x(), position.y()

        new_x = int(x * self.DevicePixelRatio + self.DevicePixelRatioTolerance)
        new_y = int(y * self.DevicePixelRatio + self.DevicePixelRatioTolerance)

        ctrl, shift = self._get_ctrl_shift(event)
        char, keysym = self._get_key_char_and_sym(event)
        repeat = 1 if event.type() is QtCore.QEvent.MouseButtonDblClick else 0
        ndx = 0 if index is None else 1

        iren.SetEventInformationFlipY(
            new_x,
            new_y,
            ctrl,
            shift,
            char,
            repeat,
            keysym,
            ndx,
        )

        self._set_key_modifiers(event, iren)

    def _set_key_modifiers(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to set key modifiers.

        Args:
            event (QtCore.QEvent): Event.
            iren (vtkRenderWindowInteractor): Render window interactor
        """
        if hasattr(event, 'modifiers'):
            modifiers = event.modifiers()
            if modifiers is not None:
                if modifiers & QtCore.Qt.AltModifier > 0:
                    iren.SetAltKey(1)  # True
                else:
                    iren.SetAltKey(0)  # False

    def _process_button_press_event(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        if event.button() is QtCore.Qt.LeftButton:
            iren.InvokeEvent(vtkCommand.LeftButtonPressEvent, str(event))
        if event.button() is QtCore.Qt.MiddleButton:
            iren.InvokeEvent(vtkCommand.MiddleButtonPressEvent, str(event))
        if event.button() is QtCore.Qt.RightButton:
            iren.InvokeEvent(vtkCommand.RightButtonPressEvent, str(event))

    def _process_double_click_event(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        if event.button() is QtCore.Qt.LeftButton:
            iren.InvokeEvent(vtkCommand.LeftButtonDoubleClickEvent, str(event))
        if event.button() is QtCore.Qt.MiddleButton:
            iren.InvokeEvent(vtkCommand.MiddleButtonDoubleClickEvent, str(event))
        if event.button() is QtCore.Qt.RightButton:
            iren.InvokeEvent(vtkCommand.RightButtonDoubleClickEvent, str(event))

    def _process_mouse_release_event(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        if event.button() is QtCore.Qt.LeftButton:
            iren.InvokeEvent(vtkCommand.LeftButtonReleaseEvent, str(event))
        if event.button() is QtCore.Qt.MiddleButton:
            iren.InvokeEvent(vtkCommand.MiddleButtonReleaseEvent, str(event))
        if event.button() is QtCore.Qt.RightButton:
            iren.InvokeEvent(vtkCommand.RightButtonReleaseEvent, str(event))

    def _process_touch_events(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process touch events.

        Args:
            event (QtCore.QEvent): Touch event.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        points = (
            event.touchPoints()
            if parse(qtpy.QT_VERSION) < parse('6.0.0')
            else event.points()
        )

        for point in points:
            if point.id() >= VTKI_MAX_POINTERS:
                break

            pos = (
                QtCore.QPointF(point.pos())
                if parse(qtpy.QT_VERSION) < parse('6.0.0')
                else QtCore.QPointF(point.position())
            )
            ndx = point.id()

            self._set_event_info_flip_y(event, iren, pos, ndx)
            iren.SetPointerIndex(ndx)

            state = point.state()

            if state & QtCore.Qt.TouchPointReleased:
                iren.InvokeEvent(vtkCommand.LeftButtonReleaseEvent, None)
            if state & QtCore.Qt.TouchPointPressed:
                iren.InvokeEvent(vtkCommand.LeftButtonPressEvent, None)
            if state & QtCore.Qt.TouchPointMoved:
                iren.InvokeEvent(vtkCommand.MouseMoveEvent, None)

    def _set_key_event_info(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to set key event info.

        Calls ``VTK`` ``SetKeyEventInformation``.

        Args:
            event (QtCore.QEvent): Event.
            iren (vtkRenderWindowInteractor): Render window interactor
        """
        ctrl, shift = self._get_ctrl_shift(event)
        repeat = event.count()
        char, keysym = self._get_key_char_and_sym(event)

        iren.SetKeyEventInformation(
            ctrl,
            shift,
            char,
            repeat,
            keysym,
        )

        self._set_key_modifiers(event, iren)

    def _process_wheel_events(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process wheel events.

        Args:
            event (QtCore.QEvent): Event.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        point = (
            QtCore.QPointF(event.x(), event.y())
            if parse(qtpy.QT_VERSION) < parse('6.0.0')
            else QtCore.QPointF(event.position())
        )

        self._set_event_info_flip_y(event, iren, point)

        delta_x, delta_y = event.angleDelta().x(), event.angleDelta().y()
        self.AccumulatedDelta += delta_x + delta_y
        threshold = 120

        if self.AccumulatedDelta >= threshold and delta_y != 0.0:
            iren.InvokeEvent(vtkCommand.MouseWheelForwardEvent, str(event))
            self.AccumulatedDelta = 0
        elif self.AccumulatedDelta <= -threshold and delta_y != 0.0:
            iren.InvokeEvent(vtkCommand.MouseWheelBackwardEvent, str(event))
            self.AccumulatedDelta = 0
        elif self.AccumulatedDelta >= threshold and delta_x != 0.0:
            iren.InvokeEvent(vtkCommand.MouseWheelLeftEvent, str(event))
            self.AccumulatedDelta = 0
        elif self.AccumulatedDelta <= -threshold and delta_x != 0.0:
            iren.InvokeEvent(vtkCommand.MouseWheelRightEvent, str(event))
            self.AccumulatedDelta = 0

    def _process_context_menu_event(
        self,
        event: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """_summary_

        Args:
            event (QtCore.QEvent): _description_
            iren (vtkRenderWindowInteractor): _description_
        """
        point = QtCore.QPointF(event.x(), event.y())
        self._set_event_info_flip_y(event, iren, point)
        iren.InvokeEvent(QVTKInteractor.vtkCustomEvents['ContextMenuEvent'], str(event))

    def _process_swipe_gesture(
        self,
        event: QtCore.QEvent,
        swipe: QtWidgets.QSwipeGesture,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process swipe gesture events.

        Args:
            event (QtCore.QEvent): Event.
            swipe (QtWidgets.QSwipeGesture): Swipe gesture.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        event.accept(QtCore.Qt.SwipeGesture)
        angle = swipe.swipeAngle()
        iren.SetRotation(angle)

        state = swipe.state()

        if state is QtCore.Qt.GestureCanceled or state is QtCore.Qt.GestureFinished:
            iren.InvokeEvent(vtkCommand.EndSwipeEvent, str(event))
        elif state is QtCore.Qt.GestureStarted:
            iren.InvokeEvent(vtkCommand.StartSwipeEvent, str(event))
            iren.InvokeEvent(vtkCommand.SwipeEvent, str(event))
        else:
            iren.InvokeEvent(vtkCommand.SwipeEvent, str(event))

    def _process_pinch_gesture(
        self,
        event: QtCore.QEvent,
        pinch: QtWidgets.QPinchGesture,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process pinch gesture events.

        Args:
            event (QtCore.QEvent): Event.
            pinch (QtWidgets.QSwipeGesture): Pinch gesture.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        event.accept(QtCore.Qt.PinchGesture)

        pos = pinch.centerPoint().toPoint()

        if sys.platform == 'darwin':
            # When using MacOS trackpad, the center of the pinch event is already reported
            # in widget coordinates. For other platforms, the coordinates need to be
            # converted from global to local.
            widget = self.parent()
            if widget is not None:
                pos = widget.mapFromGlobal(pos)

        self._set_event_info_flip_y(event, iren, pos)

        iren.SetScale(1.0)
        iren.SetScale(pinch.scaleFactor())

        state = pinch.state()

        if state in {QtCore.Qt.GestureCanceled, QtCore.Qt.GestureFinished}:
            iren.InvokeEvent(vtkCommand.EndPinchEvent, str(event))
        elif state is QtCore.Qt.GestureStarted:
            iren.InvokeEvent(vtkCommand.StartPinchEvent, str(event))
            iren.InvokeEventint(vtkCommand.PinchEvent, str(event))
        else:
            iren.InvokeEvent(vtkCommand.PinchEvent, str(event))

        iren.SetRotation(-1.0 * pinch.lastRotationAngle())
        iren.SetRotation(-1.0 * pinch.rotationAngle())

        if state in {QtCore.Qt.GestureCanceled, QtCore.Qt.GestureFinished}:
            iren.InvokeEvent(vtkCommand.EndRotateEvent, str(event))
        elif state is QtCore.Qt.GestureStarted:
            iren.InvokeEvent(vtkCommand.StartRotateEvent, str(event))
            iren.InvokeEvent(vtkCommand.RotateEvent, str(event))
        else:
            iren.InvokeEvent(vtkCommand.RotateEvent, str(event))

    def _process_pan_gesture(
        self,
        event: QtCore.QEvent,
        pan: QtWidgets.QPanGesture,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process pan gesture events.

        Args:
            event (QtCore.QEvent): Event.
            pan (QtWidgets.QSwipeGesture): Pan gesture.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        event.accept(QtCore.Qt.PanGesture)

        delta = QtCore.QPointF(pan.delta())
        translation = [
            delta.x() * self.DevicePixelRatio + self.DevicePixelRatioTolerance,
            -delta.y() * self.DevicePixelRatio + self.DevicePixelRatioTolerance,
        ]
        iren.SetTranslation(translation)

        state = pan.state()
        if state in {QtCore.Qt.GestureCanceled, QtCore.Qt.GestureFinished}:
            iren.InvokeEvent(vtkCommand.EndPanEvent, str(event))
        elif state is QtCore.Qt.GestureStarted:
            iren.InvokeEvent(vtkCommand.StartPanEvent, str(event))
            iren.InvokeEvent(vtkCommand.PanEvent, str(event))
        else:
            iren.InvokeEvent(vtkCommand.PanEvent, str(event))

    def _process_tap_gesture(
        self,
        event: QtCore.QEvent,
        tap: QtWidgets.QTapGesture,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process tap gesture events.

        Args:
            event (QtCore.QEvent): Event.
            tap (QtWidgets.QSwipeGesture): Tap gesture.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        event.accept(QtCore.Qt.TapGesture)
        pos = QtCore.QPointF(tap.position().toPoint())
        self._set_event_info_flip_y(event, iren, pos)

        state = tap.state()
        if state is QtCore.Qt.GestureStarted:
            iren.InvokeEvent(vtkCommand.TapEvent, str(event))

    def _process_tap_and_hold_gesture(
        self,
        event: QtCore.QEvent,
        tap_and_hold: QtWidgets.QTapAndHoldGesture,
        iren: vtkRenderWindowInteractor,
    ) -> None:
        """Helper function to process tap and hold gesture events.

        Args:
            event (QtCore.QEvent): Event.
            tap_and_hold (QtWidgets.QSwipeGesture): Tap and hold gesture.
            iren (vtkRenderWindowInteractor): Render window interactor.
        """
        event.accept(QtCore.Qt.TapAndHoldGesture)

        pos = QtCore.QPointF(tap_and_hold.position().toPoint())
        widget = self.parent()

        if widget is not None:
            pos = widget.mapFromGlobal(pos)

        self._set_event_info_flip_y(event, iren, pos)

        state = tap_and_hold.state()

        if state is QtCore.Qt.GestureStarted:
            iren.InvokeEvent(vtkCommand.LongTapEvent, str(event))

    def ProcessEvent(
        self,
        e: QtCore.QEvent,
        iren: vtkRenderWindowInteractor,
    ) -> bool:
        """Process a ``QEvent`` and send it to the interactor.

        Args:
            e (QtCore.QEvent): Event.
            iren (vtkRenderWindowInteractor): Render window interactor.

        Returns:
            bool: ``True`` if event accepted, ``False`` otherwise.
        """
        if iren is None or e is None:
            return False

        type_ = e.type()

        if type_ is QtCore.QEvent.HoverLeave:
            self._process_hover_leave_event(iren)

        if not iren.GetEnabled():
            return False

        mouse_events = {
            QtCore.QEvent.MouseButtonPress,
            QtCore.QEvent.MouseButtonRelease,
            QtCore.QEvent.MouseButtonDblClick,
            QtCore.QEvent.MouseMove,
        }

        if type_ in mouse_events:
            if parse(qtpy.QT_VERSION) < parse('6.0.0'):
                point = QtCore.QPointF(e.x(), e.y())
            else:
                point = QtCore.QPointF(e.position())

            self._set_event_info_flip_y(e, iren, point)

            if type_ is QtCore.QEvent.MouseMove:
                iren.InvokeEvent(vtkCommand.MouseMoveEvent, str(e))
            elif type_ is QtCore.QEvent.MouseButtonPress:
                self._process_button_press_event(e, iren)
            elif type_ is QtCore.QEvent.MouseButtonDblClick:
                self._process_double_click_event(e, iren)
            elif type_ is QtCore.QEvent.MouseButtonRelease:
                self._process_mouse_release_event(e, iren)

            return True

        touch_events = {
            QtCore.QEvent.TouchBegin,
            QtCore.QEvent.TouchUpdate,
            QtCore.QEvent.TouchEnd,
        }

        if type_ in touch_events:
            self._process_touch_events(e, iren)
            e.accept()
            return True

        if type_ is QtCore.QEvent.Enter:
            iren.InvokeEvent(vtkCommand.EnterEvent, str(e))
            return True

        if type_ is QtCore.QEvent.Leave:
            iren.InvokeEvent(vtkCommand.LeaveEvent, str(e))
            return True

        key_events = {
            QtCore.QEvent.KeyPress,
            QtCore.QEvent.KeyRelease,
        }

        if type_ in key_events:
            self._set_key_event_info(e, iren)

            if type_ is QtCore.QEvent.KeyPress:
                iren.InvokeEvent(vtkCommand.KeyPressEvent, str(e))

                char, _ = self._get_key_char_and_sym(e)
                if char != '\0':
                    iren.InvokeEvent(vtkCommand.CharEvent, str(e))
            else:
                iren.InvokeEvent(vtkCommand.KeyReleaseEvent, str(e))

            return True

        if type_ is QtCore.QEvent.Wheel:
            self._process_wheel_events(e, iren)
            return True

        if type_ is QtCore.QEvent.ContextMenu:
            self._process_context_menu_event(e, iren)
            return True

        if type_ is QtCore.QEvent.DragEnter:
            iren.InvokeEvent(QVTKInteractor.vtkCustomEvents['DragEnterEvent'], str(e))
            return True

        if type_ is QtCore.QEvent.DragLeave:
            iren.InvokeEvent(QVTKInteractor.vtkCustomEvents['DragLeaveEvent'], str(e))
            return True

        if type_ is QtCore.QEvent.DragMove:
            if parse(qtpy.QT_VERSION) < parse('6.0.0'):
                pos = QtCore.QPointF(e.pos())
            else:
                point = QtCore.QPointF(e.position().toPoint())

            self._set_event_info_flip_y(e, iren, point)
            iren.InvokeEvent(QVTKInteractor.vtkCustomEvents['DragMoveEvent'], str(e))
            return True

        if type_ is QtCore.QEvent.Drop:
            if parse(qtpy.QT_VERSION) < parse('6.0.0'):
                pos = QtCore.QPointF(e.pos())
            else:
                point = QtCore.QPointF(e.position().toPoint())

            self._set_event_info_flip_y(e, iren, point)

            iren.InvokeEvent(QVTKInteractor.vtkCustomEvents['DropEvent'], str(e))
            return True

        if type_ is QtCore.QEvent.Gesture:
            eventPosition = iren.GetEventPosition()
            lastEventPosition = iren.GetLastEventPosition()

            gesture_event = QtWidgets.QGestureEvent(e)

            swipe = gesture_event.gesture(QtCore.Qt.QSwipeGesture)
            if swipe is not None:
                self._process_swipe_gesture(e, swipe, iren)

            pinch = gesture_event.gesture(QtCore.Qt.PinchGesture)
            if pinch is not None:
                self._process_pinch_gesture(e, pinch, iren)

            pan = gesture_event.gesture(QtCore.Qt.PanGesture)
            if pan is not None:
                self._process_pan_gesture(e, pan, iren)

            tap = gesture_event.gesture(QtCore.Qt.TapGesture)
            if tap is not None:
                self._process_pan_gesture(e, tap, iren)

            tap_and_hold = gesture_event.gesture(QtCore.Qt.TapAndHoldGesture)
            if tap_and_hold is not None:
                self._process_tap_and_hold_gesture(e, tap_and_hold, iren)

            iren.SetEventPosition(eventPosition)
            iren.SetLastEventPosition(lastEventPosition)

            return True

        return False
