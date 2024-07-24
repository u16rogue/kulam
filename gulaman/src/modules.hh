#pragma once

#include <filesystem>
#include <memory>
#include <metapp/common.hh>

class EntryModule {
public:
  EntryModule(std::filesystem::path && path)
    : enabled(false), _path(path), _binary()
  {}

  using UPBinary = std::unique_ptr<mpp::u8[]>;
  bool enabled;
  auto binary() noexcept -> UPBinary&;
  auto path() noexcept -> std::filesystem::path&;
private:
  std::filesystem::path _path;
  std::unique_ptr<mpp::u8[]> _binary;
};
