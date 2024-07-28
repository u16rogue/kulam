#include <gulaman-sdk/gulaman-sdk.hh>
#include <memory>
#include <utility>

#include "logging.hh"

extern "C" auto gulaman_get_interface(const char *) noexcept -> void*;

class OSLoadProvider;
class SelfPlugin;

static gulaman::sdk::IG_Plugin * pluginapi = nullptr;
static std::unique_ptr<OSLoadProvider> instance_provider_osl;
static gulaman::sdk::IG_Provider * instance_api_osl = nullptr;
std::unique_ptr<SelfPlugin> instance_self;

class OSLoadProvider : public gulaman::sdk::IP_Provider {
public:
  virtual auto query(const char * /*query*/, void * /*data*/, int /*size*/) noexcept -> bool override { return false; }
  virtual auto event_select_provider() noexcept -> void override {
    instance_api_osl->transact_start();

    instance_api_osl->transact_end();
  }

  virtual auto event_unselect_provider() noexcept -> void override {
  }
};

class SelfPlugin : public gulaman::sdk::IP_Plugin {
public:
  virtual auto query(const char*, void*, int) noexcept -> bool override {
    return false;
  }

  virtual auto init() noexcept -> void override {
    gulaman_log("Core plugin init");
  }

  virtual auto uninit() noexcept -> void override {
    gulaman_log("Uninitializing core plugin...");
    instance_provider_osl = nullptr;
    instance_self = nullptr;
  }

  virtual auto tick() noexcept -> void override {
  }
};

auto core_plugin_init() noexcept -> bool {
  gulaman_log("Loading core plugin...");

  auto api = reinterpret_cast<gulaman::sdk::IG_GulamanAPI*>(gulaman_get_interface("api_v_*"));
  if (!api) {
    gulaman_log("Core plugin cant get api interface.");
    return false;
  }

  instance_self = std::make_unique<SelfPlugin>();
  if (!instance_self) {
    gulaman_log("Failed to create self instance");
    return false;
  }

  pluginapi = api->register_plugin("core_plugin", instance_self.get());
  if (!pluginapi) {
    gulaman_log("Failed to register core plugin.");
    return false;
  }

  if (!pluginapi->set_details("Core Plugin", "The core plugin of gulaman!")) {
    gulaman_log("Failed to set details of core plugin");
    return false;
  }

  auto _instance_provider_ll = std::make_unique<OSLoadProvider>();
  if (!_instance_provider_ll) {
    gulaman_log("Failed to create core LoadLibrary provider instance.");
    return false;
  }

  instance_api_osl = pluginapi->register_provider("OSLoad", "Module loading by OS", _instance_provider_ll.get());
  if (!instance_api_osl) {
    gulaman_log("Failed to register core OSLoad provider instance.");
    return false;
  }

  instance_provider_osl = std::move(_instance_provider_ll);
  gulaman_log("Core plugin successfully initialized!");
  return true;
}
