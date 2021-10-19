/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTableBasedClipDataSet.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

/*****************************************************************************
 *
 * Copyright (c) 2000 - 2009, Lawrence Livermore National Security, LLC
 * Produced at the Lawrence Livermore National Laboratory
 * LLNL-CODE-400124
 * All rights reserved.
 *
 * This file was adapted from the VisIt clipper (vtkVisItClipper). For  details,
 * see https://visit.llnl.gov/.  The full copyright notice is contained in the
 * file COPYRIGHT located at the root of the VisIt distribution or at
 * http://www.llnl.gov/visit/copyright.html.
 *
 *****************************************************************************/

#include "vtkTableBasedClipDataSet.h"

#include "vtkAppendFilter.h"
#include "vtkCallbackCommand.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkClipDataSet.h"
#include "vtkDoubleArray.h"
#include "vtkExecutive.h"
#include "vtkGenericCell.h"
#include "vtkImageData.h"
#include "vtkImplicitFunction.h"
#include "vtkIncrementalPointLocator.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkLogger.h"
#include "vtkMergePoints.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"
#include "vtkRectilinearGrid.h"
#include "vtkSMPTools.h"
#include "vtkSmartPointer.h"
#include "vtkStructuredGrid.h"
#include "vtkTypeInt32Array.h"
#include "vtkTypeInt64Array.h"
#include "vtkUnsignedCharArray.h"
#include "vtkUnstructuredGrid.h"

#include <array>
#include <numeric>
#include <vector>

// NOLINTNEXTLINE(bugprone-suspicious-include)
#include "vtkTableBasedClipCases.cxx"

vtkStandardNewMacro(vtkTableBasedClipDataSet);
vtkCxxSetObjectMacro(vtkTableBasedClipDataSet, ClipFunction, vtkImplicitFunction);

// ============================================================================
// ============== vtkTableBasedClipperDataSetFromVolume (begin) ===============
// ============================================================================

namespace detail
{
class ConstructSimpleDataSetsFunctor;
}

struct TableBasedClipperPointEntry
{
  vtkIdType ptIds[2];
  double percent;
};

// ---- vtkTableBasedClipperPointList (begin)
class vtkTableBasedClipperPointList
{
public:
  vtkTableBasedClipperPointList();
  virtual ~vtkTableBasedClipperPointList();

  vtkIdType AddPoint(vtkIdType, vtkIdType, double);
  vtkIdType GetTotalNumberOfPoints() const;
  int GetNumberOfLists() const;
  int GetList(vtkIdType, const TableBasedClipperPointEntry*&) const;

protected:
  vtkIdType currentList;
  vtkIdType currentPoint;
  int listSize;
  int pointsPerList;
  TableBasedClipperPointEntry** list;
};
// ---- vtkTableBasedClipperPointList (end)

// ---- vtkTableBasedClipperEdgeHashEntry (begin)
class vtkTableBasedClipperEdgeHashEntry
{
public:
  vtkTableBasedClipperEdgeHashEntry();
  virtual ~vtkTableBasedClipperEdgeHashEntry() = default;

  int GetPointId() { return ptId; }
  void SetInfo(int, int, int);
  void SetNext(vtkTableBasedClipperEdgeHashEntry* n) { next = n; }
  bool IsMatch(int i1, int i2) { return (i1 == id1 && i2 == id2 ? true : false); }

  vtkTableBasedClipperEdgeHashEntry* GetNext() { return next; }

protected:
  int id1, id2;
  int ptId;
  vtkTableBasedClipperEdgeHashEntry* next;
};
// ---- vtkTableBasedClipperEdgeHashEntry (end)

// ---- vtkTableBasedClipperEdgeHashEntryMemoryManager (begin)
#define FREE_ENTRY_LIST_SIZE 16384
#define POOL_SIZE 256
class vtkTableBasedClipperEdgeHashEntryMemoryManager
{
public:
  vtkTableBasedClipperEdgeHashEntryMemoryManager();
  virtual ~vtkTableBasedClipperEdgeHashEntryMemoryManager();

  inline vtkTableBasedClipperEdgeHashEntry* GetFreeEdgeHashEntry()
  {
    if (freeEntryindex <= 0)
    {
      AllocateEdgeHashEntryPool();
    }
    freeEntryindex--;
    return freeEntrylist[freeEntryindex];
  }

  inline void ReRegisterEdgeHashEntry(vtkTableBasedClipperEdgeHashEntry* q)
  {
    if (freeEntryindex >= FREE_ENTRY_LIST_SIZE - 1)
    {
      // We've got plenty, so ignore this one.
      return;
    }
    freeEntrylist[freeEntryindex] = q;
    freeEntryindex++;
  }

protected:
  int freeEntryindex;
  vtkTableBasedClipperEdgeHashEntry* freeEntrylist[FREE_ENTRY_LIST_SIZE];
  std::vector<vtkTableBasedClipperEdgeHashEntry*> edgeHashEntryPool;

  void AllocateEdgeHashEntryPool();
};
// ---- vtkTableBasedClipperEdgeHashEntryMemoryManager (end)

// ---- vtkTableBasedClipperEdgeHashTable (begin)
class vtkTableBasedClipperEdgeHashTable
{
public:
  vtkTableBasedClipperEdgeHashTable(int, vtkTableBasedClipperPointList&);
  virtual ~vtkTableBasedClipperEdgeHashTable();

  vtkIdType AddPoint(vtkIdType, vtkIdType, double);

protected:
  int nHashes;
  vtkTableBasedClipperPointList& pointlist;
  vtkTableBasedClipperEdgeHashEntry** hashes;
  vtkTableBasedClipperEdgeHashEntryMemoryManager emm;

  int GetKey(int, int);

private:
  vtkTableBasedClipperEdgeHashTable(const vtkTableBasedClipperEdgeHashTable&) = delete;
  void operator=(const vtkTableBasedClipperEdgeHashTable&) = delete;
};
// ---- vtkTableBasedClipperEdgeHashTable (end)

class vtkTableBasedClipperDataSetFromVolume
{
public:
  vtkTableBasedClipperDataSetFromVolume(vtkIdType ptSizeGuess);
  vtkTableBasedClipperDataSetFromVolume(vtkIdType nPts, vtkIdType ptSizeGuess);
  virtual ~vtkTableBasedClipperDataSetFromVolume() = default;

  vtkIdType AddPoint(vtkIdType p1, vtkIdType p2, double percent)
  {
    return numPrevPts + edges.AddPoint(p1, p2, percent);
  }

protected:
  int numPrevPts;
  vtkTableBasedClipperPointList pt_list;
  vtkTableBasedClipperEdgeHashTable edges;

private:
  vtkTableBasedClipperDataSetFromVolume(const vtkTableBasedClipperDataSetFromVolume&) = delete;
  void operator=(const vtkTableBasedClipperDataSetFromVolume&) = delete;
};

vtkTableBasedClipperPointList::vtkTableBasedClipperPointList()
{
  listSize = 4096;
  pointsPerList = 1024;

  list = new TableBasedClipperPointEntry*[listSize];
  list[0] = new TableBasedClipperPointEntry[pointsPerList];
  for (int i = 1; i < listSize; i++)
  {
    list[i] = nullptr;
  }

  currentList = 0;
  currentPoint = 0;
}

vtkTableBasedClipperPointList::~vtkTableBasedClipperPointList()
{
  for (int i = 0; i < listSize; i++)
  {
    if (list[i] != nullptr)
    {
      delete[] list[i];
    }
    else
    {
      break;
    }
  }

  delete[] list;
}

int vtkTableBasedClipperPointList::GetList(
  vtkIdType listId, const TableBasedClipperPointEntry*& outlist) const
{
  if (listId < 0 || listId > currentList)
  {
    outlist = nullptr;
    return 0;
  }

  outlist = list[listId];
  return (listId == currentList ? currentPoint : pointsPerList);
}

int vtkTableBasedClipperPointList::GetNumberOfLists() const
{
  return currentList + 1;
}

vtkIdType vtkTableBasedClipperPointList::GetTotalNumberOfPoints() const
{
  vtkIdType numFullLists = currentList; // actually currentList-1+1
  vtkIdType numExtra = currentPoint;    // again, currentPoint-1+1

  return numFullLists * pointsPerList + numExtra;
}

vtkIdType vtkTableBasedClipperPointList::AddPoint(vtkIdType pt0, vtkIdType pt1, double percent)
{
  if (currentPoint >= pointsPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      TableBasedClipperPointEntry** tmpList = new TableBasedClipperPointEntry*[2 * listSize];
      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new TableBasedClipperPointEntry[pointsPerList];
    currentPoint = 0;
  }

  list[currentList][currentPoint].ptIds[0] = pt0;
  list[currentList][currentPoint].ptIds[1] = pt1;
  list[currentList][currentPoint].percent = percent;
  currentPoint++;

  return (GetTotalNumberOfPoints() - 1);
}

vtkTableBasedClipperEdgeHashEntry::vtkTableBasedClipperEdgeHashEntry()
{
  id1 = -1;
  id2 = -1;
  ptId = -1;
  next = nullptr;
}

void vtkTableBasedClipperEdgeHashEntry::SetInfo(int i1, int i2, int pId)
{
  id1 = i1;
  id2 = i2;
  ptId = pId;
  next = nullptr;
}

vtkTableBasedClipperEdgeHashEntryMemoryManager::vtkTableBasedClipperEdgeHashEntryMemoryManager()
{
  freeEntryindex = 0;
}

vtkTableBasedClipperEdgeHashEntryMemoryManager::~vtkTableBasedClipperEdgeHashEntryMemoryManager()
{
  int npools = static_cast<int>(edgeHashEntryPool.size());
  for (int i = 0; i < npools; i++)
  {
    vtkTableBasedClipperEdgeHashEntry* pool = edgeHashEntryPool[i];
    delete[] pool;
  }
}

void vtkTableBasedClipperEdgeHashEntryMemoryManager::AllocateEdgeHashEntryPool()
{
  if (freeEntryindex == 0)
  {
    vtkTableBasedClipperEdgeHashEntry* newlist = new vtkTableBasedClipperEdgeHashEntry[POOL_SIZE];
    edgeHashEntryPool.push_back(newlist);

    for (int i = 0; i < POOL_SIZE; i++)
    {
      freeEntrylist[i] = &(newlist[i]);
    }

    freeEntryindex = POOL_SIZE;
  }
}

vtkTableBasedClipperEdgeHashTable::vtkTableBasedClipperEdgeHashTable(
  int nh, vtkTableBasedClipperPointList& p)
  : pointlist(p)
{
  nHashes = nh;
  hashes = new vtkTableBasedClipperEdgeHashEntry*[nHashes];
  for (int i = 0; i < nHashes; i++)
  {
    hashes[i] = nullptr;
  }
}

vtkTableBasedClipperEdgeHashTable::~vtkTableBasedClipperEdgeHashTable()
{
  delete[] hashes;
}

int vtkTableBasedClipperEdgeHashTable::GetKey(int p1, int p2)
{
  int rv = (int)((unsigned int)p1 * 18457U + (unsigned int)p2 * 234749U) % nHashes;

  // In case of overflows and modulo with negative numbers.
  if (rv < 0)
  {
    rv += nHashes;
  }

  return rv;
}

vtkIdType vtkTableBasedClipperEdgeHashTable::AddPoint(vtkIdType ap1, vtkIdType ap2, double apercent)
{
  vtkIdType p1, p2;
  double percent;

  if (ap2 < ap1)
  {
    p1 = ap2;
    p2 = ap1;
    percent = 1.0 - apercent;
  }
  else
  {
    p1 = ap1;
    p2 = ap2;
    percent = apercent;
  }

  int key = GetKey(p1, p2);

  //
  // See if we have any matches in the current hashes.
  //
  vtkTableBasedClipperEdgeHashEntry* cur = hashes[key];
  while (cur != nullptr)
  {
    if (cur->IsMatch(p1, p2))
    {
      //
      // We found a match.
      //
      return cur->GetPointId();
    }

    cur = cur->GetNext();
  }

  //
  // There was no match.  We will have to add a new entry.
  //
  vtkTableBasedClipperEdgeHashEntry* new_one = emm.GetFreeEdgeHashEntry();

  vtkIdType newPt = pointlist.AddPoint(p1, p2, percent);
  new_one->SetInfo(p1, p2, newPt);
  new_one->SetNext(hashes[key]);
  hashes[key] = new_one;

  return newPt;
}

vtkTableBasedClipperDataSetFromVolume::vtkTableBasedClipperDataSetFromVolume(vtkIdType ptSizeGuess)
  : numPrevPts(0)
  , pt_list()
  , edges(ptSizeGuess, pt_list)
{
}

vtkTableBasedClipperDataSetFromVolume::vtkTableBasedClipperDataSetFromVolume(
  vtkIdType nPts, vtkIdType ptSizeGuess)
  : numPrevPts(nPts)
  , pt_list()
  , edges(ptSizeGuess, pt_list)
{
}
// ============================================================================
// ============== vtkTableBasedClipperDataSetFromVolume ( end ) ===============
// ============================================================================

// ============================================================================
// =============== vtkTableBasedClipperVolumeFromVolume (begin) ===============
// ============================================================================

class vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperShapeList(int size);
  virtual ~vtkTableBasedClipperShapeList();
  virtual int GetVTKType() const = 0;
  int GetShapeSize() const { return shapeSize; }
  int GetTotalNumberOfShapes() const;
  int GetNumberOfLists() const;
  int GetList(vtkIdType, const vtkIdType*&) const;

protected:
  vtkIdType** list;
  int currentList;
  int currentShape;
  int listSize;
  int shapesPerList;
  int shapeSize;
};

