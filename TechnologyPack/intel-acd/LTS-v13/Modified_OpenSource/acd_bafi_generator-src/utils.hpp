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
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <regex>
#include <sstream>
#include <stdio.h>
#include <string>

const static uint8_t decimal = 10;

[[nodiscard]] static inline bool startsWith(const std::string &input,
                                                  std::string &&search)
{
  return (input.substr(0, search.length()) == search);
}

[[nodiscard]] bool checkInproperValue(const std::string &input)
{
  if (startsWith(input, "UA") || startsWith(input, "N/A") || input == "")
  {
    return true;
  }
  return false;
}

template <typename T>
[[nodiscard]] bool str2uint(const std::string &stringValue, T &output,
                            uint8_t base = 16)
{
  try
  {
    output = std::stoull(stringValue, nullptr, base);
  }
  catch (...)
  {
    if (!checkInproperValue(stringValue))
    {
      std::cerr << "Cannot convert to unsigned int: " << stringValue << "\n";
      return false;
    }
  }
  return true;
}

template <typename T>
[[nodiscard]] inline std::string int_to_hex(T val, size_t width = sizeof(T),
                                            bool prefix = true)
{
  std::stringstream ss;
  if (prefix)
    ss << "0x";

  ss << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
  return ss.str();
}

template <typename T>
[[nodiscard]] std::optional<std::string>
getDecoded(std::map<T, const char *> decodingTable, T toDecode)
{
  const auto &item = decodingTable.find(toDecode);
  if (item != decodingTable.cend()) {
    return item->second;
  }
  return {};
}

[[nodiscard]] std::string countDuration(
                                    std::chrono::steady_clock::time_point end,
                                    std::chrono::steady_clock::time_point start)
{
  auto durationSeconds =
      std::chrono::duration_cast<std::chrono::seconds> (end - start);
  auto durationMiliseconds =
      std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
  std::stringstream ss;
  ss << durationSeconds.count() << "." << durationMiliseconds.count() << "s";
  return ss.str();
}

void showBdfDescription(nlohmann::json deviceMap, nlohmann::ordered_json &bdfObject)
{
  if (deviceMap != NULL)
  {
    for (const auto& [devKey, devVal] : deviceMap["devicesMap"].items())
    {
      if (devVal["bus"] == std::string(bdfObject["Bus"]) &&
      devVal["device"] == std::string(bdfObject["Device"]) &&
      devVal["function"] == std::string(bdfObject["Function"]))
      {
        bdfObject["Description"] = devVal["description"];
      }
    }
  }
}

void getBdfFromFirstMcerrSection(std::string bdfString,
                                  nlohmann::ordered_json &bdfObj)
{
  if (bdfString != "Please refer to system address map")
  {
    std::size_t left = bdfString.find("Bus");
    std::size_t right = bdfString.find(",");
    bdfObj["Bus"] = bdfString.substr(left, right).substr(5);
    left = bdfString.find("Device");
    right = bdfString.substr(right + 2).find(",");
    bdfObj["Device"] = bdfString.substr(left, right).substr(8);
    left = bdfString.find("Function");
    bdfObj["Function"] =
      bdfString.substr(left, bdfString.size()).substr(10);
  }
}

void getBdfFromIoErrorsSection(std::string bdfString,
                                nlohmann::ordered_json &bdfObj)
{
  if (bdfString != "Please refer to system address map")
  {
    std::size_t left = bdfString.find("Bus");
    std::size_t right = bdfString.find("] ");
    bdfObj["Bus"] = bdfString.substr(left, right).substr(4);
    left = bdfString.find("Device");
    right = bdfString.substr(right + 2).find("]");
    bdfObj["Device"] = bdfString.substr(left, right).substr(7);
    left = bdfString.find("Function");
    bdfObj["Function"] = bdfString
    .substr(left, bdfString.size() - left - 1).substr(9);
  }
}

