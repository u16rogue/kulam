#pragma once

namespace gulaman::sdk {
  
  struct Version {
    unsigned long long v[3];
  };

  constexpr int version_size = sizeof(Version);
  static_assert(version_size == 24, "gulaman::sdk::Version must be 24 bytes. Check your compile options.");
  #define gulaman_set_interface_version(a, b, c) \
    static constexpr gulaman::sdk::Version version = { .v = { a, b, c } }

  /*
   *  Base Interface for Gulaman
   */
  class IG_Base {
  public:
    gulaman_set_interface_version(1, 0, 0);
    virtual auto query(const char * query, void * data, int size) noexcept -> bool = 0;
  };

  /*
   *  Base Interface for Plugins
   */
  class IP_Base {
  public:
    gulaman_set_interface_version(1, 0, 0);
    virtual auto query(const char * query, void * data, int size) noexcept -> bool = 0;
  };

  class IP_Plugin : public IP_Base {
  public:
    virtual auto init() noexcept -> void = 0;
    virtual auto uninit() noexcept -> void = 0;
    virtual auto tick() noexcept -> void = 0;
  };

  class IP_Provider : public IP_Base {
  public:

  };

  class IG_Plugin : public IG_Base {
  public:
    gulaman_set_interface_version(0, 1, 0);

    virtual auto unregister() noexcept -> bool = 0;
    virtual auto set_details(const char * name, const char * description) noexcept -> bool = 0;
    virtual auto register_provider(const char * name, const char * description, IP_Provider * implementation) noexcept -> bool = 0;
  };

  enum class GulamanEvents {
    Shutdown, // Gulaman is shutting down
    PluginLoad,
  };

  class IG_GulamanAPI : public IG_Base {
  public:
    gulaman_set_interface_version(0, 1, 0);

    /*
     *  @description Register a new plugin entry.
     *  @arg         plugin_interface - The registering Plugin's interface implementation that is
     *               provided to gulaman and other plugins that wants to interface with your plugin.
     *  @remark      This parameter is optional and can be null.
     */
    virtual auto register_plugin(const char * identifier, IP_Plugin * plugin_interface) noexcept -> IG_Plugin* = 0;

    /*
     *  @description Listen to events
     */
    virtual auto listen_event(GulamanEvents event, void * callback) noexcept -> bool = 0;
  };

  template <GulamanEvents event, typename T> constexpr auto listen_event(IG_GulamanAPI*, T) -> bool { return false; }

  template <> inline auto listen_event<GulamanEvents::Shutdown>(IG_GulamanAPI * api, void(*callback)(void)) -> bool {
    return api->listen_event(GulamanEvents::Shutdown, (void*)callback);
  }

  template <> inline auto listen_event<GulamanEvents::PluginLoad>(IG_GulamanAPI * api, void(*callback)(IP_Base*)) -> bool {
    return api->listen_event(GulamanEvents::PluginLoad, (void*)callback);
  }
}