class vtkTableBasedClipperHexList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperHexList();
  ~vtkTableBasedClipperHexList() override;
  int GetVTKType() const override { return VTK_HEXAHEDRON; }
  void AddHex(vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType,
    vtkIdType, vtkIdType);
};

class vtkTableBasedClipperWedgeList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperWedgeList();
  ~vtkTableBasedClipperWedgeList() override;
  int GetVTKType() const override { return VTK_WEDGE; }
  void AddWedge(vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType);
};

class vtkTableBasedClipperPyramidList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperPyramidList();
  ~vtkTableBasedClipperPyramidList() override;
  int GetVTKType() const override { return VTK_PYRAMID; }
  void AddPyramid(vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType);
};

class vtkTableBasedClipperTetList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperTetList();
  ~vtkTableBasedClipperTetList() override;
  int GetVTKType() const override { return VTK_TETRA; }
  void AddTet(vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType);
};

class vtkTableBasedClipperQuadList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperQuadList();
  ~vtkTableBasedClipperQuadList() override;
  int GetVTKType() const override { return VTK_QUAD; }
  void AddQuad(vtkIdType, vtkIdType, vtkIdType, vtkIdType, vtkIdType);
};

class vtkTableBasedClipperTriList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperTriList();
  ~vtkTableBasedClipperTriList() override;
  int GetVTKType() const override { return VTK_TRIANGLE; }
  void AddTri(vtkIdType, vtkIdType, vtkIdType, vtkIdType);
};

class vtkTableBasedClipperLineList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperLineList();
  ~vtkTableBasedClipperLineList() override;
  int GetVTKType() const override { return VTK_LINE; }
  void AddLine(vtkIdType, vtkIdType, vtkIdType);
};

class vtkTableBasedClipperVertexList : public vtkTableBasedClipperShapeList
{
public:
  vtkTableBasedClipperVertexList();
  ~vtkTableBasedClipperVertexList() override;
  int GetVTKType() const override { return VTK_VERTEX; }
  void AddVertex(vtkIdType, vtkIdType);
};

struct TableBasedClipperCentroidPointEntry
{
  vtkIdType nPts;
  int ptIds[8];
};

class vtkTableBasedClipperCentroidPointList
{
public:
  vtkTableBasedClipperCentroidPointList();
  virtual ~vtkTableBasedClipperCentroidPointList();

  vtkIdType AddPoint(vtkIdType, vtkIdType*);

  vtkIdType GetTotalNumberOfPoints() const;
  int GetNumberOfLists() const;
  int GetList(vtkIdType, const TableBasedClipperCentroidPointEntry*&) const;

protected:
  TableBasedClipperCentroidPointEntry** list;
  int currentList;
  int currentPoint;
  int listSize;
  int pointsPerList;
};

struct TableBasedClipperCommonPointsStructure
{
  bool hasPtsList;
  double* pts_ptr;
  int* dims;
  double* X;
  double* Y;
  double* Z;
};

class vtkTableBasedClipperVolumeFromVolume : public vtkTableBasedClipperDataSetFromVolume
{
  friend class detail::ConstructSimpleDataSetsFunctor;

public:
  vtkTableBasedClipperVolumeFromVolume(int precision, vtkIdType nPts, vtkIdType ptSizeGuess);
  ~vtkTableBasedClipperVolumeFromVolume() override = default;

  void ConstructDataSet(vtkDataSet*, vtkUnstructuredGrid*, double*);
  void ConstructDataSet(vtkDataSet*, vtkUnstructuredGrid*, int*, double*, double*, double*);

  vtkIdType AddCentroidPoint(int n, vtkIdType* p)
  {
    return -1 - centroid_list.AddPoint(static_cast<vtkIdType>(n), p);
  }

  void AddHex(vtkIdType z, vtkIdType v0, vtkIdType v1, vtkIdType v2, vtkIdType v3, vtkIdType v4,
    vtkIdType v5, vtkIdType v6, vtkIdType v7)
  {
    this->hexes.AddHex(z, v0, v1, v2, v3, v4, v5, v6, v7);
  }

  void AddWedge(
    vtkIdType z, vtkIdType v0, vtkIdType v1, vtkIdType v2, vtkIdType v3, vtkIdType v4, vtkIdType v5)
  {
    this->wedges.AddWedge(z, v0, v1, v2, v3, v4, v5);
  }
  void AddPyramid(vtkIdType z, vtkIdType v0, vtkIdType v1, vtkIdType v2, vtkIdType v3, vtkIdType v4)
  {
    this->pyramids.AddPyramid(z, v0, v1, v2, v3, v4);
  }
  void AddTet(vtkIdType z, vtkIdType v0, vtkIdType v1, vtkIdType v2, vtkIdType v3)
  {
    this->tets.AddTet(z, v0, v1, v2, v3);
  }
  void AddQuad(vtkIdType z, vtkIdType v0, vtkIdType v1, vtkIdType v2, vtkIdType v3)
  {
    this->quads.AddQuad(z, v0, v1, v2, v3);
  }
  void AddTri(vtkIdType z, vtkIdType v0, vtkIdType v1, vtkIdType v2)
  {
    this->tris.AddTri(z, v0, v1, v2);
  }
  void AddLine(vtkIdType z, vtkIdType v0, vtkIdType v1) { this->lines.AddLine(z, v0, v1); }
  void AddVertex(vtkIdType z, vtkIdType v0) { this->vertices.AddVertex(z, v0); }

protected:
  vtkTableBasedClipperCentroidPointList centroid_list;
  vtkTableBasedClipperHexList hexes;
  vtkTableBasedClipperWedgeList wedges;
  vtkTableBasedClipperPyramidList pyramids;
  vtkTableBasedClipperTetList tets;
  vtkTableBasedClipperQuadList quads;
  vtkTableBasedClipperTriList tris;
  vtkTableBasedClipperLineList lines;
  vtkTableBasedClipperVertexList vertices;

  std::array<vtkTableBasedClipperShapeList*, 8> shapes;
  int OutputPointsPrecision;

  void ConstructDataSet(vtkDataSet*, vtkUnstructuredGrid*, TableBasedClipperCommonPointsStructure&);
};

vtkTableBasedClipperVolumeFromVolume::vtkTableBasedClipperVolumeFromVolume(
  int precision, vtkIdType nPts, vtkIdType ptSizeGuess)
  : vtkTableBasedClipperDataSetFromVolume(nPts, ptSizeGuess)
  , OutputPointsPrecision(precision)
{
  shapes[0] = &tets;
  shapes[1] = &pyramids;
  shapes[2] = &wedges;
  shapes[3] = &hexes;
  shapes[4] = &quads;
  shapes[5] = &tris;
  shapes[6] = &lines;
  shapes[7] = &vertices;
}

vtkTableBasedClipperCentroidPointList::vtkTableBasedClipperCentroidPointList()
{
  listSize = 4096;
  pointsPerList = 1024;

  list = new TableBasedClipperCentroidPointEntry*[listSize];
  list[0] = new TableBasedClipperCentroidPointEntry[pointsPerList];
  for (int i = 1; i < listSize; i++)
  {
    list[i] = nullptr;
  }

  currentList = 0;
  currentPoint = 0;
}

vtkTableBasedClipperCentroidPointList::~vtkTableBasedClipperCentroidPointList()
{
  for (int i = 0; i < listSize; i++)
  {
    if (list[i] != nullptr)
    {
      delete[] list[i];
    }
    else
    {
      break;
    }
  }

  delete[] list;
}

int vtkTableBasedClipperCentroidPointList::GetList(
  vtkIdType listId, const TableBasedClipperCentroidPointEntry*& outlist) const
{
  if (listId < 0 || listId > currentList)
  {
    outlist = nullptr;
    return 0;
  }

  outlist = list[listId];
  return (listId == currentList ? currentPoint : pointsPerList);
}

int vtkTableBasedClipperCentroidPointList::GetNumberOfLists() const
{
  return currentList + 1;
}

vtkIdType vtkTableBasedClipperCentroidPointList::GetTotalNumberOfPoints() const
{
  vtkIdType numFullLists = static_cast<vtkIdType>(currentList); // actually currentList-1+1
  vtkIdType numExtra = static_cast<vtkIdType>(currentPoint);    // again, currentPoint-1+1

  return numFullLists * pointsPerList + numExtra;
}

vtkIdType vtkTableBasedClipperCentroidPointList::AddPoint(vtkIdType npts, vtkIdType* pts)
{
  if (currentPoint >= pointsPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      TableBasedClipperCentroidPointEntry** tmpList =
        new TableBasedClipperCentroidPointEntry*[2 * listSize];

      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new TableBasedClipperCentroidPointEntry[pointsPerList];
    currentPoint = 0;
  }

  list[currentList][currentPoint].nPts = npts;
  for (int i = 0; i < npts; i++)
  {
    list[currentList][currentPoint].ptIds[i] = pts[i];
  }
  currentPoint++;

  return (GetTotalNumberOfPoints() - 1);
}

vtkTableBasedClipperShapeList::vtkTableBasedClipperShapeList(int size)
{
  shapeSize = size;
  listSize = 4096;
  shapesPerList = 1024;

  list = new vtkIdType*[listSize];
  list[0] = new vtkIdType[(shapeSize + 1) * shapesPerList];

  for (int i = 1; i < listSize; i++)
  {
    list[i] = nullptr;
  }

  currentList = 0;
  currentShape = 0;
}

vtkTableBasedClipperShapeList::~vtkTableBasedClipperShapeList()
{
  for (int i = 0; i < listSize; i++)
  {
    if (list[i] != nullptr)
    {
      delete[] list[i];
    }
    else
    {
      break;
    }
  }

  delete[] list;
}

int vtkTableBasedClipperShapeList::GetList(vtkIdType listId, const vtkIdType*& outlist) const
{
  if (listId < 0 || listId > currentList)
  {
    outlist = nullptr;
    return 0;
  }

  outlist = list[listId];
  return (listId == currentList ? currentShape : shapesPerList);
}

int vtkTableBasedClipperShapeList::GetNumberOfLists() const
{
  return currentList + 1;
}

int vtkTableBasedClipperShapeList::GetTotalNumberOfShapes() const
{
  int numFullLists = currentList; // actually currentList-1+1
  int numExtra = currentShape;    // again, currentShape-1+1

  return numFullLists * shapesPerList + numExtra;
}

vtkTableBasedClipperHexList::vtkTableBasedClipperHexList()
  : vtkTableBasedClipperShapeList(8)
{
}

vtkTableBasedClipperHexList::~vtkTableBasedClipperHexList() = default;

void vtkTableBasedClipperHexList::AddHex(vtkIdType cellId, vtkIdType v1, vtkIdType v2, vtkIdType v3,
  vtkIdType v4, vtkIdType v5, vtkIdType v6, vtkIdType v7, vtkIdType v8)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];

      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  list[currentList][idx + 3] = v3;
  list[currentList][idx + 4] = v4;
  list[currentList][idx + 5] = v5;
  list[currentList][idx + 6] = v6;
  list[currentList][idx + 7] = v7;
  list[currentList][idx + 8] = v8;
  currentShape++;
}

vtkTableBasedClipperWedgeList::vtkTableBasedClipperWedgeList()
  : vtkTableBasedClipperShapeList(6)
{
}

vtkTableBasedClipperWedgeList::~vtkTableBasedClipperWedgeList() = default;

void vtkTableBasedClipperWedgeList::AddWedge(vtkIdType cellId, vtkIdType v1, vtkIdType v2,
  vtkIdType v3, vtkIdType v4, vtkIdType v5, vtkIdType v6)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];
      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  list[currentList][idx + 3] = v3;
  list[currentList][idx + 4] = v4;
  list[currentList][idx + 5] = v5;
  list[currentList][idx + 6] = v6;
  currentShape++;
}

vtkTableBasedClipperPyramidList::vtkTableBasedClipperPyramidList()
  : vtkTableBasedClipperShapeList(5)
{
}

vtkTableBasedClipperPyramidList::~vtkTableBasedClipperPyramidList() = default;

void vtkTableBasedClipperPyramidList::AddPyramid(
  vtkIdType cellId, vtkIdType v1, vtkIdType v2, vtkIdType v3, vtkIdType v4, vtkIdType v5)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];

      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  list[currentList][idx + 3] = v3;
  list[currentList][idx + 4] = v4;
  list[currentList][idx + 5] = v5;
  currentShape++;
}

vtkTableBasedClipperTetList::vtkTableBasedClipperTetList()
  : vtkTableBasedClipperShapeList(4)
{
}

vtkTableBasedClipperTetList::~vtkTableBasedClipperTetList() = default;

void vtkTableBasedClipperTetList::AddTet(
  vtkIdType cellId, vtkIdType v1, vtkIdType v2, vtkIdType v3, vtkIdType v4)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];

      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  list[currentList][idx + 3] = v3;
  list[currentList][idx + 4] = v4;
  currentShape++;
}

vtkTableBasedClipperQuadList::vtkTableBasedClipperQuadList()
  : vtkTableBasedClipperShapeList(4)
{
}

vtkTableBasedClipperQuadList::~vtkTableBasedClipperQuadList() = default;

void vtkTableBasedClipperQuadList::AddQuad(
  vtkIdType cellId, vtkIdType v1, vtkIdType v2, vtkIdType v3, vtkIdType v4)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];
      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  list[currentList][idx + 3] = v3;
  list[currentList][idx + 4] = v4;
  currentShape++;
}

vtkTableBasedClipperTriList::vtkTableBasedClipperTriList()
  : vtkTableBasedClipperShapeList(3)
{
}

vtkTableBasedClipperTriList::~vtkTableBasedClipperTriList() = default;

