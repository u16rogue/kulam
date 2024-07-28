#pragma once

#include <filesystem>
#include <memory>
#include <metapp/common.hh>
#include <string_view>

class EntryModule {
public:
  EntryModule(std::filesystem::path && path)
    : enabled(false), _path(path), _binary()
  {}

  using UPBinary = std::unique_ptr<mpp::u8[]>;
  bool enabled;
  auto binary() noexcept -> UPBinary&;
  auto path() noexcept -> std::filesystem::path&;

public:
  auto name() noexcept -> std::string_view;
  auto name_hover() noexcept -> std::string_view;
  auto status() noexcept -> std::string_view;

private:
  std::filesystem::path _path;
  std::unique_ptr<mpp::u8[]> _binary;
private:
  std::string _cache_name;
  std::string _cache_name_hover;
  std::string _cache_status;
};
