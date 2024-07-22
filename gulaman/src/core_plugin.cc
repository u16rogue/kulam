#include <gulaman-sdk/gulaman-sdk.hh>
#include <memory>
#include <utility>

#include "logging.hh"

extern "C" auto gulaman_get_interface(const char *) noexcept -> gulaman::sdk::IG_GulamanAPI*;

static gulaman::sdk::IG_Plugin * plugself = nullptr;

class LoadLibraryProvider : public gulaman::sdk::IP_Provider {
public:
  virtual auto query(const char * /*query*/, void * /*data*/, int /*size*/) noexcept -> bool override { return false; }
};

std::unique_ptr<LoadLibraryProvider> instance_provider_ll;

auto core_plugin_init() noexcept -> bool {
  gulaman_log("Loading core plugin...");

  auto api = gulaman_get_interface("api_v_*");
  if (!api) {
    gulaman_log("Core plugin cant get api interface.");
    return false;
  }

  plugself = api->register_plugin("core_plugin", nullptr);
  if (!plugself) {
    gulaman_log("Failed to register core plugin.");
    return false;
  }

  if (!plugself->set_details("Core Plugin", "The core plugin of gulaman!")) {
    gulaman_log("Failed to set details of core plugin");
    return false;
  }

  auto _instance_provider_ll = std::make_unique<LoadLibraryProvider>();
  if (!_instance_provider_ll) {
    gulaman_log("Failed to create core LoadLibrary provider instance.");
    return false;
  }

  if (!plugself->register_provider("GenericLoadLibrary", "Typical Load Library Loader", _instance_provider_ll.get())) {
    gulaman_log("Failed to register core LoadLibrary provider instance.");
    return false;
  }

  instance_provider_ll = std::move(_instance_provider_ll);
  gulaman_log("Core plugin successfully initialized!");
  return true;
}
