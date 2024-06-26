/******************************************************************************
 *
 * INTEL CONFIDENTIAL
 *
 * Copyright 2022 Intel Corporation.
 *
 * This software and the related documents are Intel copyrighted materials, and
 * your use of them is governed by the express license under which they were
 * provided to you ("License"). Unless the License provides otherwise, you may
 * not use, modify, copy, publish, distribute, disclose or transmit this
 * software or the related documents without Intel's prior written permission.
 *
 * This software and the related documents are provided as is, with no express
 * or implied warranties, other than those that are expressly stated in
 * the License.
 *
 ******************************************************************************/

#pragma once
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

#include "aer.hpp"
#include "mca_defs.hpp"
#include "tor_defs_cpx.hpp"
#include "tor_defs_icx.hpp"
#include "tor_defs_skx.hpp"
#include "tor_defs_spr.hpp"

using json = nlohmann::json;

class CpuGeneric
{
  public:
  virtual void analyze(const json& input, TORDataCpxSkxMap &output) = 0;
  virtual void analyze(const json& input, TORDataIcxMap &output) = 0;
  virtual void analyze(const json& input, TORDataSprMap &output) = 0;
  [[nodiscard]] virtual MCA analyzeMca(const json& input) = 0;
  [[nodiscard]] virtual CorAer analyzeCorAer(const json& input) = 0;
  [[nodiscard]] virtual UncAer analyzeUncAer(const json& input) = 0;
  [[nodiscard]] virtual std::map<uint32_t, TscData>
                getTscData(const json& input) = 0;
  [[nodiscard]] virtual std::optional<std::map<uint32_t, PackageThermStatus>>
                getThermData(const json& input) = 0;
  [[nodiscard]] virtual std::map<std::string, std::array<uint64_t, 2>>
                getMemoryMap(const json& input) = 0;
  std::string cpuId;
  virtual ~CpuGeneric() {}; //AMI change for coverity
};