void printHelp() {
  std::cerr << "\nUsage:\n";
  std::cerr << "  bafi [-d][-t][-v][-h][-m memory_map_file][-p device_map_file]";
  std::cerr << "[-s silkscreen_map_file] crashdump_file\n\n";
  std::cerr << "Options:\n";
  std::cerr << "  -d\t\t\tgenerate BAFI output from all crashdump files in current folder\n";
  std::cerr << "  -t\t\t\tprint triage information\n";
  std::cerr << "  --triage\t\tprint triage information\n";
  std::cerr << "  -v\t\t\tversion information\n";
  std::cerr << "  --version\t\tversion information\n";
  std::cerr << "  -h\t\t\tprint help\n";
  std::cerr << "  --help\t\tprint help\n";
  std::cerr << "  -m\t\t\timport memory map from json file\n";
  std::cerr << "  --memory-map\t\timport memory map from json file\n";
  std::cerr << "  -p\t\t\timport device map from json file\n";
  std::cerr << "  --pcie_names\t\timport device map from json file\n";
  std::cerr << "  -s\t\t\timport silkscreen map from json file\n";
  std::cerr << "  --silkscreen_names\timport silkscreen map from json file\n";
  std::cerr << "  memory_map_file\tJSON file containing memory map ";
  std::cerr << "information\n";
  std::cerr << "  device_map_file\tJSON file containing device map ";
  std::cerr << "information\n";
  std::cerr << "  silkscreen_map_file\tJSON file containing silkscreen map ";
  std::cerr << "information\n";
  std::cerr << "  crashdump_file\tJSON file containing Autonomous Crashdump\n";
  std::cerr << "\n\nInstructions:\n";
  std::cerr << "  Step 1: In Summary TSC section, locate which TSC register has";
  std::cerr << " \"Occurred first between all TSCs\" tag.\n";
  std::cerr << "  Step 2: From the first MCERR or first IERR information in step 1, locate the";
  std::cerr << "IP and MCE bank number that asserted FirstIERR, FirstMCERR and";
  std::cerr << "RMCA_FirstMCERR (i.e. FirstMCERR=CHA1 bank 10 M2MEM 0 bank 12, UPI 0 bank 5).\n";
  std::cerr << "  Step 3: Correlate above MCE bank number with error ";
  std::cerr << "information from MCE section or find device Bus/Device/";
  std::cerr << "Function number in 'First.MCERR' section if available.\n\n";
  std::cerr << "I have cscript memory map created using pci.resource_check() ";
  std::cerr << "command or iomem.txt from Linux /proc/iomem. Can I use it in BAFI?\n";
  std::cerr << "  Yes. You can use tools/cscript_map_converter.py like this:\n\n";
  std::cerr << "     > python3 csript_map_converter.py ";
  std::cerr << "<cscript_memory_map_file_format> or <iomem.txt> [-o output_path]\n\n";
  std::cerr << "  This command will create default_memory_map.json that can be ";
  std::cerr << "later used in BAFI using -m (--memory-map) option or by ";
  std::cerr << "copying this map to proper location described in README file.\n\n";
  std::cerr << "I have cscript PCIe map created using pci.devices() ";
  std::cerr << "command or lspci command output from Linux. Can I use it in BAFI?\n";
  std::cerr << "  Yes. You can use tools/devices_log_converter.py like this:\n\n";
  std::cerr << "     > python3 devices_log_converter.py ";
  std::cerr << "<cscript_device_map_file_format> or <lspci.txt> [-o output_path]\n\n";
  std::cerr << "  This command will create default_device_map.json that can be ";
  std::cerr << "later used in BAFI using -p (--pcie_names) option.\n\n";
}

void saveDumpFile(std::string fileName, nlohmann::ordered_json report)
{
  std::filesystem::path filePath = std::filesystem::current_path().string();
  filePath /= "BAFI_" +
                std::filesystem::path(fileName).stem().string() + ".json";
  std::ofstream file(filePath);
  file << report.dump(4);
  file.close();
}

void collectFilesFromDir(std::vector<std::string> &files)
{
    std::regex jsonFileLower(".*.json");
    std::regex jsonFileCapital(".*.JSON");
    std::string path = std::filesystem::current_path().string();
    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        if (std::regex_search(entry.path().string(), jsonFileLower) ||
            std::regex_search(entry.path().string(), jsonFileCapital))
        {
          std::string line;
          std::ifstream file;
          file.open(entry.path().string());
          while (getline(file, line))
          {
            if (line.find("METADATA") != std::string::npos ||
                line.find("metadata") != std::string::npos)
                  files.push_back(entry.path().string());
          }
        }
    }
}

std::string BDFtoString(const std::tuple<uint8_t, uint8_t, uint8_t>& bdf)
{
  auto& [bus, dev, func] = bdf;
  if (bus == 0 && dev == 0 && func == 0)
  {
      return "Please refer to system address map";
  }
  std::stringstream ss;
  ss << "Bus: " << int_to_hex(bus, false) << ", ";
  ss << "Device: " << int_to_hex(dev, false) << ", ";
  ss << "Function: " << int_to_hex(func, false);
  return ss.str();
}

std::tuple<uint8_t, uint8_t, uint8_t> getBDFFromAddress(uint32_t address)
{
  BDFFormatter bdf;
  bdf.address = address;
  return std::make_tuple(static_cast<uint8_t>(bdf.bus),
      static_cast<uint8_t>(bdf.dev), static_cast<uint8_t>(bdf.func));
}