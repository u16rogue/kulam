#pragma once

#include <gulaman-sdk/gulaman-sdk.hh>
#include <string>
#include <string_view>
#include <vector>

struct Provider {
  Provider(std::string_view name, std::string_view description, gulaman::sdk::IP_Provider * interface)
    : name(name), description(description), interface(interface)
  {}
  std::string name;
  std::string description;
  gulaman::sdk::IP_Provider * interface;
};

class Plugin : public gulaman::sdk::IG_Plugin {
public:
  Plugin(std::string_view identifier, gulaman::sdk::IP_Plugin * pi, int cache_id)
    : identifier(identifier),
      _cache_id(cache_id),
      _name(identifier),
      _description(""),
      _plugin_interface(pi)
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

public:
  std::vector<Provider> providers;

private:
  int _cache_id;
  bool _active;
  std::string _name, _description;
  gulaman::sdk::IP_Plugin * _plugin_interface;
};
