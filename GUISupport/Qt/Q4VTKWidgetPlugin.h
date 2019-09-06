/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/



#ifndef QVTK_WIDGET_PLUGIN
#define QVTK_WIDGET_PLUGIN

// Disable warnings that Qt headers give.
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <QDesignerCustomWidgetInterface>
#include <QDesignerCustomWidgetCollectionInterface>
#include <QObject>
#include <QtPlugin>
#include <QWidget>

// implement Designer Custom Widget interface
class QVTKWidgetPlugin : public QDesignerCustomWidgetInterface
{
  public:
    QVTKWidgetPlugin();
    ~QVTKWidgetPlugin() override;

    QString name() const override;
    QString domXml() const override;
    QWidget* createWidget(QWidget* parent = 0) override;
    QString group() const override;
    QIcon icon() const override;
    QString includeFile() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    bool isContainer() const override;
};

// implement designer widget collection interface
class QVTKPlugin : public QObject, public QDesignerCustomWidgetCollectionInterface
{
  Q_OBJECT
  #if QT_VERSION >= 0x050000
  Q_PLUGIN_METADATA(IID "org.vtk.qvtkplugin")
  #endif
  Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)
  public:
  QVTKPlugin();
  ~QVTKPlugin() override;

  QList<QDesignerCustomWidgetInterface*> customWidgets() const override;
  private:
    QVTKWidgetPlugin* mQVTKWidgetPlugin;
};

// fake QVTKWidget class to satisfy the designer
class QVTKWidget : public QWidget
{
  Q_OBJECT
public:
  QVTKWidget(QWidget* p) : QWidget(p) {}
};

// Undo disabling of warning.
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#endif //QVTK_WIDGET_PLUGIN
