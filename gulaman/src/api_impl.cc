#include "global.hh"
#include "logging.hh"
#include "plugin.hh"
#include <gulaman-sdk/gulaman-sdk.hh>
#include <memory>
#include <string_view>

class impl_IG_Plugin : public gulaman::sdk::IG_Plugin {
public:
};

class impl_IG_GulamanAPI : public gulaman::sdk::IG_GulamanAPI {
public:
  virtual auto query(const char * _query, void * data, int size) noexcept -> bool override {
    const std::string_view squery = _query;
    if (squery == "ifc_version_IG_GulamanAPI" && gulaman::sdk::version_size == size) {
      *reinterpret_cast<gulaman::sdk::Version*>(data) = impl_IG_GulamanAPI::version;
      return true;
    }
    return false;
  }

  virtual auto register_plugin(const char * identifier, gulaman::sdk::IP_Plugin * plugin_interface) noexcept -> gulaman::sdk::IG_Plugin* override {
    for (const auto & plugin : global::entries.plugins) {
      if (plugin->is_active() && plugin->identifier == identifier) {
        gulaman_log("Cant load plugin as identifier '{}' is already in use.", identifier);
        return nullptr;
      }
    }

    const auto next_plugin_index = global::entries.plugins.size();
    auto & new_plugin = global::entries.plugins.emplace_back(std::make_unique<Plugin>(identifier, plugin_interface, next_plugin_index));
    if (plugin_interface) {
      plugin_interface->init();
    }

    gulaman_log("Registered plugin {} with id {}", identifier, next_plugin_index);
    return new_plugin.get();
  }

  virtual auto listen_event(gulaman::sdk::GulamanEvents /*event*/, void * /*callback*/) noexcept -> bool override {
    return false;
  }
};

auto get_api(const char * _name) noexcept -> gulaman::sdk::IG_GulamanAPI* {
  static std::unique_ptr<impl_IG_GulamanAPI> instance;
  if (!instance) {
    instance = std::make_unique<impl_IG_GulamanAPI>();
    if (!instance) {
      gulaman_log("Failed to create <impl_IG_GulamanAPI>");
      return nullptr;
    }
  }

  const std::string_view name = _name;
  if (name == "api_v_*" || name == "api_v_0_1_0") {
    return instance.get();
  }

  return nullptr;
}
