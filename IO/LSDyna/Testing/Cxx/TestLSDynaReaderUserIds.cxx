/*=========================================================================

  Program:   Visualization Toolkit
  Module:    TestLSDynaReaderUserIds.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Test of vtkLSDynaReader (about node/elem IDs)
// .SECTION Description
// Tests node/elem IDs read by the vtkLSDynaReader.

#include "vtkLSDynaReader.h"

#include "vtkCellData.h"
#include "vtkCompositeDataSet.h"
#include "vtkInformation.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkNew.h"
#include "vtkPointData.h"
#include "vtkPointSet.h"
#include "vtkSetGet.h"
#include "vtkTestUtilities.h"

#include <algorithm>
#include <fstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using ElemIds = std::vector<std::size_t>;
using NodeIds = std::unordered_set<std::size_t>;

std::unordered_map<std::size_t, std::tuple<ElemIds, NodeIds>> ReadIdsFromDynaInput(
  const std::string& fpath)
{
  constexpr char SPACE = ' ';

  auto split = [SPACE](const std::string& s) -> std::vector<std::string> {
    std::vector<std::string> result;
    std::size_t start = 0, end = 0;
    while ((start = s.find_first_not_of(SPACE, end)) != std::string::npos)
    {
      end = s.find_first_of(SPACE, start);
      result.push_back(s.substr(start, end - start));
    }
    return result;
  };

  std::unordered_map<std::size_t, std::tuple<ElemIds, NodeIds>> results;

  std::ifstream ifs(fpath);
  if (!ifs.is_open())
  {
    vtkErrorWithObjectMacro(nullptr, "failed to open LS-DYNA input");
  }

  std::string line;
  std::string block{ "" };
  std::vector<std::size_t> nodeid_glob;

  while (std::getline(ifs, line))
  {
    line.erase(0, line.find_first_not_of(SPACE));
    line.erase(line.find_last_not_of(SPACE) + 1);

    if (line[0] == '$')
    {
      // ignore comments
      continue;
    }
    if (line[0] == '*')
    {
      block = line.substr(1);
      continue;
    }
    if (block == "NODE")
    {
      auto s_nid = line.substr(0, line.find_first_of(SPACE));
      nodeid_glob.push_back(std::stoull(s_nid));
    }
    else if (block.substr(0, 8) == "ELEMENT_")
    {
      auto splitted = split(line);
      if (splitted.size() <= 2)
      {
        vtkErrorWithObjectMacro(nullptr, "wrong input: eid, pid and connectivities are expected");
      }

      const std::size_t eid = std::stoull(splitted[0]);
      const std::size_t pid = std::stoull(splitted[1]);
      if (results.find(pid) == results.end())
      {
        // append new pid
        results.emplace(pid, std::make_tuple<ElemIds, NodeIds>({}, {}));
      }
      auto& res = results.at(pid);
      std::get<0>(res).push_back(eid);
      for (std::size_t i = 2; i < splitted.size(); i++)
      {
        auto nid = std::stoull(splitted[i]);
        if (std::find(nodeid_glob.begin(), nodeid_glob.end(), nid) == nodeid_glob.end())
        {
          vtkErrorWithObjectMacro(nullptr, "wrong input: nid must be in the global nodes");
        }
        std::get<1>(res).emplace(nid);
      }
    }
  }
  return results;
}

std::size_t GetPartId(const std::string& pname)
{
  if (pname.substr(0, 4) != "Part")
  {
    vtkErrorWithObjectMacro(nullptr, << "unexpected part name: " << pname);
  }
  return std::stoull(pname.substr(4));
}

int TestLSDynaReaderUserIds(int argc, char* argv[])
{
  constexpr char name_elem_id[] = "UserIds";
  constexpr char name_node_id[] = "UserID";

  // Read elem/node IDs from LS-DYNA input directly
  char* fname_dynainp =
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/LSDyna/hemi.draw/hemi_draw.k");
  const auto expected_ids = ReadIdsFromDynaInput(fname_dynainp);
  delete[] fname_dynainp;

  // Read file name.
  char* fname =
    vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/LSDyna/hemi.draw/hemi_draw.d3plot");

  // Create the reader.
  vtkNew<vtkLSDynaReader> reader;
  reader->SetFileName(fname);
  reader->Update();
  delete[] fname;

  auto mesh_all = reader->GetOutput();
  auto num_blocks = mesh_all->GetNumberOfBlocks();

  // NOTE: Part5 is not contained in LS-DYNA input file explicitly
  //       because it is rigid plane.
  const std::unordered_set<std::size_t> skipped_pids{ 5 };

  for (decltype(num_blocks) iblock = 0; iblock < num_blocks; iblock++)
  {
    auto info = mesh_all->GetMetaData(iblock);
    auto pid = GetPartId(info->Get(vtkCompositeDataSet::NAME()));

    auto block = vtkPointSet::SafeDownCast(mesh_all->GetBlock(iblock));
    auto cell_data = block->GetCellData();
    auto point_data = block->GetPointData();

    if (!cell_data->HasArray(name_elem_id))
    {
      vtkErrorWithObjectMacro(nullptr, << "data not found: user elem ID (" << name_elem_id << ")");
    }
    if (!point_data->HasArray(name_node_id))
    {
      vtkErrorWithObjectMacro(nullptr, << "data not found: user node ID (" << name_node_id << ")");
    }

    auto arr_elem_id = cell_data->GetAbstractArray(name_elem_id);
    auto arr_node_id = point_data->GetAbstractArray(name_node_id);

    if (skipped_pids.find(pid) != skipped_pids.end())
    {
      // skip assertion
    }
    else
    {
      const auto& item = expected_ids.at(pid);
      const auto& expected_eid = std::get<0>(item);
      const auto& expected_nid = std::get<1>(item);

      if (expected_eid.size() != arr_elem_id->GetNumberOfTuples())
      {
        vtkErrorWithObjectMacro(nullptr,
          "length mismatch for elem IDs: " << expected_eid.size() << ", "
                                           << arr_elem_id->GetNumberOfTuples());
      }
      if (expected_nid.size() != arr_node_id->GetNumberOfTuples())
      {
        vtkErrorWithObjectMacro(nullptr,
          "length mismatch for node IDs: " << expected_nid.size() << ", "
                                           << arr_node_id->GetNumberOfTuples());
      }

      for (std::size_t i = 0; i < arr_elem_id->GetNumberOfTuples(); i++)
      {
        auto v = arr_elem_id->GetVariantValue(i);
        if (!v.IsLongLong())
        {
          vtkErrorWithObjectMacro(
            nullptr, << "wrong type for " << name_elem_id << "  (expected long long)");
        }
        if (v.ToLongLong() != expected_eid.at(i))
        {
          vtkErrorWithObjectMacro(nullptr, << "value mismatch for user elem ID (" << i << "): "
                                           << v.ToLongLong() << ", " << expected_eid.at(i));
        }
      }

      for (std::size_t i = 0; i < arr_node_id->GetNumberOfTuples(); i++)
      {
        // TODO: check the order of node IDs
        auto v = arr_node_id->GetVariantValue(i);
        if (!v.IsLongLong())
        {
          vtkErrorWithObjectMacro(
            nullptr, << "wrong type for " << name_node_id << "  (expected long long)");
        }
        if (expected_nid.find(v.ToLongLong()) == expected_nid.end())
        {
          vtkErrorWithObjectMacro(
            nullptr, << "unexpected value for user elem ID (" << i << "): " << v.ToLongLong());
        }
      }
    }
  }

  return 0;
}
