#pragma once

#include <gulaman-sdk/gulaman-sdk.hh>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

using TargetEntryId = unsigned int;

struct TargetEntry {
  TargetEntry(TargetEntryId id, std::string_view display_text)
    : id(id), display_text(display_text)
  {}
  TargetEntryId id;
  std::string display_text;
};

class Provider : public gulaman::sdk::IG_Provider {
public:
  Provider(std::string_view name, std::string_view description, gulaman::sdk::IP_Provider * interface)
    : name(name), description(description), interface(interface), target_id_selected(0), transacting(false)
  {}
  std::string name;
  std::string description;
  gulaman::sdk::IP_Provider * interface;

  std::vector<TargetEntry> targets;
  TargetEntryId target_id_selected;
  bool transacting;

public:
  virtual auto query(const char * query, void * data, int size) noexcept -> bool override;
  virtual auto transact_start() noexcept -> bool override;
  virtual auto transact_end() noexcept -> bool override;

  virtual auto target_clear() noexcept -> bool override;
  virtual auto target_add(const char * displaytext, unsigned int uid) noexcept -> bool override;
  virtual auto target_remove(unsigned int uid) noexcept -> bool override;
  virtual auto target_edit_dt(unsigned int uid, const char * newdisplaytext) noexcept -> bool override;
};

class Plugin : public gulaman::sdk::IG_Plugin {
public:
  Plugin(std::string_view identifier, gulaman::sdk::IP_Plugin * pi, int cache_id)
    : identifier(identifier)
    , _cache_id(cache_id)
    , _registered(true)
    , _name(identifier)
    , _description("")
    , _plugin_interface(pi)
  {}

public:
  virtual auto query(const char * query, void * data, int size) noexcept -> bool override;
  virtual auto unregister() noexcept -> bool override;
  virtual auto set_details(const char * name, const char * description) noexcept -> bool override;
  virtual auto register_provider(const char * name, const char * description, gulaman::sdk::IP_Provider * implementation) noexcept -> gulaman::sdk::IG_Provider* override;

public:
  auto is_registered() noexcept -> bool;

public:
  const std::string identifier;

public:
  std::vector<std::unique_ptr<Provider>> providers;

private:
  int _cache_id;
  bool _registered;
  std::string _name, _description;
  gulaman::sdk::IP_Plugin * _plugin_interface;
};
