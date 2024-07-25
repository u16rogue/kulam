#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "modules.hh"
#include "plugin.hh"

namespace global {
  namespace directories {
    inline std::filesystem::path plugins_dir;
  }

  namespace entries {
    inline std::vector<EntryModule> modules;
    inline std::vector<std::unique_ptr<Plugin>> plugins;
  }

  namespace cache {
    inline std::vector<int> providers; // Indexes to entries::plugins[<cache id>]->providers
  }

  namespace state {
    inline std::string title;
    inline Provider * active_provider = nullptr;
  }
}