void vtkTableBasedClipperTriList::AddTri(vtkIdType cellId, vtkIdType v1, vtkIdType v2, vtkIdType v3)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];
      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  list[currentList][idx + 3] = v3;
  currentShape++;
}

vtkTableBasedClipperLineList::vtkTableBasedClipperLineList()
  : vtkTableBasedClipperShapeList(2)
{
}

vtkTableBasedClipperLineList::~vtkTableBasedClipperLineList() = default;

void vtkTableBasedClipperLineList::AddLine(vtkIdType cellId, vtkIdType v1, vtkIdType v2)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];
      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  list[currentList][idx + 2] = v2;
  currentShape++;
}

vtkTableBasedClipperVertexList::vtkTableBasedClipperVertexList()
  : vtkTableBasedClipperShapeList(1)
{
}

vtkTableBasedClipperVertexList::~vtkTableBasedClipperVertexList() = default;

void vtkTableBasedClipperVertexList::AddVertex(vtkIdType cellId, vtkIdType v1)
{
  if (currentShape >= shapesPerList)
  {
    if ((currentList + 1) >= listSize)
    {
      vtkIdType** tmpList = new vtkIdType*[2 * listSize];
      for (int i = 0; i < listSize; i++)
      {
        tmpList[i] = list[i];
      }

      for (int i = listSize; i < listSize * 2; i++)
      {
        tmpList[i] = nullptr;
      }

      listSize *= 2;
      delete[] list;
      list = tmpList;
    }

    currentList++;
    list[currentList] = new vtkIdType[(shapeSize + 1) * shapesPerList];
    currentShape = 0;
  }

  int idx = (shapeSize + 1) * currentShape;
  list[currentList][idx + 0] = cellId;
  list[currentList][idx + 1] = v1;
  currentShape++;
}

void vtkTableBasedClipperVolumeFromVolume::ConstructDataSet(
  vtkDataSet* input, vtkUnstructuredGrid* output, double* pts_ptr)
{
  TableBasedClipperCommonPointsStructure cps;
  cps.hasPtsList = true;
  cps.pts_ptr = pts_ptr;
  ConstructDataSet(input, output, cps);
}

void vtkTableBasedClipperVolumeFromVolume::ConstructDataSet(
  vtkDataSet* input, vtkUnstructuredGrid* output, int* dims, double* X, double* Y, double* Z)
{
  TableBasedClipperCommonPointsStructure cps;
  cps.hasPtsList = false;
  cps.dims = dims;
  cps.X = X;
  cps.Y = Y;
  cps.Z = Z;
  ConstructDataSet(input, output, cps);
}

void vtkTableBasedClipperVolumeFromVolume::ConstructDataSet(
  vtkDataSet* input, vtkUnstructuredGrid* output, TableBasedClipperCommonPointsStructure& cps)
{
  int i, j, k, l;

  vtkPointData* inPD = input->GetPointData();
  vtkCellData* inCD = input->GetCellData();

  vtkPointData* outPD = output->GetPointData();
  vtkCellData* outCD = output->GetCellData();

  vtkSmartPointer<vtkIntArray> newOrigNodes;
  vtkIntArray* origNodes = vtkArrayDownCast<vtkIntArray>(inPD->GetArray("avtOriginalNodeNumbers"));

  // If the isovolume only affects a small part of the dataset, we can save
  // on memory by only bringing over the points from the original dataset
  // that are used with the output.  Determine which points those are here.
  std::vector<int> ptLookup(numPrevPts, -1);

  int numUsed = 0;
  for (i = 0; i < shapes.size(); i++)
  {
    int nlists = shapes[i]->GetNumberOfLists();
    int npts_per_shape = shapes[i]->GetShapeSize();

    for (j = 0; j < nlists; j++)
    {
      const vtkIdType* list;
      int listSize = shapes[i]->GetList(j, list);

      for (k = 0; k < listSize; k++)
      {
        list++; // skip the cell id entry

        for (l = 0; l < npts_per_shape; l++)
        {
          int pt = *list;
          list++;

          if (pt >= 0 && pt < numPrevPts)
          {
            if (ptLookup[pt] == -1)
            {
              ptLookup[pt] = numUsed++;
            }
          }
        }
      }
    }
  }

  // Set up the output points and its point data.
  vtkNew<vtkPoints> outPts;

  // set precision for the points in the output
  if (this->OutputPointsPrecision == vtkAlgorithm::DEFAULT_PRECISION)
  {
    vtkPointSet* inputPointSet = vtkPointSet::SafeDownCast(input);
    if (inputPointSet)
    {
      outPts->SetDataType(inputPointSet->GetPoints()->GetDataType());
    }
    else
    {
      outPts->SetDataType(VTK_FLOAT);
    }
  }
  else if (this->OutputPointsPrecision == vtkAlgorithm::SINGLE_PRECISION)
  {
    outPts->SetDataType(VTK_FLOAT);
  }
  else if (this->OutputPointsPrecision == vtkAlgorithm::DOUBLE_PRECISION)
  {
    outPts->SetDataType(VTK_DOUBLE);
  }

  vtkIdType centroidStart = numUsed + pt_list.GetTotalNumberOfPoints();
  vtkIdType nOutPts = centroidStart + centroid_list.GetTotalNumberOfPoints();
  outPts->SetNumberOfPoints(nOutPts);
  outPD->CopyAllocate(inPD, nOutPts);

  std::vector<double> tuple;
  if (origNodes != nullptr)
  {
    tuple.resize(origNodes->GetNumberOfComponents());
    newOrigNodes = vtkSmartPointer<vtkIntArray>::New();
    newOrigNodes->SetNumberOfComponents(origNodes->GetNumberOfComponents());
    newOrigNodes->SetNumberOfTuples(nOutPts);
    newOrigNodes->SetName(origNodes->GetName());
  }

  // Copy over all the points from the input that are actually used in the
  // output.
  for (i = 0; i < numPrevPts; i++)
  {
    if (ptLookup[i] == -1)
    {
      continue;
    }

    if (cps.hasPtsList)
    {
      outPts->SetPoint(ptLookup[i], cps.pts_ptr + 3 * i);
    }
    else
    {
      int I = i % cps.dims[0];
      int J = (i / cps.dims[0]) % cps.dims[1];
      int K = i / (cps.dims[0] * cps.dims[1]);
      outPts->SetPoint(ptLookup[i], cps.X[I], cps.Y[J], cps.Z[K]);
    }

    outPD->CopyData(inPD, i, ptLookup[i]);
    if (newOrigNodes)
    {
      origNodes->GetTuple(i, tuple.data());
      newOrigNodes->SetTuple(ptLookup[i], tuple.data());
    }
  }

  vtkIdType pointId = numUsed;

  // Now construct all the points that are along edges and new and add
  // them to the points list.
  int nLists = pt_list.GetNumberOfLists();
  for (i = 0; i < nLists; i++)
  {
    const TableBasedClipperPointEntry* pe_list = nullptr;
    int nPts = pt_list.GetList(i, pe_list);
    for (j = 0; j < nPts; j++)
    {
      const TableBasedClipperPointEntry& pe = pe_list[j];
      double pt[3];
      const int& idx1 = pe.ptIds[0];
      const int& idx2 = pe.ptIds[1];

      // Construct the original points -- this will depend on whether
      // we started with a rectilinear grid or a point set.
      double* pt1 = nullptr;
      double* pt2 = nullptr;
      double pt1_storage[3];
      double pt2_storage[3];
      if (cps.hasPtsList)
      {
        pt1 = cps.pts_ptr + 3 * idx1;
        pt2 = cps.pts_ptr + 3 * idx2;
      }
      else
      {
        pt1 = pt1_storage;
        pt2 = pt2_storage;
        int I = idx1 % cps.dims[0];
        int J = (idx1 / cps.dims[0]) % cps.dims[1];
        int K = idx1 / (cps.dims[0] * cps.dims[1]);
        pt1[0] = cps.X[I];
        pt1[1] = cps.Y[J];
        pt1[2] = cps.Z[K];
        I = idx2 % cps.dims[0];
        J = (idx2 / cps.dims[0]) % cps.dims[1];
        K = idx2 / (cps.dims[0] * cps.dims[1]);
        pt2[0] = cps.X[I];
        pt2[1] = cps.Y[J];
        pt2[2] = cps.Z[K];
      }

      // Now that we have the original points, calculate the new one.
      double p = pe.percent;
      double bp = 1.0 - p;
      pt[0] = pt1[0] * p + pt2[0] * bp;
      pt[1] = pt1[1] * p + pt2[1] * bp;
      pt[2] = pt1[2] * p + pt2[2] * bp;
      outPts->SetPoint(pointId, pt);
      outPD->InterpolateEdge(inPD, pointId, pe.ptIds[0], pe.ptIds[1], bp);

      if (newOrigNodes)
      {
        int id = (bp <= 0.5 ? pe.ptIds[0] : pe.ptIds[1]);
        origNodes->GetTuple(id, tuple.data());
        newOrigNodes->SetTuple(pointId, tuple.data());
      }
      pointId++;
    }
  }

  //
  // Now construct the new "centroid" points and add them to the points list.
  //
  nLists = centroid_list.GetNumberOfLists();
  vtkNew<vtkIdList> idList;
  for (i = 0; i < nLists; i++)
  {
    const TableBasedClipperCentroidPointEntry* ce_list = nullptr;
    int nPts = centroid_list.GetList(i, ce_list);
    for (j = 0; j < nPts; j++)
    {
      const TableBasedClipperCentroidPointEntry& ce = ce_list[j];
      idList->SetNumberOfIds(ce.nPts);
      double pts[8][3];
      double weights[8];
      double pt[3] = { 0.0, 0.0, 0.0 };
      double weight_factor = 1.0 / ce.nPts;
      for (k = 0; k < ce.nPts; k++)
      {
        weights[k] = 1.0 * weight_factor;
        vtkIdType id = 0;

        if (ce.ptIds[k] < 0) // this is an id of an inserted centroid point
        {
          id = centroidStart - 1 - ce.ptIds[k];
        }
        else if (ce.ptIds[k] >= numPrevPts) // this is an id of an inserted edge point
        {
          id = numUsed + (ce.ptIds[k] - numPrevPts);
        }
        else // this is an id of a point from the input
        {
          id = ptLookup[ce.ptIds[k]];
        }

        idList->SetId(k, id);
        outPts->GetPoint(id, pts[k]);
        pt[0] += pts[k][0];
        pt[1] += pts[k][1];
        pt[2] += pts[k][2];
      }
      pt[0] *= weight_factor;
      pt[1] *= weight_factor;
      pt[2] *= weight_factor;

      outPts->SetPoint(pointId, pt);
      outPD->InterpolatePoint(outPD, pointId, idList, weights);
      if (newOrigNodes)
      {
        // these 'created' nodes have no original designation
        for (int z = 0; z < newOrigNodes->GetNumberOfComponents(); z++)
        {
          newOrigNodes->SetComponent(pointId, z, -1);
        }
      }
      pointId++;
    }
  }

  // We are finally done constructing the points list.  Set it with our
  // output and clean up memory.
  output->SetPoints(outPts);

  if (newOrigNodes)
  {
    // AddArray will overwrite an already existing array with
    // the same name, exactly what we want here.
    outPD->AddArray(newOrigNodes);
  }

  // Now set up the shapes and the cell data.
  vtkIdType ncells = 0;
  vtkIdType connectivitySize = 0;
  for (i = 0; i < shapes.size(); i++)
  {
    vtkIdType ns = shapes[i]->GetTotalNumberOfShapes();
    ncells += ns;
    connectivitySize += static_cast<vtkIdType>(shapes[i]->GetShapeSize()) * ns;
  }
  vtkIdType offsetsSize = ncells + 1;

  outCD->CopyAllocate(inCD, ncells);

  vtkNew<vtkUnsignedCharArray> cellTypes;
  cellTypes->SetNumberOfValues(ncells);

  vtkNew<vtkIdTypeArray> offsets;
  offsets->SetNumberOfValues(offsetsSize);

  vtkNew<vtkIdTypeArray> connectivity;
  connectivity->SetNumberOfValues(connectivitySize);

  vtkIdType ids[8]; // 8 (for hex)
  int nlists;
  vtkIdType cellId = 0;
  vtkIdType offsetValue = 0;
  for (i = 0; i < shapes.size(); i++)
  {
    const vtkIdType* list;
    nlists = shapes[i]->GetNumberOfLists();
    int shapeSize = shapes[i]->GetShapeSize();
    auto vtkType = static_cast<unsigned char>(shapes[i]->GetVTKType());

    for (j = 0; j < nlists; j++)
    {
      int listSize = shapes[i]->GetList(j, list);

      for (k = 0; k < listSize; k++)
      {
        outCD->CopyData(inCD, list[0], cellId);

        for (l = 0; l < shapeSize; l++)
        {
          if (list[l + 1] < 0)
          {
            ids[l] = centroidStart - 1 - list[l + 1];
          }
          else if (list[l + 1] >= numPrevPts)
          {
            ids[l] = numUsed + (list[l + 1] - numPrevPts);
          }
          else
          {
            ids[l] = ptLookup[list[l + 1]];
          }
        }
        list += shapeSize + 1;
        cellTypes->SetValue(cellId, vtkType);
        offsets->SetValue(cellId, offsetValue);
        for (l = 0; l < shapeSize; l++)
        {
          connectivity->SetValue(offsetValue + l, ids[l]);
        }
        cellId++;
        offsetValue += shapeSize;
      }
    }
  }
  // assign the last offset
  offsets->SetValue(cellId, offsetValue);

  vtkNew<vtkCellArray> cells;
  cells->SetData(offsets, connectivity);
  // set nullptr for faces since there will be, for sure, no polyhedrons.
  output->SetCells(cellTypes, cells, nullptr, nullptr);
}

