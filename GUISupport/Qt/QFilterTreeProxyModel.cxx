/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/

#include "QFilterTreeProxyModel.h"


QFilterTreeProxyModel::QFilterTreeProxyModel(QObject* p)
  : QSortFilterProxyModel(p)
{
  this->TreeLevel = 0;
}

QFilterTreeProxyModel::~QFilterTreeProxyModel()
{
}

void QFilterTreeProxyModel::setFilterTreeLevel(int level)
{
  this->TreeLevel = level;
}

bool QFilterTreeProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
  int level = 0;
  QModelIndex pidx = sourceParent;
  while(pidx != QModelIndex())
  {
    pidx = pidx.parent();
    level++;
  }

  if(level < this->TreeLevel)
  {
    return true;
  }

  if(level > this->TreeLevel)
  {
    return filterAcceptsRow(sourceRow, sourceParent.parent());
  }

 QModelIndex idx = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);

 return (sourceModel()->data(idx).toString().contains(filterRegExp()));
}

bool QFilterTreeProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
  QVariant leftData = this->sourceModel()->data(left);
  QVariant rightData = this->sourceModel()->data(right);

  QString leftString = leftData.toString();
  QString rightString = rightData.toString();

  return QString::localeAwareCompare(leftString, rightString) < 0;
}

