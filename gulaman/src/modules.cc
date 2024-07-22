#include "modules.hh"

auto EntryModule::binary() -> UPBinary& {
  return _binary;
}

auto EntryModule::name() -> std::string_view {
  if (_name_cache.empty()) {
    const auto str = path.string();
    const auto pos = str.find_last_of("/\\");
    if (pos != std::string::npos) {
      _name_cache = str.substr(pos + 1);
    }
  }
  return _name_cache;
}

auto EntryModule::status() -> std::string_view {
  return std::string_view();
}
