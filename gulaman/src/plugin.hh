#pragma once

#include <gulaman-sdk/gulaman-sdk.hh>
#include <string>
#include <string_view>

class Plugin : public gulaman::sdk::IG_Plugin {
public:
  Plugin(std::string_view identifier, gulaman::sdk::IP_Plugin * pi)
    : identifier(identifier),
      name(identifier),
      description(""),
      plugin_interface(pi)
  {}

public:
  virtual auto query(const char * query, void * data, int size) noexcept -> bool override;
  virtual auto unregister() noexcept -> bool override;
  virtual auto set_details(const char * name, const char * description) noexcept -> bool override;
  virtual auto register_provider(const char * name, const char * description, gulaman::sdk::IP_Provider * implementation) noexcept -> bool override;

public:
  auto is_active() noexcept -> bool;

public:
  const std::string identifier;

private:
  bool active;
  std::string name, description;
  gulaman::sdk::IP_Plugin * plugin_interface;
};
