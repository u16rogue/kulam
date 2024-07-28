#include "modules.hh"
#include "logging.hh"

auto EntryModule::binary() noexcept -> UPBinary& {
  return _binary;
}

auto EntryModule::path() noexcept -> std::filesystem::path& { return _path; }

auto EntryModule::name() noexcept -> std::string_view {
  if (_cache_name.empty()) {
    gulaman_log("Caching name of module entry {}", (void*)this);
    const auto pathstr = name_hover();
    const auto pos = pathstr.find_last_of("/\\");
    if (pos == std::string::npos) {
      return "<err:npos>";
    }
    _cache_name = pathstr.substr(pos + 1);
  }
  

  return _cache_name;
}

auto EntryModule::name_hover() noexcept -> std::string_view {
  if (_cache_name_hover.empty()) {
    gulaman_log("Caching hover text of module entry {}", (void*)this);
    _cache_name_hover = _path.string();
    if (_cache_name_hover.empty()) {
      return "<err:nostr>";
    }
  }

  return _cache_name_hover;
}

auto EntryModule::status() noexcept -> std::string_view {
  return "<status>";
}
