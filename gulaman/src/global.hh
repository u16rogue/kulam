#pragma once

#include <filesystem>
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

  struct {
    std::vector<int> providers;
  } inline static cache;

  struct {
    std::string title;
  } inline static state;
}
