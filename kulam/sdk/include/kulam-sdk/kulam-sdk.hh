#pragma once

namespace kulam::sdk {
  
  struct Version {
    unsigned long long v[3];
  };

  constexpr int version_size = sizeof(Version);
  static_assert(version_size == 24, "kulam::Version must be 24 bytes. Check your compile options.");
  #define kulam_set_interface_version(a, b, c) \
    static constexpr kulam::sdk::Version version = { .v = { a, b, c } }

  //-----------------------------------------------------------------------------------------------

  template <typename IK_>
  constexpr auto is_interface_type(IK_*) noexcept -> bool
  { return false; }

  #define kulam_make_is_interface_type(ifc)                                     \
    template <> inline auto is_interface_type(ifc * interface) noexcept -> bool \
    { return interface->query("ifc_is_"#ifc, nullptr, 0); }

  template <typename IK_>
  constexpr auto interface_version(IK_*, Version*) noexcept -> bool
  { return false; }

  #define kulam_make_interface_version(ifc)                                     \
    template <> inline auto interface_version(ifc * interface, Version * vout) noexcept -> bool \
    { return interface->query("ifc_version_"#ifc, vout, version_size); }

  class IK_Base {
  public:
    kulam_set_interface_version(0, 1, 0);
    virtual auto query(const char * query, void * data, int size) noexcept -> bool = 0;
  };

  //-----------------------------------------------------------------------------------------------

  class IK_ModuleManager : public IK_Base {
  public:
    kulam_set_interface_version(0, 1, 0);
    virtual auto query(const char * query, void * data, int size) noexcept -> bool = 0;
  };
  kulam_make_is_interface_type(IK_ModuleManager)
  kulam_make_interface_version(IK_ModuleManager)

}
