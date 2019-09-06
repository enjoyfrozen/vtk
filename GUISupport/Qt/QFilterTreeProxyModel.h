/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
// .NAME QFilterTreeProxyModel - An implementation of a QSortFilterProxyModel
//    tailored for hierarchical models.
//
// .SECTION Description
// An implementation of a QSortFilterProxyModel tailored for hierarchical
// models. It allows you to filter the model based on the content in a
// certain column of a certain level in the tree. Indices above that level
// in the tree are retained. Indices below the level are kept if their
// ancestor at the tree level is kept.
//
// .SECTION See also

#ifndef __QFilterTreeProxyModel_h
#define __QFilterTreeProxyModel_h

#include "vtkGUISupportQtModule.h" // For export macro
#include "QVTKWin32Header.h"
#include <QSortFilterProxyModel>

class QModelIndex;

class VTKGUISUPPORTQT_EXPORT QFilterTreeProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:

  QFilterTreeProxyModel(QObject* p = nullptr);
  ~QFilterTreeProxyModel() override;

  // Description:
  // The 0-based level in the tree hierarchy to filter on. The root is level 0.
  void setFilterTreeLevel(int level);

protected:

  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
  bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:

  int TreeLevel;
};

#endif
