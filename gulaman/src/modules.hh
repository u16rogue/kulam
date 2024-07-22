#pragma once

#include <filesystem>
#include <memory>
#include <metapp/common.hh>
#include <string_view>

class EntryModule {
public:
  EntryModule(std::filesystem::path && path)
    : enabled(false), path(path), _binary()
  {}

  using UPBinary = std::unique_ptr<mpp::u8[]>;
  bool enabled;
  std::filesystem::path path;
  auto binary() -> UPBinary&;
  auto name() -> std::string_view;
  auto status() -> std::string_view;
private:
  std::unique_ptr<mpp::u8[]> _binary;
  std::string _name_cache;
};
