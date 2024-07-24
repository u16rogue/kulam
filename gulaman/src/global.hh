#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "modules.hh"
#include "plugin.hh"

namespace global {
  struct {
    std::filesystem::path plugins_dir;
  } inline static directories;

  struct {
    std::vector<EntryModule> modules;
    std::vector<std::unique_ptr<Plugin>> plugins;
  } inline static entries;

  class ModuleInfoCache {
    std::string name;
    std::string path;
  };

  struct {
    std::vector<int> providers;
    std::vector<ModuleInfoCache> modules;
  } inline static cache;

  struct {
    std::string title;
  } inline static state;
}
