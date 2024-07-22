#pragma once

#if defined(GULAMAN_LOGGING)
  #include <fmt/printf.h>
  #include <fmt/color.h>
  #include <string_view>
  #include <filesystem>
  
  namespace logging {
    namespace details {
      constexpr int gsd_len = sizeof(GULAMAN_SOURCE_DIR);
      auto write_log(std::string_view log) noexcept -> void;
    }

    auto init_logfile(const std::filesystem::path & path) noexcept -> bool;
  }
  
  #define gulaman_log(msg, ...)                          \
    logging::details::write_log(fmt::format(             \
      "\n[@] {} {} {}\n"                                 \
      msg                                                \
    , (const char *)__FILE__ + logging::details::gsd_len \
    , __FUNCTION__                                       \
    , __LINE__                                           \
    __VA_OPT__(,) __VA_ARGS__                            \
  ))
#else // #if defined(GULAMAN_LOGGING)
  #define gulaman_log(...)
  #define gulaman_newline
#endif // #if defined(GULAMAN_LOGGING)
