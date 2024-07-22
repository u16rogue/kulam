#include "plugin.hh"

auto Plugin::is_active() noexcept -> bool {
  return active;
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
  active = false;
  if (plugin_interface) {
    plugin_interface->uninit();
  }
  return true;
}

auto Plugin::set_details(const char * name, const char * description) noexcept -> bool {
  if (name) {
    this->name = name;
  }

  if (description) {
    this->description = description;
  }

  return name || description;
}

auto Plugin::register_provider(const char * name, const char * description, gulaman::sdk::IP_Provider * implementation) noexcept -> bool {
  (void)name;
  (void)description;
  (void)implementation;
  return false;
}
