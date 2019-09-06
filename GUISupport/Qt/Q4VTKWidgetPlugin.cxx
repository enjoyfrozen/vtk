/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



// Disable warnings that Qt headers give.
#ifdef _MSC_VER
#pragma warning(disable:4127)
#pragma warning(disable:4512)
#endif

#if !defined(_DEBUG)
# if !defined(QT_NO_DEBUG)
#  define QT_NO_DEBUG
# endif
#endif

#include "Q4VTKWidgetPlugin.h"

#include "QVTKWidget.xpm"

// macro for debug printing
#ifndef qDebug
#define qDebug(a)
//#define qDebug(a) printf(a)
#endif

QVTKWidgetPlugin::QVTKWidgetPlugin()
{
  qDebug("QVTKWidgetPlugin instantiated\n");
}

QVTKWidgetPlugin::~QVTKWidgetPlugin()
{
  qDebug("QVTKWidgetPlugin destructed\n");
}

//! return the name of this widget
QString QVTKWidgetPlugin::name() const
{
  qDebug("QVTKWidgetPlugin::name\n");
  return "QVTKWidget";
}

QString QVTKWidgetPlugin::domXml() const
{
  return QLatin1String("<widget class=\"QVTKWidget\" name=\"qvtkWidget\">\n"
                       " <property name=\"geometry\">\n"
                       "  <rect>\n"
                       "   <x>0</x>\n"
                       "   <y>0</y>\n"
                       "   <width>100</width>\n"
                       "   <height>100</height>\n"
                       "  </rect>\n"
                       " </property>\n"
                       "</widget>\n");
}

QWidget* QVTKWidgetPlugin::createWidget(QWidget* parent)
{
  qDebug("QVTKWidgetPlugin::createWidget\n");
  QVTKWidget* widget = new QVTKWidget(parent);

  // make black background
  QPalette p = widget->palette();
  p.setColor(QPalette::Background, QColor("black"));
  widget->setPalette(p);
  widget->setAutoFillBackground(true);

  // return the widget
  return widget;
}

QString QVTKWidgetPlugin::group() const
{
  qDebug("QVTKWidgetPlugin::group\n");
  return "QVTK";
}

QIcon QVTKWidgetPlugin::icon() const
{
  qDebug("QVTKWidgetPlugin::icon\n");
  return QIcon( QPixmap( QVTKWidget_image ) );
}

//! the name of the include file for building an app with a widget
QString QVTKWidgetPlugin::includeFile() const
{
  qDebug("QVTKWidgetPlugin::includeFile\n");
  return "QVTKWidget.h";
}

//! tool tip text
QString QVTKWidgetPlugin::toolTip() const
{
  qDebug("QVTKWidgetPlugin::toolTip\n");
  return "Qt VTK Widget";
}

//! what's this text
QString QVTKWidgetPlugin::whatsThis() const
{
  qDebug("QVTKWidgetPlugin::whatsThis\n");
  return "A Qt/VTK Graphics Window";
}

//! returns whether widget is a container
bool QVTKWidgetPlugin::isContainer() const
{
  qDebug("QVTKWidgetPlugin::isContainer\n");
  return false;
}

QVTKPlugin::QVTKPlugin()
{
  mQVTKWidgetPlugin = new QVTKWidgetPlugin;
}
QVTKPlugin::~QVTKPlugin()
{
  delete mQVTKWidgetPlugin;
}

QList<QDesignerCustomWidgetInterface*> QVTKPlugin::customWidgets() const
{
  QList<QDesignerCustomWidgetInterface*> plugins;
  plugins.append(mQVTKWidgetPlugin);
  return plugins;
}
