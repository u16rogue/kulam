#include <gulaman-sdk/gulaman-sdk.hh>
#include "global.hh"
#include "logging.hh"

auto Provider::query(const char*, void*, int) noexcept -> bool {
  return false;
}

auto Provider::transact_start() noexcept -> bool {
  if (transacting) {
    return false;
  }

  this->transacting = true;
  return true;
}

auto Provider::transact_end() noexcept -> bool {
  if (!transacting) {
    return false;
  }

  this->transacting = false;
  return true;
}

auto Provider::target_clear() noexcept -> bool {
  if (!transacting) return false;
  targets.clear();
  return true;
}

auto Provider::target_add(const char * displaytext, unsigned int uid) noexcept -> bool {
  if (!transacting) return false;
  targets.emplace_back(uid, displaytext);
  return true;
}

auto Provider::target_remove(unsigned int /*uid*/) noexcept -> bool {
  if (!transacting) return false;
  return false;
}

auto Provider::target_edit_dt(unsigned int /*uid*/, const char * /*newdisplaytext*/) noexcept -> bool {
  if (!transacting) return false;
  return false;
}

auto Plugin::is_registered() noexcept -> bool {
  return _registered;
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
  _registered = false;
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

auto Plugin::register_provider(const char * name, const char * description, gulaman::sdk::IP_Provider * implementation) noexcept -> gulaman::sdk::IG_Provider* {
  for (const auto & provider : providers) {
    if (provider->interface == implementation) {
      gulaman_log("Provider implementation already registered 0x{} - Rejected.", (void*)implementation);
      return nullptr;
    }
  }

  const auto provider = providers.emplace_back(std::make_unique<Provider>(name, description, implementation)).get();
  gulaman_log("Registered provider {}[{}] from {}[{}]", name, (void*)provider, this->_name, _cache_id);

  for (const int i : global::cache::providers) {
    if (i == _cache_id) {
      return provider;
    }
  }

  global::cache::providers.emplace_back(_cache_id);
  return provider;
}
