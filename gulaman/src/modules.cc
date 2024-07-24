#include "modules.hh"

auto EntryModule::binary() noexcept -> UPBinary& {
  return _binary;
}

auto EntryModule::path() noexcept -> std::filesystem::path& { return _path; }