inline void GetPoint(
  double* pt, const double* X, const double* Y, const double* Z, const int* dims, const int& index)
{
  int cellI = index % dims[0];
  int cellJ = (index / dims[0]) % dims[1];
  int cellK = index / (dims[0] * dims[1]);
  pt[0] = X[cellI];
  pt[1] = Y[cellJ];
  pt[2] = Z[cellK];
}
// ============================================================================
// =============== vtkTableBasedClipperVolumeFromVolume ( end ) ===============
// ============================================================================

namespace detail
{
// struct to collect all thread-local results into one object to ensure correct ordering
struct LocalDataType
{
  // structure to save whatever can be clipped
  vtkTableBasedClipperVolumeFromVolume* VisItVFV;
  // the cells that can not be clipped by this filter
  vtkIdType NumberOfSpecialCells;
  vtkSmartPointer<vtkUnstructuredGrid> SpecialCells;

  // the structures that can be merged
  vtkSmartPointer<vtkUnstructuredGrid> SimpleClippedCells;
  vtkSmartPointer<vtkUnstructuredGrid> SpecialClippedCells;
};

using ThreadLocalDataType = std::vector<vtkSMPThreadLocal<LocalDataType>::iterator>;

// Base functor used for vtkPolyData and vtkUnstructuredGrid
template <typename TGrid>
class ClipUnstructuredDataFunctor
{
protected:
  TGrid* Input;
  vtkDataArray* ClipArray;
  double IsoValue;
  vtkIdType NumberOfPoints;
  vtkIdType NumberOfCells;
  int OutputPointsPrecision;
  vtkTypeBool InsideOut;
  // it needs to be in double because it's an approximation
  double NumberOfThreadCells;

  // structure to save whatever can be clipped
  vtkSMPThreadLocal<LocalDataType> TLDataType;
  using EDGEIDXS = const int[2];

public:
  // constructor when TGrid == vtkUnstructuredGrid
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkUnstructuredGrid>::value>::type>
  ClipUnstructuredDataFunctor(TGrid* input, vtkDataArray* clipArray, double isoValue,
    int outputPointsPrecision, vtkTypeBool insideOut)
    : Input(input)
    , ClipArray(clipArray)
    , IsoValue(isoValue)
    , NumberOfPoints(Input->GetNumberOfPoints())
    , NumberOfCells(Input->GetNumberOfCells())
    , OutputPointsPrecision(outputPointsPrecision)
    , InsideOut(insideOut)
  {
    // estimated amount of extra edge points needed per thread
    int numberOfThreads = vtkSMPTools::GetEstimatedNumberOfThreads();
    this->NumberOfThreadCells = (this->NumberOfCells / (1.0f * numberOfThreads)) + numberOfThreads;
  }

  // constructor when TGrid == vtkPolyData
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkPolyData>::value>::type, typename = T>
  ClipUnstructuredDataFunctor(TGrid* input, vtkDataArray* clipArray, double isoValue,
    int outputPointsPrecision, vtkTypeBool insideOut)
    : Input(input)
    , ClipArray(clipArray)
    , IsoValue(isoValue)
    , NumberOfPoints(Input->GetNumberOfPoints())
    , NumberOfCells(Input->GetNumberOfCells())
    , OutputPointsPrecision(outputPointsPrecision)
    , InsideOut(insideOut)
  {
    // estimated amount of extra edge points needed per thread
    int numberOfThreads = vtkSMPTools::GetEstimatedNumberOfThreads();
    this->NumberOfThreadCells = (this->NumberOfCells / (1.0f * numberOfThreads)) + numberOfThreads;

    // Ensure build cells before calling GetCellPoints in parallel
    if (this->Input->NeedToBuildCells())
    {
      this->Input->BuildCells();
    }
  }

  void Initialize()
  {
    // volume from volume structure
    auto& visItVFV = this->TLDataType.Local().VisItVFV;
    visItVFV =
      new vtkTableBasedClipperVolumeFromVolume(this->OutputPointsPrecision, this->NumberOfPoints,
        static_cast<vtkIdType>(std::pow(this->NumberOfThreadCells, double(0.6667f))) * 5 + 100);

    // initialize number of special cells
    auto& numberOfSpecialCells = this->TLDataType.Local().NumberOfSpecialCells;
    numberOfSpecialCells = 0;

    // initialize special cells
    auto& specialCells = this->TLDataType.Local().SpecialCells;
    specialCells = vtkSmartPointer<vtkUnstructuredGrid>::New();
    specialCells->SetPoints(this->Input->GetPoints());
    specialCells->GetPointData()->ShallowCopy(this->Input->GetPointData());
  }

