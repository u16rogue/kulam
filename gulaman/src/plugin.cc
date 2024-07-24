#include "plugin.hh"
#include "global.hh"
#include "logging.hh"

auto Plugin::is_active() noexcept -> bool {
  return _active;
}

auto Plugin::query(const char * query, void * data, int size) noexcept -> bool {
  const std::string_view svquery = query;
  if (svquery == "ifc_version_IG_Plugin" && gulaman::sdk::version_size == size) {
    *reinterpret_cast<gulaman::sdk::Version*>(data) = gulaman::sdk::IG_Plugin::version;
    return true;
  }
  return false;
}

auto Plugin::unregister() noexcept -> bool {
  _active = false;
  if (_plugin_interface) {
    _plugin_interface->uninit();
  }
  return true;
}

auto Plugin::set_details(const char * name, const char * description) noexcept -> bool {
  if (name) {
    this->_name = name;
  }

  if (description) {
    this->_description = description;
  }

  return name || description;
}

auto Plugin::register_provider(const char * name, const char * description, gulaman::sdk::IP_Provider * implementation) noexcept -> bool {
  for (const auto & provider : providers) {
    if (provider.interface == implementation) {
      gulaman_log("Provider implementation already registered 0x{} - Rejected.", (void*)implementation);
      return false;
    }
  }

  providers.emplace_back(name, description, implementation);
  gulaman_log("Registered provider {} from {}[{}]", name, this->_name, _cache_id);

  for (const int i : global::cache::providers) {
    if (i == _cache_id) {
      return true;
    }
  }

  global::cache::providers.emplace_back(_cache_id);
  return true;
}
