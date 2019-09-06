/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/*===========================================================================*/

#include <QMainWindow>

class vtkRenderWindow;

class QTestMainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    QTestMainWindow(vtkRenderWindow* renWin, int ac, char** av);

    bool regressionImageResult() const;
  public slots:
    void captureImage();
  private:
    bool             RegressionImageResult;
    vtkRenderWindow* RenderWindow;

    int    argc;
    char** argv;
};
