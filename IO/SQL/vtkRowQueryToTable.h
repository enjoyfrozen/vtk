/*===========================================================================*/
/* Distributed under OSI-approved BSD 3-Clause License.                      */
/* For copyright, see the following accompanying files or https://vtk.org:   */
/* - VTK-Copyright.txt                                                       */
/* - Sandia-Copyright.txt                                                    */
/*===========================================================================*/
/**
 * @class   vtkRowQueryToTable
 * @brief   executes an sql query and retrieves results into a table
 *
 *
 * vtkRowQueryToTable creates a vtkTable with the results of an arbitrary SQL
 * query.  To use this filter, you first need an instance of a vtkSQLDatabase
 * subclass.  You may use the database class to obtain a vtkRowQuery instance.
 * Set that query on this filter to extract the query as a table.
 *
 * @par Thanks:
 * Thanks to Andrew Wilson from Sandia National Laboratories for his work
 * on the database classes.
 *
 * @sa
 * vtkSQLDatabase vtkRowQuery
*/

#ifndef vtkRowQueryToTable_h
#define vtkRowQueryToTable_h

#include "vtkIOSQLModule.h" // For export macro
#include "vtkTableAlgorithm.h"

class vtkRowQuery;

class VTKIOSQL_EXPORT vtkRowQueryToTable : public vtkTableAlgorithm
{
public:
  static vtkRowQueryToTable* New();
  vtkTypeMacro(vtkRowQueryToTable, vtkTableAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * The query to execute.
   */
  void SetQuery(vtkRowQuery* query);
  vtkGetObjectMacro(Query, vtkRowQuery);
  //@}

  /**
   * Update the modified time based on the query.
   */
  vtkMTimeType GetMTime() override;

protected:
  vtkRowQueryToTable();
  ~vtkRowQueryToTable() override;

  vtkRowQuery* Query;

  int RequestData(
    vtkInformation*,
    vtkInformationVector**,
    vtkInformationVector*) override;

private:
  vtkRowQueryToTable(const vtkRowQueryToTable&) = delete;
  void operator=(const vtkRowQueryToTable&) = delete;
};

#endif