  void operator()(vtkIdType beginCellId, vtkIdType endCellId)
  {
    auto& visItVFV = this->TLDataType.Local().VisItVFV;
    auto& numberOfSpecialCells = this->TLDataType.Local().NumberOfSpecialCells;
    auto& specialCells = this->TLDataType.Local().SpecialCells;
    int cellType;
    vtkIdType numPoints = 0;
    bool cellCanBeClippedFast;

    for (vtkIdType cellId = beginCellId; cellId < endCellId; ++cellId)
    {
      cellType = this->Input->GetCellType(cellId);
      const vtkIdType* pointIndices = nullptr;
      this->Input->GetCellPoints(cellId, numPoints, pointIndices);

      // identify if cell can be clipped fast
      cellCanBeClippedFast = false;
      switch (cellType)
      {
        case VTK_HEXAHEDRON:
        case VTK_WEDGE:
        case VTK_PYRAMID:
        case VTK_TETRA:
        case VTK_VOXEL:
        case VTK_PIXEL:
        case VTK_QUAD:
        case VTK_TRIANGLE:
        case VTK_LINE:
        case VTK_VERTEX:
          cellCanBeClippedFast = true;
          break;

        default:
          cellCanBeClippedFast = false;
          break;
      }

      if (cellCanBeClippedFast)
      {
        int caseIndex = 0;
        double grdDiffs[8];

        for (vtkIdType j = numPoints - 1; j >= 0; j--)
        {
          grdDiffs[j] = this->ClipArray->GetComponent(pointIndices[j], 0) - this->IsoValue;
          caseIndex += ((grdDiffs[j] >= 0.0) ? 1 : 0);
          caseIndex <<= (1 - (!j));
        }

        int startIdx = 0;
        int numberOfOutputPoints = 0;
        EDGEIDXS* edgeVertices = nullptr;
        unsigned char* thisCase = nullptr;

        // start index, split case, number of output, and vertices from edges
        switch (cellType)
        {
          // Volume 3D cells
          case VTK_HEXAHEDRON:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesHex[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesHex[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesHex[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::HexVerticesFromEdges;
            break;

          case VTK_WEDGE:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesWdg[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesWdg[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesWdg[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::WedgeVerticesFromEdges;
            break;

          case VTK_PYRAMID:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesPyr[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesPyr[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesPyr[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::PyramidVerticesFromEdges;
            break;

          case VTK_TETRA:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesTet[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesTet[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesTet[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::TetVerticesFromEdges;
            break;

          case VTK_VOXEL:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesVox[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesVox[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesVox[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::VoxVerticesFromEdges;
            break;

          case VTK_PIXEL:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesPix[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesPix[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesPix[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::PixelVerticesFromEdges;
            break;

          // Polygonal 3D cells
          case VTK_QUAD:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesQua[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesQua[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesQua[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::QuadVerticesFromEdges;
            break;

          case VTK_TRIANGLE:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesTri[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesTri[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesTri[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::TriVerticesFromEdges;
            break;

          case VTK_LINE:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesLin[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesLin[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesLin[caseIndex];
            edgeVertices = vtkTableBasedClipperTriangulationTables::LineVerticesFromEdges;
            break;

          case VTK_VERTEX:
            startIdx = vtkTableBasedClipperClipTables::StartClipShapesVtx[caseIndex];
            thisCase = &vtkTableBasedClipperClipTables::ClipShapesVtx[startIdx];
            numberOfOutputPoints = vtkTableBasedClipperClipTables::NumClipShapesVtx[caseIndex];
            edgeVertices = nullptr;
            break;
        }

        vtkIdType intrpIds[4];
        for (int j = 0; j < numberOfOutputPoints; j++)
        {
          int nCellPts = 0;
          int theColor = -1;
          int intrpIdx = -1;
          unsigned char theShape = *thisCase++;

          // number of points and color
          switch (theShape)
          {
            case ST_HEX:
              nCellPts = 8;
              theColor = *thisCase++;
              break;

            case ST_WDG:
              nCellPts = 6;
              theColor = *thisCase++;
              break;

            case ST_PYR:
              nCellPts = 5;
              theColor = *thisCase++;
              break;

            case ST_TET:
              nCellPts = 4;
              theColor = *thisCase++;
              break;

            case ST_QUA:
              nCellPts = 4;
              theColor = *thisCase++;
              break;

            case ST_TRI:
              nCellPts = 3;
              theColor = *thisCase++;
              break;

            case ST_LIN:
              nCellPts = 2;
              theColor = *thisCase++;
              break;

            case ST_VTX:
              nCellPts = 1;
              theColor = *thisCase++;
              break;

            case ST_PNT:
              intrpIdx = *thisCase++;
              theColor = *thisCase++;
              nCellPts = *thisCase++;
              break;

            default:
              vtkLogF(ERROR, "An invalid output shape was found in the ClipCases.\n");
          }

          if ((!this->InsideOut && theColor == COLOR0) || (this->InsideOut && theColor == COLOR1))
          {
            // We don't want this one; it's the wrong side.
            thisCase += nCellPts;
            continue;
          }

          vtkIdType shapeIds[8]; // the maximum number of points is 8 for the hexahedron.
          for (int p = 0; p < nCellPts; p++)
          {
            unsigned char pntIndex = *thisCase++;

            if (pntIndex <= P7)
            {
              // We know pt P0 must be > P0 since we already
              // assume P0 == 0.  This is why we do not
              // bother subtracting P0 from pt here.
              shapeIds[p] = pointIndices[pntIndex];
            }
            else if (pntIndex >= EA && pntIndex <= EL)
            {
              int pt1Index = edgeVertices[pntIndex - EA][0];
              int pt2Index = edgeVertices[pntIndex - EA][1];
              if (pt2Index < pt1Index)
              {
                std::swap(pt1Index, pt2Index);
              }
              const double pt1ToPt2 = grdDiffs[pt2Index] - grdDiffs[pt1Index];
              const double pt1ToIso = 0.0 - grdDiffs[pt1Index];
              const double p1Weight = 1.0 - pt1ToIso / pt1ToPt2;

              const vtkIdType& pointId1 = pointIndices[pt1Index];
              const vtkIdType& pointId2 = pointIndices[pt2Index];

              shapeIds[p] = visItVFV->AddPoint(pointId1, pointId2, p1Weight);
            }
            else if (pntIndex >= N0 && pntIndex <= N3)
            {
              shapeIds[p] = intrpIds[pntIndex - N0];
            }
            else
            {
              vtkLogF(ERROR, "An invalid output point value was found in the ClipCases.\n");
            }
          }

          switch (theShape)
          {
            case ST_HEX:
              visItVFV->AddHex(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3],
                shapeIds[4], shapeIds[5], shapeIds[6], shapeIds[7]);
              break;

            case ST_WDG:
              visItVFV->AddWedge(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3],
                shapeIds[4], shapeIds[5]);
              break;

            case ST_PYR:
              visItVFV->AddPyramid(
                cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3], shapeIds[4]);
              break;

            case ST_TET:
              visItVFV->AddTet(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3]);
              break;

            case ST_QUA:
              visItVFV->AddQuad(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3]);
              break;

            case ST_TRI:
              visItVFV->AddTri(cellId, shapeIds[0], shapeIds[1], shapeIds[2]);
              break;

            case ST_LIN:
              visItVFV->AddLine(cellId, shapeIds[0], shapeIds[1]);
              break;

            case ST_VTX:
              visItVFV->AddVertex(cellId, shapeIds[0]);
              break;

            case ST_PNT:
              intrpIds[intrpIdx] = visItVFV->AddCentroidPoint(nCellPts, shapeIds);
              break;
          }
        }
        edgeVertices = nullptr;
        thisCase = nullptr;
      }
      else if (cellType == VTK_POLYHEDRON) // this is only used for vtkUnstructuredGrid
      {
        if (numberOfSpecialCells == 0)
        {
          specialCells->Allocate(static_cast<vtkIdType>(this->NumberOfThreadCells));
          specialCells->GetCellData()->CopyAllocate(
            this->Input->GetCellData(), static_cast<vtkIdType>(this->NumberOfThreadCells));
        }
        InsertPolyhedron(cellId, specialCells, numberOfSpecialCells);
      }
      else
      {
        if (numberOfSpecialCells == 0)
        {
          specialCells->Allocate(static_cast<vtkIdType>(this->NumberOfThreadCells));
          specialCells->GetCellData()->CopyAllocate(
            this->Input->GetCellData(), static_cast<vtkIdType>(this->NumberOfThreadCells));
        }
        specialCells->InsertNextCell(cellType, numPoints, pointIndices);
        specialCells->GetCellData()->CopyData(
          this->Input->GetCellData(), cellId, numberOfSpecialCells);
        numberOfSpecialCells++;
      }
      pointIndices = nullptr;
    }
  }

private:
  // InsertPolyhedron when TGrid == vtkUnstructuredGrid
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkUnstructuredGrid>::value>::type>
  void InsertPolyhedron(
    vtkIdType& cellId, vtkUnstructuredGrid* specialCells, vtkIdType& numberOfSpecialCells)
  {
    vtkIdType numberOfFaces;
    const vtkIdType* facePtIds;
    this->Input->GetFaceStream(cellId, numberOfFaces, facePtIds);
    specialCells->InsertNextCell(VTK_POLYHEDRON, numberOfFaces, facePtIds);
    specialCells->GetCellData()->CopyData(this->Input->GetCellData(), cellId, numberOfSpecialCells);
    numberOfSpecialCells++;
  }

  // InsertPolyhedron when TGrid == vtkPolyData
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkPolyData>::value>::type, typename = T>
  void InsertPolyhedron(vtkIdType&, vtkUnstructuredGrid*, vtkIdType&)
  {
  }

public:
  void Reduce() {}

  ThreadLocalDataType GetTLDataType()
  {
    ThreadLocalDataType tlDataType;
    for (auto tItr = this->TLDataType.begin(), tEnd = this->TLDataType.end(); tItr != tEnd; ++tItr)
    {
      tlDataType.push_back(tItr);
    }
    return tlDataType;
  }
};

// Base functor used for vtkRectilinearGridData and vtkStructuredGridData
template <typename TGrid>
class ClipStructuredDataFunctor
{
protected:
  TGrid* Input;
  vtkDataArray* ClipArray;
  double IsoValue;
  vtkIdType NumberOfPoints;
  vtkIdType NumberOfCells;
  int OutputPointsPrecision;
  vtkTypeBool InsideOut;

  enum TwoDimensionType
  {
    XY,
    YZ,
    XZ
  };
  TwoDimensionType TwoDimType;
  int IsTwoDim;

  std::array<std::array<int, 8>, 3> ShiftLUT;
  std::array<int, 3> CellDims;
  int CyStride;
  int CzStride;
  int PyStride;
  int PzStride;
  // it needs to be in double because it's an approximation
  double NumberOfThreadCells;

  // structure to save whatever can be clipped
  vtkSMPThreadLocal<LocalDataType> TLDataType;

public:
  ClipStructuredDataFunctor(TGrid* input, vtkDataArray* clipArray, double isoValue,
    int outputPointsPrecision, vtkTypeBool insideOut)
    : Input(input)
    , ClipArray(clipArray)
    , IsoValue(isoValue)
    , NumberOfPoints(Input->GetNumberOfPoints())
    , NumberOfCells(Input->GetNumberOfCells())
    , OutputPointsPrecision(outputPointsPrecision)
    , InsideOut(insideOut)
  {
    int gridDims[3];
    this->Input->GetDimensions(gridDims);
    this->IsTwoDim = int(gridDims[0] <= 1 || gridDims[1] <= 1 || gridDims[2] <= 1);
    if (gridDims[0] <= 1)
    {
      this->TwoDimType = YZ;
    }
    else if (gridDims[1] <= 1)
    {
      this->TwoDimType = XZ;
    }
    else
    {
      this->TwoDimType = XY;
    }

    const std::array<int, 8> shiftLUTx = { 0, 1, 1, 0, 0, 1, 1, 0 };
    const std::array<int, 8> shiftLUTy = { 0, 0, 1, 1, 0, 0, 1, 1 };
    const std::array<int, 8> shiftLUTz = { 0, 0, 0, 0, 1, 1, 1, 1 };

    if (this->IsTwoDim && this->TwoDimType == XZ)
    {
      this->ShiftLUT[0] = shiftLUTx;
      this->ShiftLUT[1] = shiftLUTz;
      this->ShiftLUT[2] = shiftLUTy;
    }
    else if (this->IsTwoDim && this->TwoDimType == YZ)
    {
      this->ShiftLUT[0] = shiftLUTy;
      this->ShiftLUT[1] = shiftLUTz;
      this->ShiftLUT[2] = shiftLUTx;
    }
    else
    {
      this->ShiftLUT[0] = shiftLUTx;
      this->ShiftLUT[1] = shiftLUTy;
      this->ShiftLUT[2] = shiftLUTz;
    }

    this->CellDims = { gridDims[0] - 1, gridDims[1] - 1, gridDims[2] - 1 };
    this->CyStride = (this->CellDims[0] ? this->CellDims[0] : 1);
    this->CzStride =
      (this->CellDims[0] ? this->CellDims[0] : 1) * (this->CellDims[1] ? this->CellDims[1] : 1);
    this->PyStride = gridDims[0];
    this->PzStride = gridDims[0] * gridDims[1];

    // estimated amount of extra edge points needed per thread
    int numberOfThreads = vtkSMPTools::GetEstimatedNumberOfThreads();
    this->NumberOfThreadCells = (this->NumberOfCells / (1.0f * numberOfThreads)) + numberOfThreads;
  }

  void Initialize()
  {
    // volume from volume structure
    auto& visItVFV = this->TLDataType.Local().VisItVFV;
    visItVFV =
      new vtkTableBasedClipperVolumeFromVolume(this->OutputPointsPrecision, this->NumberOfPoints,
        static_cast<vtkIdType>(std::pow(this->NumberOfThreadCells, double(0.6667f))) * 5 + 100);
  }

  // SetGridDiffsAndCaseIndex when TGrid == vtkRectilinearGrid
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkRectilinearGrid>::value>::type>
  void SetGridDiffsAndCaseIndex(std::array<double, 8>& grdDiffs, int& caseIndex,
    const int& theCellI, const int& theCellJ, const int& theCellK)
  {
    caseIndex = 0;
    const vtkIdType numberOfCellPoints = this->IsTwoDim ? 4 : 8;
    vtkIdType cellIndex;
    for (vtkIdType j = numberOfCellPoints - 1; j >= 0; j--)
    {
      cellIndex = (theCellK + this->ShiftLUT[2][j]) * this->PzStride +
        (theCellJ + this->ShiftLUT[1][j]) * this->PyStride + (theCellI + this->ShiftLUT[0][j]);

      grdDiffs[j] = this->ClipArray->GetComponent(cellIndex, 0) - this->IsoValue;
      caseIndex += ((grdDiffs[j] >= 0.0) ? 1 : 0);
      caseIndex <<= (1 - (!j));
    }
  }

  // SetGridDiffsAndCaseIndex when TGrid == vtkStructuredGrid
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkStructuredGrid>::value>::type,
    typename = T>
  void SetGridDiffsAndCaseIndex(std::array<double, 8>& grdDiffs, int& caseIndex,
    const int& theCellI, const int& theCellJ, const int& theCellK)
  {
    caseIndex = 0;
    const vtkIdType numberOfPoints = this->IsTwoDim ? 4 : 8;
    vtkIdType pointIndex;
    for (vtkIdType j = numberOfPoints - 1; j >= 0; j--)
    {
      pointIndex = (theCellI + this->ShiftLUT[0][j]) +
        (theCellJ + this->ShiftLUT[1][j]) * this->PyStride +
        (theCellK + this->ShiftLUT[2][j]) * this->PzStride;

      grdDiffs[j] = this->ClipArray->GetComponent(pointIndex, 0) - this->IsoValue;
      caseIndex += ((grdDiffs[j] >= 0.0) ? 1 : 0);
      caseIndex <<= (1 - (!j));
    }
  }

  void operator()(vtkIdType beginCellId, vtkIdType endCellId)
  {
    auto& visItVFV = this->TLDataType.Local().VisItVFV;
    vtkIdType numPoints = 0;

    int caseIndex;
    int theCellI;
    int theCellJ;
    int theCellK;
    for (vtkIdType cellId = beginCellId; cellId < endCellId; ++cellId)
    {
      theCellI = (this->CellDims[0] > 0 ? cellId % this->CellDims[0] : 0);
      theCellJ = (this->CellDims[1] > 0 ? (cellId / this->CyStride) % this->CellDims[1] : 0);
      theCellK = (this->CellDims[2] > 0 ? (cellId / this->CzStride) : 0);
      std::array<double, 8> grdDiffs;
      this->SetGridDiffsAndCaseIndex(grdDiffs, caseIndex, theCellI, theCellJ, theCellK);

      int nOutputs;
      vtkIdType intrpIds[4];
      unsigned char* thisCase = nullptr;

      if (this->IsTwoDim)
      {
        thisCase = &vtkTableBasedClipperClipTables::ClipShapesQua
                     [vtkTableBasedClipperClipTables::StartClipShapesQua[caseIndex]];
        nOutputs = vtkTableBasedClipperClipTables::NumClipShapesQua[caseIndex];
      }
      else
      {
        thisCase = &vtkTableBasedClipperClipTables::ClipShapesHex
                     [vtkTableBasedClipperClipTables::StartClipShapesHex[caseIndex]];
        nOutputs = vtkTableBasedClipperClipTables::NumClipShapesHex[caseIndex];
      }

      for (int j = 0; j < nOutputs; j++)
      {
        int nCellPts = 0;
        int intrpIdx = -1;
        int theColor = -1;
        unsigned char theShape = *thisCase++;

        nCellPts = 0;
        switch (theShape)
        {
          case ST_HEX:
            nCellPts = 8;
            theColor = *thisCase++;
            break;

          case ST_WDG:
            nCellPts = 6;
            theColor = *thisCase++;
            break;

          case ST_PYR:
            nCellPts = 5;
            theColor = *thisCase++;
            break;

          case ST_TET:
            nCellPts = 4;
            theColor = *thisCase++;
            break;

          case ST_QUA:
            nCellPts = 4;
            theColor = *thisCase++;
            break;

          case ST_TRI:
            nCellPts = 3;
            theColor = *thisCase++;
            break;

          case ST_LIN:
            nCellPts = 2;
            theColor = *thisCase++;
            break;

          case ST_VTX:
            nCellPts = 1;
            theColor = *thisCase++;
            break;

          case ST_PNT:
            intrpIdx = *thisCase++;
            theColor = *thisCase++;
            nCellPts = *thisCase++;
            break;

          default:
            vtkLogF(ERROR, "An invalid output shape was found in the ClipCases.\n");
        }

        if ((!this->InsideOut && theColor == COLOR0) || (this->InsideOut && theColor == COLOR1))
        {
          // We don't want this one; it's the wrong side.
          thisCase += nCellPts;
          continue;
        }

        vtkIdType shapeIds[8];
        for (int p = 0; p < nCellPts; p++)
        {
          unsigned char pntIndex = *thisCase++;

          if (pntIndex <= P7)
          {
            // We know pt P0 must be >P0 since we already
            // assume P0 == 0.  This is why we do not
            // bother subtracting P0 from pt here.
            shapeIds[p] = ((theCellI + this->ShiftLUT[0][pntIndex]) +
              (theCellJ + this->ShiftLUT[1][pntIndex]) * this->PyStride +
              (theCellK + this->ShiftLUT[2][pntIndex]) * this->PzStride);
          }
          else if (pntIndex >= EA && pntIndex <= EL)
          {
            int pt1Index =
              vtkTableBasedClipperTriangulationTables::HexVerticesFromEdges[pntIndex - EA][0];
            int pt2Index =
              vtkTableBasedClipperTriangulationTables::HexVerticesFromEdges[pntIndex - EA][1];

            if (pt2Index < pt1Index)
            {
              std::swap(pt1Index, pt2Index);
            }

            const double pt1ToPt2 = grdDiffs[pt2Index] - grdDiffs[pt1Index];
            const double pt1ToIso = 0.0 - grdDiffs[pt1Index];
            const double p1Weight = 1.0 - pt1ToIso / pt1ToPt2;

            const int pointId1 = ((theCellI + this->ShiftLUT[0][pt1Index]) +
              (theCellJ + this->ShiftLUT[1][pt1Index]) * this->PyStride +
              (theCellK + this->ShiftLUT[2][pt1Index]) * this->PzStride);
            const int pointId2 = ((theCellI + this->ShiftLUT[0][pt2Index]) +
              (theCellJ + this->ShiftLUT[1][pt2Index]) * this->PyStride +
              (theCellK + this->ShiftLUT[2][pt2Index]) * this->PzStride);

            /* We may have physically (though not logically) degenerate cells
            // if p1Weight == 0 or p1Weight == 1. We could pretty easily and
            // mostly safely clamp percent to the range [1e-4, 1 - 1e-4].
            if( p1Weight == 1.0)
              {
              shapeIds[p] = pointId1;
              }
            else
            if( p1Weight == 0.0 )
              {
              shapeIds[p] = pointId2;
              }
            else

              {
              shapeIds[p] = visItVFV->AddPoint( pointId1, pointId2, p1Weight );
              }
            */

            // Turning on the above code segment, the alternative, would cause
            // a bug with a synthetic Wavelet dataset (vtkImageData) when the
            // clipping plane (x/y/z axis) is positioned exactly at (0,0,0).
            // The problem occurs in the form of an open 'box', as opposed to an
            // expected closed one. This is due to the use of hash instead of a
            // point-locator based detection of duplicate points.
            shapeIds[p] = visItVFV->AddPoint(pointId1, pointId2, p1Weight);
          }
          else if (pntIndex >= N0 && pntIndex <= N3)
          {
            shapeIds[p] = intrpIds[pntIndex - N0];
          }
          else
          {
            vtkLogF(ERROR, "An invalid output point value was found in the ClipCases.\n");
          }
        }

        switch (theShape)
        {
          case ST_HEX:
            visItVFV->AddHex(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3],
              shapeIds[4], shapeIds[5], shapeIds[6], shapeIds[7]);
            break;

          case ST_WDG:
            visItVFV->AddWedge(
              cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3], shapeIds[4], shapeIds[5]);
            break;

          case ST_PYR:
            visItVFV->AddPyramid(
              cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3], shapeIds[4]);
            break;

          case ST_TET:
            visItVFV->AddTet(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3]);
            break;

          case ST_QUA:
            visItVFV->AddQuad(cellId, shapeIds[0], shapeIds[1], shapeIds[2], shapeIds[3]);
            break;

          case ST_TRI:
            visItVFV->AddTri(cellId, shapeIds[0], shapeIds[1], shapeIds[2]);
            break;

          case ST_LIN:
            visItVFV->AddLine(cellId, shapeIds[0], shapeIds[1]);
            break;

          case ST_VTX:
            visItVFV->AddVertex(cellId, shapeIds[0]);
            break;

          case ST_PNT:
            intrpIds[intrpIdx] = visItVFV->AddCentroidPoint(nCellPts, shapeIds);
            break;
        }
      }

      thisCase = nullptr;
    }
  }

  void Reduce() {}

  ThreadLocalDataType GetTLDataType()
  {
    ThreadLocalDataType tlDataType;
    for (auto tItr = this->TLDataType.begin(), tEnd = this->TLDataType.end(); tItr != tEnd; ++tItr)
    {
      tlDataType.push_back(tItr);
    }
    return tlDataType;
  }
};

// Functor to construct simple cells datasets in parallel
class ConstructSimpleDataSetsFunctor
{
private:
  vtkDataSet* Input;
  ThreadLocalDataType& TLDataType;
  TableBasedClipperCommonPointsStructure& CPS;

public:
  ConstructSimpleDataSetsFunctor(
    vtkDataSet* input, ThreadLocalDataType& tlDataType, TableBasedClipperCommonPointsStructure& cps)
    : Input(input)
    , TLDataType(tlDataType)
    , CPS(cps)
  {
    for (auto& localDataType : this->TLDataType)
    {
      localDataType->SimpleClippedCells = vtkSmartPointer<vtkUnstructuredGrid>::New();
    }
  }

  void operator()(vtkIdType begin, vtkIdType end)
  {
    for (vtkIdType i = begin; i < end; ++i)
    {
      this->TLDataType[i]->VisItVFV->ConstructDataSet(
        this->Input, this->TLDataType[i]->SimpleClippedCells, CPS);
    }
  }
};

// Functor to clip special cells datasets in parallel
// Note: if vtkClipDataSet ever becomes multithreaded in the future,
// this functor might become slower because of nested parallelism.
class ClipSpecialCellsFunctor
{
private:
  ThreadLocalDataType& TLDataType;
  vtkDataArray* ClipArray;
  vtkTableBasedClipDataSet* TableBasedClipDataSet;

public:
  ClipSpecialCellsFunctor(ThreadLocalDataType& tlDataType, vtkDataArray* clipArray,
    vtkTableBasedClipDataSet* tableBasedClipDataSet)
    : TLDataType(tlDataType)
    , ClipArray(clipArray)
    , TableBasedClipDataSet(tableBasedClipDataSet)
  {
  }

  void operator()(vtkIdType begin, vtkIdType end)
  {
    for (vtkIdType i = begin; i < end; ++i)
    {
      auto& localDataType = this->TLDataType[i];
      // initialize output
      localDataType->SpecialClippedCells = vtkSmartPointer<vtkUnstructuredGrid>::New();
      // clip if needed
      if (localDataType->SpecialCells->GetNumberOfCells() > 0)
      {
        this->TableBasedClipDataSet->ClipDataSet(
          localDataType->SpecialCells, this->ClipArray, localDataType->SpecialClippedCells);
      }
    }
  }
};

// This functor merges unstructured grids in parallel.
// The speed-up will be maximized if the unstructured grids have similar size.
// Note: duplicate points won't be removed, and polyhedrons are not supported.
class MergeUnstructuredGridsFunctor
{
private:
  vtkDataSet* Input; // original dataset which created the thread parts
  ThreadLocalDataType& TLDataType;
  bool CheckSpecialCells;
  int OutputPointsPrecision;
  vtkUnstructuredGrid* Output;

  // vectors to calculate begin ids for the output arrays
  std::vector<vtkIdType> TLNumberOfPoints;
  std::vector<vtkIdType> TLNumberOfCells;
  std::vector<vtkIdType> TLCellConnectivityArraySize;

  // output arrays which define the output
  vtkSmartPointer<vtkPoints> Points;
  vtkSmartPointer<vtkUnsignedCharArray> CellTypes;
  vtkSmartPointer<vtkIdTypeArray> Offsets;
  vtkSmartPointer<vtkIdTypeArray> Connectivity;

  enum TransformCellFunction
  {
    ConnectivityFunction,
    OffsetsFunction
  };

public:
  MergeUnstructuredGridsFunctor(vtkDataSet* input, ThreadLocalDataType& tlDataType,
    bool checkSpecialCells, int outputPointsPrecision, vtkUnstructuredGrid* output)
    : Input(input)
    , TLDataType(tlDataType)
    , CheckSpecialCells(checkSpecialCells)
    , OutputPointsPrecision(outputPointsPrecision)
    , Output(output)
  {
    const size_t numberOfTlObjects = this->TLDataType.size();
    this->TLNumberOfPoints.resize(numberOfTlObjects);
    this->TLNumberOfCells.resize(numberOfTlObjects);
    this->TLCellConnectivityArraySize.resize(numberOfTlObjects);

    for (size_t i = 0; i < numberOfTlObjects; ++i)
    {
      this->TLNumberOfPoints[i] = 0;
      this->TLNumberOfCells[i] = 0;
      this->TLCellConnectivityArraySize[i] = 0;

      const auto& simpleClippedCells = this->TLDataType[i]->SimpleClippedCells;
      if (simpleClippedCells->GetNumberOfPoints() > 0)
      {
        // add number of points of simple clipped cells
        this->TLNumberOfPoints[i] += simpleClippedCells->GetNumberOfPoints();
        if (simpleClippedCells->GetNumberOfCells() > 0)
        {
          // add number of simple clipped cells
          this->TLNumberOfCells[i] += simpleClippedCells->GetNumberOfCells();
          // add number of simple clipped cells' connectivity array size
          this->TLCellConnectivityArraySize[i] +=
            simpleClippedCells->GetCells()->GetConnectivityArray()->GetNumberOfValues();
        }
      }

      // if we have special cells
      if (this->CheckSpecialCells)
      {
        const auto& specialClippedCells = this->TLDataType[i]->SpecialClippedCells;
        if (specialClippedCells->GetNumberOfPoints() > 0)
        {
          // add number of points of special clipped cells
          this->TLNumberOfPoints[i] += specialClippedCells->GetNumberOfPoints();
          if (specialClippedCells->GetNumberOfCells() > 0)
          {
            // add number of special clipped cells
            this->TLNumberOfCells[i] += specialClippedCells->GetNumberOfCells();
            // add number of special clipped cells' connectivity array size
            this->TLCellConnectivityArraySize[i] +=
              specialClippedCells->GetCells()->GetConnectivityArray()->GetNumberOfValues();
          }
        }
      }
    }
    // calculate total counters;
    vtkIdType totalNumberOfPoints =
      std::accumulate(this->TLNumberOfPoints.begin(), this->TLNumberOfPoints.end(), 0);
    vtkIdType totalNumberOfCells =
      std::accumulate(this->TLNumberOfCells.begin(), this->TLNumberOfCells.end(), 0);
    vtkIdType totalCellConnectivityArraySize = std::accumulate(
      this->TLCellConnectivityArraySize.begin(), this->TLCellConnectivityArraySize.end(), 0);

    // set size of output arrays
    this->Points = vtkSmartPointer<vtkPoints>::New();
    // set precision for the points in the output
    if (this->OutputPointsPrecision == vtkAlgorithm::DEFAULT_PRECISION)
    {
      vtkPointSet* inputPointSet = vtkPointSet::SafeDownCast(this->Input);
      if (inputPointSet)
      {
        this->Points->SetDataType(inputPointSet->GetPoints()->GetDataType());
      }
      else
      {
        this->Points->SetDataType(VTK_FLOAT);
      }
    }
    else if (this->OutputPointsPrecision == vtkAlgorithm::SINGLE_PRECISION)
    {
      this->Points->SetDataType(VTK_FLOAT);
    }
    else if (this->OutputPointsPrecision == vtkAlgorithm::DOUBLE_PRECISION)
    {
      this->Points->SetDataType(VTK_DOUBLE);
    }
    this->Points->SetNumberOfPoints(totalNumberOfPoints);
    // set CellTypes array size
    this->CellTypes = vtkSmartPointer<vtkUnsignedCharArray>::New();
    this->CellTypes->SetNumberOfValues(totalNumberOfCells);
    // set offsets array size
    this->Offsets = vtkSmartPointer<vtkIdTypeArray>::New();
    this->Offsets->SetNumberOfValues(totalNumberOfCells + 1);
    // set connectivity array size
    this->Connectivity = vtkSmartPointer<vtkIdTypeArray>::New();
    this->Connectivity->SetNumberOfValues(totalCellConnectivityArraySize);
    // copy from simpleClippedCells to ensure correctness in CopyData later on
    auto& firstSimpleClippedCells = this->TLDataType[0]->SimpleClippedCells;
    // initialize output point data
    this->Output->GetPointData()->CopyAllocate(
      firstSimpleClippedCells->GetPointData(), totalNumberOfPoints);
    // initialize output cell data
    this->Output->GetCellData()->CopyAllocate(
      firstSimpleClippedCells->GetCellData(), totalNumberOfCells);
  }

  void Initialize() {}

  void operator()(vtkIdType begin, vtkIdType end)
  {
    vtkPointData* outPD = this->Output->GetPointData();
    vtkCellData* outCD = this->Output->GetCellData();

    for (vtkIdType tlId = begin; tlId < end; ++tlId)
    {
      // the following variables will be used as begin ids to correctly write to the output arrays
      vtkIdType beginPointsId =
        std::accumulate(&this->TLNumberOfPoints[0], &this->TLNumberOfPoints[tlId], 0);
      vtkIdType beginCellsId =
        std::accumulate(&this->TLNumberOfCells[0], &this->TLNumberOfCells[tlId], 0);
      vtkIdType beginOffsetValue = std::accumulate(
        &this->TLCellConnectivityArraySize[0], &this->TLCellConnectivityArraySize[tlId], 0);

      auto& simpleClippedCells = this->TLDataType[tlId]->SimpleClippedCells;
      vtkPointData* inSimplePD = simpleClippedCells->GetPointData();
      vtkCellData* inSimpleCD = simpleClippedCells->GetCellData();

      if (simpleClippedCells->GetNumberOfPoints() > 0)
      {
        // copy points
        this->Points->InsertPoints(beginPointsId, simpleClippedCells->GetNumberOfPoints(), 0,
          simpleClippedCells->GetPoints());
        // copy point data
        outPD->CopyData(inSimplePD, beginPointsId, simpleClippedCells->GetNumberOfPoints(), 0);

        if (simpleClippedCells->GetNumberOfCells() > 0)
        {
          // copy cell types
          this->CellTypes->InsertTuples(beginCellsId, simpleClippedCells->GetNumberOfCells(), 0,
            simpleClippedCells->GetCellTypesArray());
          // set cell offsets array
          this->TransformCellInformation(simpleClippedCells->GetCells()->GetOffsetsArray(),
            this->Offsets, beginPointsId, beginCellsId, beginOffsetValue,
            TransformCellFunction::OffsetsFunction);
          // set cell connectivity array
          this->TransformCellInformation(simpleClippedCells->GetCells()->GetConnectivityArray(),
            this->Connectivity, beginPointsId, beginCellsId, beginOffsetValue,
            TransformCellFunction::ConnectivityFunction);
          // Copy cell data
          outCD->CopyData(inSimpleCD, beginCellsId, simpleClippedCells->GetNumberOfCells(), 0);
        }

        // update begin values
        beginPointsId += simpleClippedCells->GetNumberOfPoints();
        if (simpleClippedCells->GetNumberOfCells() > 0)
        {
          beginCellsId += simpleClippedCells->GetNumberOfCells();
          beginOffsetValue +=
            simpleClippedCells->GetCells()->GetConnectivityArray()->GetNumberOfValues();
        }
      }

      if (this->CheckSpecialCells)
      {
        auto& specialClippedCells = this->TLDataType[tlId]->SpecialClippedCells;
        vtkPointData* inSpecialPD = specialClippedCells->GetPointData();
        vtkCellData* inSpecialCD = specialClippedCells->GetCellData();

        if (specialClippedCells->GetNumberOfPoints() > 0)
        {
          // copy points
          this->Points->InsertPoints(beginPointsId, specialClippedCells->GetNumberOfPoints(), 0,
            specialClippedCells->GetPoints());
          // copy point data
          outPD->CopyData(inSpecialPD, beginPointsId, specialClippedCells->GetNumberOfPoints(), 0);

          if (specialClippedCells->GetNumberOfCells() > 0)
          {
            // copy cell types
            this->CellTypes->InsertTuples(beginCellsId, specialClippedCells->GetNumberOfCells(), 0,
              specialClippedCells->GetCellTypesArray());
            // set cell offsets array
            this->TransformCellInformation(specialClippedCells->GetCells()->GetOffsetsArray(),
              this->Offsets, beginPointsId, beginCellsId, beginOffsetValue,
              TransformCellFunction::OffsetsFunction);
            // set cell connectivity array
            this->TransformCellInformation(specialClippedCells->GetCells()->GetConnectivityArray(),
              this->Connectivity, beginPointsId, beginCellsId, beginOffsetValue,
              TransformCellFunction::ConnectivityFunction);
            // Copy cell data
            outCD->CopyData(inSpecialCD, beginCellsId, specialClippedCells->GetNumberOfCells(), 0);
          }

          // update begin values
          beginPointsId += specialClippedCells->GetNumberOfPoints();
          if (specialClippedCells->GetNumberOfCells() > 0)
          {
            beginCellsId += specialClippedCells->GetNumberOfCells();
            beginOffsetValue +=
              specialClippedCells->GetCells()->GetConnectivityArray()->GetNumberOfValues();
          }
        }
      }

      // the last tlId should define the last offset
      if (tlId + 1 == static_cast<vtkIdType>(this->TLNumberOfPoints.size()))
      {
        // set last offset value
        this->Offsets->SetValue(beginCellsId, beginOffsetValue);
      }
    }
  }

  void Reduce()
  {
    this->Output->SetPoints(this->Points);
    vtkNew<vtkCellArray> cells;
    cells->SetData(this->Offsets, this->Connectivity);
    // set nullptr for faces since there will be, for sure, no polyhedrons.
    this->Output->SetCells(this->CellTypes, cells, nullptr, nullptr);
  }

private:
  template <class InputArrayType>
  void TransformCellInformationT(vtkDataArray* inputPointsArray, vtkDataArray* outputPointsArray,
    vtkIdType beginPointId, vtkIdType beginCellId, vtkIdType beginOffsetValue,
    TransformCellFunction transformFunction)
  {
    if (transformFunction == TransformCellFunction::OffsetsFunction)
    {
      // subtract -1 to get the number of cells
      vtkIdType numberOfCells = inputPointsArray->GetNumberOfValues() - 1;
      auto inputArrayPtr = InputArrayType::SafeDownCast(inputPointsArray)->GetPointer(0);
      auto outputArray = vtkIdTypeArray::SafeDownCast(outputPointsArray);
      std::transform(inputArrayPtr, inputArrayPtr + numberOfCells,
        outputArray->GetPointer(beginCellId),
        [&](const vtkIdType& offset) -> vtkIdType { return beginOffsetValue + offset; });
    }
    else if (transformFunction == TransformCellFunction::ConnectivityFunction)
    {
      vtkIdType connectivityArraySize = inputPointsArray->GetNumberOfValues();
      auto inputArrayPtr = InputArrayType::SafeDownCast(inputPointsArray)->GetPointer(0);
      auto outputArray = vtkIdTypeArray::SafeDownCast(outputPointsArray);
      std::transform(inputArrayPtr, inputArrayPtr + connectivityArraySize,
        outputArray->GetPointer(beginOffsetValue),
        [&](const vtkIdType& pointId) -> vtkIdType { return beginPointId + pointId; });
    }
  }

  void TransformCellInformation(vtkDataArray* inputArray, vtkDataArray* outputArray,
    vtkIdType beginPointId, vtkIdType beginCellId, vtkIdType beginOffsetValue,
    TransformCellFunction transformFunction)
  {
    if (vtkTypeInt64Array::SafeDownCast(inputArray) != nullptr)
    {
      this->TransformCellInformationT<vtkTypeInt64Array>(
        inputArray, outputArray, beginPointId, beginCellId, beginOffsetValue, transformFunction);
    }
    else
    {
      this->TransformCellInformationT<vtkTypeInt32Array>(
        inputArray, outputArray, beginPointId, beginCellId, beginOffsetValue, transformFunction);
    }
  }
};

template <typename TGrid>
struct ClipUnstructuredDataBaseWorker
{
  void operator()(vtkDataSet* inputDS, vtkDataArray* clipArray, double isoValue,
    vtkUnstructuredGrid* output, vtkTableBasedClipDataSet* tableBasedClipDataSet,
    int outputPointsPrecision, vtkTypeBool insideOut)
  {
    // clip the simple cells and detect the special cells
    TGrid* input = TGrid::SafeDownCast(inputDS);
    ClipUnstructuredDataFunctor<TGrid> clipUnstructuredData(
      input, clipArray, isoValue, outputPointsPrecision, insideOut);
    vtkSMPTools::For(0, inputDS->GetNumberOfCells(), clipUnstructuredData);
    // gather results of clipUnstructuredDataFunctor
    ThreadLocalDataType tlDataType = clipUnstructuredData.GetTLDataType();

    // check if point accuracy needs to be changed
    vtkNew<vtkDoubleArray> cords;
    vtkPoints* inputPts = input->GetPoints();
    if (inputPts->GetDataType() == VTK_DOUBLE)
    {
      cords->ShallowCopy(inputPts->GetData());
    }
    else
    {
      cords->DeepCopy(inputPts->GetData());
    }

    // construct simple cells datasets
    TableBasedClipperCommonPointsStructure cps;
    cps.hasPtsList = true;
    cps.pts_ptr = cords->GetPointer(0);
    ConstructSimpleDataSetsFunctor constructSimpleDataSets(inputDS, tlDataType, cps);
    vtkSMPTools::For(0, tlDataType.size(), constructSimpleDataSets);
    // delete VisItVFV since they are no longer needed
    for (auto& localDataType : tlDataType)
    {
      delete localDataType->VisItVFV;
    }
    // delete cords if they are no longer needed
    if (inputPts->GetDataType() != VTK_DOUBLE)
    {
      cords->Initialize();
    }

    // check if special cells exist
    bool haveSpecialCells = false;
    for (auto& localDataType : tlDataType)
    {
      if (localDataType->NumberOfSpecialCells > 0)
      {
        haveSpecialCells = true;
        break;
      }
    }

    // if special cells exist, clip them
    if (haveSpecialCells)
    {
      // clip the special cells
      ClipSpecialCellsFunctor clipSpecialCells(tlDataType, clipArray, tableBasedClipDataSet);
      vtkSMPTools::For(0, tlDataType.size(), clipSpecialCells);
      // delete specialCells since they are no longer needed
      for (auto& localDataType : tlDataType)
      {
        localDataType->SpecialCells->Initialize();
      }
    }

    // check if the special clipped cells have polyhedra
    bool havePolyhedra = false;
    if (haveSpecialCells)
    {
      for (auto& localDataType : tlDataType)
      {
        if (localDataType->SpecialClippedCells->GetFaces() != nullptr &&
          localDataType->SpecialClippedCells->GetFaceLocations() != nullptr)
        {
          havePolyhedra = true;
          break;
        }
      }
    }

    if (!havePolyhedra)
    {
      // merge unstructured grids
      MergeUnstructuredGridsFunctor mergeUnstructuredGrids(
        inputDS, tlDataType, haveSpecialCells, outputPointsPrecision, output);
      vtkSMPTools::For(0, tlDataType.size(), mergeUnstructuredGrids);

      // delete SimpleClippedCells and SpecialClippedCells since they are no longer needed
      for (auto& localDataType : tlDataType)
      {
        localDataType->SimpleClippedCells->Initialize();
        if (haveSpecialCells)
        {
          localDataType->SpecialClippedCells->Initialize();
        }
      }
    }
    else // if we have polyhedra, special treatment is needed
    {
      vtkNew<vtkUnstructuredGrid> simpleClippedCellsMerged;
      // construct dataSet using simpleClippedCells only
      MergeUnstructuredGridsFunctor mergeUnstructuredGrids(
        inputDS, tlDataType, false, outputPointsPrecision, simpleClippedCellsMerged);
      vtkSMPTools::For(0, tlDataType.size(), mergeUnstructuredGrids);
      // delete SimpleClippedCells since they are no longer needed
      for (auto& localDataType : tlDataType)
      {
        localDataType->SimpleClippedCells->Initialize();
      }

      // append simpleClippedCellsMerged with SpecialClippedCells
      vtkNew<vtkAppendFilter> appender;
      appender->AddInputData(simpleClippedCellsMerged);
      for (auto& localDataType : tlDataType)
      {
        appender->AddInputData(localDataType->SpecialClippedCells);
      }
      appender->Update();

      // delete SpecialClippedCells since they are no longer needed
      for (auto& localDataType : tlDataType)
      {
        localDataType->SpecialClippedCells->Initialize();
      }

      output->ShallowCopy(appender->GetOutput());
    }
  }
};
using ClipPolyDataWorker = ClipUnstructuredDataBaseWorker<vtkPolyData>;
using ClipUnstructuredGridWorker = ClipUnstructuredDataBaseWorker<vtkUnstructuredGrid>;

template <typename TGrid>
struct ClipStructuredDataBaseWorker
{
  // SetGridDiffsAndCaseIndex when TGrid == vtkRectilinearGrid
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkRectilinearGrid>::value>::type>
  TableBasedClipperCommonPointsStructure SetCords(TGrid* input, std::array<int, 3>& gridDims,
    bool& deleteCords, std::vector<vtkSmartPointer<vtkDoubleArray>>& cords)
  {
    cords.resize(3);
    // init arrays
    for (int i = 0; i < 3; ++i)
    {
      cords[i] = vtkSmartPointer<vtkDoubleArray>::New();
    }
    if (input->GetXCoordinates()->GetDataType() == VTK_DOUBLE &&
      input->GetYCoordinates()->GetDataType() == VTK_DOUBLE &&
      input->GetZCoordinates()->GetDataType() == VTK_DOUBLE)
    {
      deleteCords = false;
      cords[0]->ShallowCopy(input->GetXCoordinates());
      cords[1]->ShallowCopy(input->GetYCoordinates());
      cords[2]->ShallowCopy(input->GetZCoordinates());
    }
    else
    {
      deleteCords = true;
      cords[0]->DeepCopy(input->GetXCoordinates());
      cords[1]->DeepCopy(input->GetYCoordinates());
      cords[2]->DeepCopy(input->GetZCoordinates());
    }
    TableBasedClipperCommonPointsStructure cps;
    cps.hasPtsList = false;
    cps.dims = gridDims.data();
    cps.X = cords[0]->GetPointer(0);
    cps.Y = cords[1]->GetPointer(0);
    cps.Z = cords[2]->GetPointer(0);
    return cps;
  }

  // SetGridDiffsAndCaseIndex when TGrid == vtkStructuredGrid
  template <typename T = TGrid,
    typename = typename std::enable_if<std::is_same<T, vtkStructuredGrid>::value>::type,
    typename = T>
  TableBasedClipperCommonPointsStructure SetCords(TGrid* input,
    std::array<int, 3>& vtkNotUsed(gridDims), bool& deleteCords,
    std::vector<vtkSmartPointer<vtkDoubleArray>>& cords)
  {
    cords.resize(1);
    cords[0] = vtkSmartPointer<vtkDoubleArray>::New();
    vtkPoints* inputPts = input->GetPoints();
    if (inputPts->GetDataType() == VTK_DOUBLE)
    {
      deleteCords = false;
      cords[0]->ShallowCopy(inputPts->GetData());
    }
    else
    {
      deleteCords = true;
      cords[0]->DeepCopy(inputPts->GetData());
    }
    TableBasedClipperCommonPointsStructure cps;
    cps.hasPtsList = true;
    cps.pts_ptr = cords[0]->GetPointer(0);
    return cps;
  }

  void operator()(vtkDataSet* inputDS, vtkDataArray* clipArray, double isoValue,
    vtkUnstructuredGrid* output, int outputPointsPrecision, vtkTypeBool insideOut)
  {
    // clip the simple cells and detect the special cells
    TGrid* input = TGrid::SafeDownCast(inputDS);
    ClipStructuredDataFunctor<TGrid> clipStructuredData(
      input, clipArray, isoValue, outputPointsPrecision, insideOut);
    vtkSMPTools::For(0, inputDS->GetNumberOfCells(), clipStructuredData);
    // gather results of clipStructuredDataFunctor
    ThreadLocalDataType tlDataType = clipStructuredData.GetTLDataType();

    // check if point accuracy needs to be changed
    std::vector<vtkSmartPointer<vtkDoubleArray>> cords;
    std::array<int, 3> gridDims;
    input->GetDimensions(gridDims.data());
    bool deleteCords;
    TableBasedClipperCommonPointsStructure cps =
      this->SetCords(input, gridDims, deleteCords, cords);

    // construct cells datasets
    ConstructSimpleDataSetsFunctor constructSimpleDataSets(inputDS, tlDataType, cps);
    vtkSMPTools::For(0, tlDataType.size(), constructSimpleDataSets);
    // delete VisItVFV since they are no longer needed
    for (auto& localDataType : tlDataType)
    {
      delete localDataType->VisItVFV;
    }
    // delete cords if they are no longer needed
    for (auto& cord : cords)
    {
      if (deleteCords)
      {
        cord->Initialize();
      }
    }

    // merge unstructured grids
    MergeUnstructuredGridsFunctor mergeUnstructuredGrids(
      inputDS, tlDataType, false, outputPointsPrecision, output);
    vtkSMPTools::For(0, tlDataType.size(), mergeUnstructuredGrids);

    // delete SimpleClippedCells since they are no longer needed
    for (auto& localDataType : tlDataType)
    {
      localDataType->SimpleClippedCells->Initialize();
    }
  }
};
using ClipRectilinearGridWorker = ClipStructuredDataBaseWorker<vtkRectilinearGrid>;
using ClipStructuredGridWorker = ClipStructuredDataBaseWorker<vtkStructuredGrid>;
}

//------------------------------------------------------------------------------
// Construct with user-specified implicit function; InsideOut turned off; value
// set to 0.0; and generate clip scalars turned off.
vtkTableBasedClipDataSet::vtkTableBasedClipDataSet(vtkImplicitFunction* cf)
{
  this->Locator = nullptr;
  this->ClipFunction = cf;

  // setup a callback to report progress
  this->InternalProgressObserver = vtkCallbackCommand::New();
  this->InternalProgressObserver->SetCallback(
    &vtkTableBasedClipDataSet::InternalProgressCallbackFunction);
  this->InternalProgressObserver->SetClientData(this);

  this->Value = 0.0;
  this->InsideOut = 0;
  this->MergeTolerance = 0.01;
  this->UseValueAsOffset = true;
  this->GenerateClipScalars = 0;
  this->GenerateClippedOutput = 0;

  this->OutputPointsPrecision = DEFAULT_PRECISION;

  this->SetNumberOfOutputPorts(2);
  vtkUnstructuredGrid* output2 = vtkUnstructuredGrid::New();
  this->GetExecutive()->SetOutputData(1, output2);
  output2->Delete();
  output2 = nullptr;

  // process active point scalars by default
  this->SetInputArrayToProcess(
    0, 0, 0, vtkDataObject::FIELD_ASSOCIATION_POINTS, vtkDataSetAttributes::SCALARS);
}

//------------------------------------------------------------------------------
vtkTableBasedClipDataSet::~vtkTableBasedClipDataSet()
{
  if (this->Locator)
  {
    this->Locator->UnRegister(this);
    this->Locator = nullptr;
  }
  this->SetClipFunction(nullptr);
  this->InternalProgressObserver->Delete();
  this->InternalProgressObserver = nullptr;
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::InternalProgressCallbackFunction(
  vtkObject* arg, unsigned long, void* clientdata, void*)
{
  reinterpret_cast<vtkTableBasedClipDataSet*>(clientdata)
    ->InternalProgressCallback(static_cast<vtkAlgorithm*>(arg));
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::InternalProgressCallback(vtkAlgorithm* algorithm)
{
  double progress = algorithm->GetProgress();
  this->UpdateProgress(progress);

  if (this->AbortExecute)
  {
    algorithm->SetAbortExecute(1);
  }
}

//------------------------------------------------------------------------------
vtkMTimeType vtkTableBasedClipDataSet::GetMTime()
{
  vtkMTimeType time;
  vtkMTimeType mTime = this->Superclass::GetMTime();

  if (this->ClipFunction != nullptr)
  {
    time = this->ClipFunction->GetMTime();
    mTime = (time > mTime ? time : mTime);
  }

  if (this->Locator != nullptr)
  {
    time = this->Locator->GetMTime();
    mTime = (time > mTime ? time : mTime);
  }

  return mTime;
}

vtkUnstructuredGrid* vtkTableBasedClipDataSet::GetClippedOutput()
{
  if (!this->GenerateClippedOutput)
  {
    return nullptr;
  }

  return vtkUnstructuredGrid::SafeDownCast(this->GetExecutive()->GetOutputData(1));
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::SetLocator(vtkIncrementalPointLocator* locator)
{
  if (this->Locator == locator)
  {
    return;
  }

  if (this->Locator)
  {
    this->Locator->UnRegister(this);
    this->Locator = nullptr;
  }

  if (locator)
  {
    locator->Register(this);
  }

  this->Locator = locator;
  this->Modified();
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::CreateDefaultLocator()
{
  if (this->Locator == nullptr)
  {
    this->Locator = vtkMergePoints::New();
    this->Locator->Register(this);
    this->Locator->Delete();
  }
}

//------------------------------------------------------------------------------
int vtkTableBasedClipDataSet::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataSet");
  return 1;
}

//------------------------------------------------------------------------------
int vtkTableBasedClipDataSet::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // input and output information objects
  vtkInformation* inInfo = inputVector[0]->GetInformationObject(0);
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  // Get the input of which we have to create a copy since the clipper requires
  // that InterpolateAllocate() be invoked for the output based on its input in
  // terms of the point data. If the input and output arrays are different,
  // vtkCell3D's Clip will fail. The last argument of InterpolateAllocate makes
  // sure that arrays are shallow-copied from input to inputCopy.
  vtkDataSet* input = vtkDataSet::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkSmartPointer<vtkDataSet> inputCopy;
  inputCopy.TakeReference(input->NewInstance());
  inputCopy->CopyStructure(input);
  inputCopy->GetCellData()->PassData(input->GetCellData());
  inputCopy->GetFieldData()->PassData(input->GetFieldData());
  inputCopy->GetPointData()->InterpolateAllocate(input->GetPointData(), 0, 0, 1);

  // get the output (the remaining and the clipped parts)
  vtkUnstructuredGrid* outputUG =
    vtkUnstructuredGrid::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  vtkUnstructuredGrid* clippedOutputUG = this->GetClippedOutput();

  inInfo = nullptr;
  outInfo = nullptr;
  input = nullptr;
  vtkDebugMacro(<< "Clipping dataset" << endl);

  int i;
  vtkIdType numPoints = inputCopy->GetNumberOfPoints();

  // handling exceptions
  if (numPoints < 1)
  {
    vtkDebugMacro(<< "No data to clip" << endl);
    outputUG = nullptr;
    return 1;
  }

  if (!this->ClipFunction && this->GenerateClipScalars)
  {
    vtkErrorMacro(<< "Cannot generate clip scalars "
                  << "if no clip function defined" << endl);
    outputUG = nullptr;
    return 1;
  }

  vtkDataArray* clipArray = nullptr;
  vtkDoubleArray* pScalars = nullptr;

  // check whether the cells are clipped with input scalars or a clip function
  if (this->ClipFunction)
  {
    pScalars = vtkDoubleArray::New();
    pScalars->SetNumberOfTuples(numPoints);
    pScalars->SetName("ClipDataSetScalars");

    // enable clipDataSetScalars to be passed to the output
    if (this->GenerateClipScalars)
    {
      inputCopy->GetPointData()->SetScalars(pScalars);
    }

    for (i = 0; i < numPoints; i++)
    {
      double s = this->ClipFunction->FunctionValue(inputCopy->GetPoint(i));
      pScalars->SetTuple1(i, s);
    }

    clipArray = pScalars;
  }
  else // using input scalars
  {
    clipArray = this->GetInputArrayToProcess(0, inputVector);
    if (!clipArray)
    {
      vtkErrorMacro(<< "no input scalars." << endl);
      return 1;
    }
  }

  int gridType = inputCopy->GetDataObjectType();
  double isoValue = (!this->ClipFunction || this->UseValueAsOffset) ? this->Value : 0.0;
  if (gridType == VTK_IMAGE_DATA || gridType == VTK_STRUCTURED_POINTS)
  {
    this->ClipImageData(inputCopy, clipArray, isoValue, outputUG);
    if (clippedOutputUG)
    {
      this->InsideOut = !(this->InsideOut);
      this->ClipImageData(inputCopy, clipArray, isoValue, clippedOutputUG);
      this->InsideOut = !(this->InsideOut);
    }
  }
  else if (gridType == VTK_POLY_DATA)
  {
    this->ClipPolyData(inputCopy, clipArray, isoValue, outputUG);
    if (clippedOutputUG)
    {
      this->InsideOut = !(this->InsideOut);
      this->ClipPolyData(inputCopy, clipArray, isoValue, clippedOutputUG);
      this->InsideOut = !(this->InsideOut);
    }
  }
  else if (gridType == VTK_RECTILINEAR_GRID)
  {
    this->ClipRectilinearGridData(inputCopy, clipArray, isoValue, outputUG);
    if (clippedOutputUG)
    {
      this->InsideOut = !(this->InsideOut);
      this->ClipRectilinearGridData(inputCopy, clipArray, isoValue, clippedOutputUG);
      this->InsideOut = !(this->InsideOut);
    }
  }
  else if (gridType == VTK_STRUCTURED_GRID)
  {
    this->ClipStructuredGridData(inputCopy, clipArray, isoValue, outputUG);
    if (clippedOutputUG)
    {
      this->InsideOut = !(this->InsideOut);
      this->ClipStructuredGridData(inputCopy, clipArray, isoValue, clippedOutputUG);
      this->InsideOut = !(this->InsideOut);
    }
  }
  else if (gridType == VTK_UNSTRUCTURED_GRID)
  {
    this->ClipUnstructuredGridData(inputCopy, clipArray, isoValue, outputUG);
    if (clippedOutputUG)
    {
      this->InsideOut = !(this->InsideOut);
      this->ClipUnstructuredGridData(inputCopy, clipArray, isoValue, clippedOutputUG);
      this->InsideOut = !(this->InsideOut);
    }
  }
  else
  {
    this->ClipDataSet(inputCopy, clipArray, outputUG);
    if (clippedOutputUG)
    {
      this->InsideOut = !(this->InsideOut);
      this->ClipDataSet(inputCopy, clipArray, clippedOutputUG);
      this->InsideOut = !(this->InsideOut);
    }
  }

  outputUG->Squeeze();
  outputUG->GetFieldData()->PassData(inputCopy->GetFieldData());

  if (clippedOutputUG)
  {
    clippedOutputUG->Squeeze();
    clippedOutputUG->GetFieldData()->PassData(inputCopy->GetFieldData());
  }

  if (pScalars)
  {
    pScalars->Delete();
  }
  pScalars = nullptr;
  outputUG = nullptr;
  clippedOutputUG = nullptr;
  clipArray = nullptr;

  return 1;
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::ClipDataSet(
  vtkDataSet* pDataSet, vtkDataArray* clipArray, vtkUnstructuredGrid* unstruct)
{
  vtkNew<vtkClipDataSet> clipData;
  clipData->SetInputData(pDataSet);
  clipData->SetValue(this->Value);
  clipData->SetInsideOut(this->InsideOut);
  clipData->SetClipFunction(this->ClipFunction);
  clipData->SetUseValueAsOffset(this->UseValueAsOffset);
  clipData->SetGenerateClipScalars(this->GenerateClipScalars);

  if (!this->ClipFunction)
  {
    pDataSet->GetPointData()->SetScalars(clipArray);
  }
  clipData->Update();
  unstruct->ShallowCopy(clipData->GetOutput());
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::ClipImageData(
  vtkDataSet* inputGrd, vtkDataArray* clipArray, double isoValue, vtkUnstructuredGrid* outputUG)
{
  int i, j;
  int dataDims[3];
  double spacings[3];
  double tmpValue = 0.0;
  vtkImageData* volImage = vtkImageData::SafeDownCast(inputGrd);
  volImage->GetDimensions(dataDims);
  volImage->GetSpacing(spacings);
  const double* dataBBox = volImage->GetBounds();

  vtkNew<vtkDoubleArray> pxCords;
  vtkNew<vtkDoubleArray> pyCords;
  vtkNew<vtkDoubleArray> pzCords;
  vtkDoubleArray* tmpArrays[3] = { pxCords, pyCords, pzCords };
  for (j = 0; j < 3; j++)
  {
    tmpArrays[j]->SetNumberOfValues(dataDims[j]);
    for (tmpValue = dataBBox[j << 1], i = 0; i < dataDims[j]; ++i, tmpValue += spacings[j])
    {
      tmpArrays[j]->SetValue(i, tmpValue);
    }
    tmpArrays[j] = nullptr;
  }

  vtkNew<vtkRectilinearGrid> rectGrid;
  rectGrid->SetDimensions(dataDims);
  rectGrid->SetXCoordinates(pxCords);
  rectGrid->SetYCoordinates(pyCords);
  rectGrid->SetZCoordinates(pzCords);
  rectGrid->GetPointData()->ShallowCopy(volImage->GetPointData());
  rectGrid->GetCellData()->ShallowCopy(volImage->GetCellData());

  this->ClipRectilinearGridData(rectGrid, clipArray, isoValue, outputUG);
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::ClipPolyData(
  vtkDataSet* inputGrd, vtkDataArray* clipArray, double isoValue, vtkUnstructuredGrid* outputUG)
{
  detail::ClipPolyDataWorker clipPolyDataWorker;
  clipPolyDataWorker(
    inputGrd, clipArray, isoValue, outputUG, this, this->OutputPointsPrecision, this->InsideOut);
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::ClipRectilinearGridData(
  vtkDataSet* inputGrd, vtkDataArray* clipArray, double isoValue, vtkUnstructuredGrid* outputUG)
{
  detail::ClipRectilinearGridWorker clipRectilinearGridWorker;
  clipRectilinearGridWorker(
    inputGrd, clipArray, isoValue, outputUG, this->OutputPointsPrecision, this->InsideOut);
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::ClipStructuredGridData(
  vtkDataSet* inputGrd, vtkDataArray* clipArray, double isoValue, vtkUnstructuredGrid* outputUG)
{
  detail::ClipStructuredGridWorker clipStructuredGridWorker;
  clipStructuredGridWorker(
    inputGrd, clipArray, isoValue, outputUG, this->OutputPointsPrecision, this->InsideOut);
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::ClipUnstructuredGridData(
  vtkDataSet* inputGrd, vtkDataArray* clipArray, double isoValue, vtkUnstructuredGrid* outputUG)
{
  detail::ClipUnstructuredGridWorker clipPolyDataWorker;
  clipPolyDataWorker(
    inputGrd, clipArray, isoValue, outputUG, this, this->OutputPointsPrecision, this->InsideOut);
}

//------------------------------------------------------------------------------
void vtkTableBasedClipDataSet::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "Merge Tolerance: " << this->MergeTolerance << "\n";
  if (this->ClipFunction)
  {
    os << indent << "Clip Function: " << this->ClipFunction << "\n";
  }
  else
  {
    os << indent << "Clip Function: (none)\n";
  }
  os << indent << "InsideOut: " << (this->InsideOut ? "On\n" : "Off\n");
  os << indent << "Value: " << this->Value << "\n";
  if (this->Locator)
  {
    os << indent << "Locator: " << this->Locator << "\n";
  }
  else
  {
    os << indent << "Locator: (none)\n";
  }

  os << indent << "Generate Clip Scalars: " << (this->GenerateClipScalars ? "On\n" : "Off\n");

  os << indent << "Generate Clipped Output: " << (this->GenerateClippedOutput ? "On\n" : "Off\n");

  os << indent << "UseValueAsOffset: " << (this->UseValueAsOffset ? "On\n" : "Off\n");

  os << indent << "Precision of the output points: " << this->OutputPointsPrecision << "\n";
}
