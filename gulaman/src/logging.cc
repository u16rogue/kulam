#include "logging.hh"
#include <fmt/printf.h>
#include <fstream>

static bool has_file = false;
std::ofstream file;

auto logging::init_logfile(const std::filesystem::path & path) noexcept -> bool {
  file.open(path, std::ios::out);
  has_file = file.is_open();
  return true;
}

auto logging::details::write_log(std::string_view log) noexcept -> void {
  if (has_file) {
    file.write(log.data(), static_cast<long long>(log.size()));
    file.flush();
  }
  fmt::print("{}", log);
}
