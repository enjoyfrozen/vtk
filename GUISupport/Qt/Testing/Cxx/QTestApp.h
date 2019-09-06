/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#ifndef QTestApp_h
#define QTestApp_h

#include <QApplication>
#include <QVector>
#include <QByteArray>

class QTestApp
{
public:
  QTestApp(int _argc, char** _argv);
  ~QTestApp();

  static int exec();

  static void messageHandler(QtMsgType type,
    const QMessageLogContext & context,
    const QString & message);

  static void delay(int ms);

  static void simulateEvent(QWidget* w, QEvent* e);

  static void keyUp(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms);

  static void keyDown(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms);

  static void keyClick(QWidget* w, Qt::Key key, Qt::KeyboardModifiers mod, int ms);

  static void mouseDown(QWidget* w, QPoint pos, Qt::MouseButton btn,
                        Qt::KeyboardModifiers mod, int ms);

  static void mouseUp(QWidget* w, QPoint pos, Qt::MouseButton btn,
                      Qt::KeyboardModifiers mod, int ms);

  static void mouseMove(QWidget* w, QPoint pos, Qt::MouseButton btn,
                        Qt::KeyboardModifiers mod, int ms);

  static void mouseClick(QWidget* w, QPoint pos, Qt::MouseButton btn,
                         Qt::KeyboardModifiers mod, int ms);

private:
  QApplication* App;
  static int Error;
  QList<QByteArray> Argv;
  QVector<char*> Argvp;
  int Argc;
};

#endif

