#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <imgui.h>
#include <kitakit/kitakit.hh>
#include <kitakit/kk_imgui.hh>
#include <gulaman-sdk/gulaman-sdk.hh>
#include <metapp/metapp.hh>
#include <string_view>
#include <kitakit/kk_kit.hh>
#include <tuple>
#include <unordered_map>

#include "logging.hh"
#include "global.hh"
#include "modules.hh"

extern auto core_plugin_init() noexcept -> bool; // @defined_at core_plugin.cc
extern auto get_api(const char*) noexcept -> gulaman::sdk::IG_GulamanAPI*; // @defined_at api_impl.cc

#if defined(_WIN32)
  #include <windows.h>
  #include <processthreadsapi.h>
  #define GULAMAN_EXPORT extern "C" __attribute__((dllexport))
  auto DllMain(HMODULE, DWORD, LPVOID) -> BOOL { return TRUE; }
#else
  #define GULAMAN_EXPORT
#endif

struct _EntryModuleCache {
  std::string name;
  std::string name_hover;
  std::string status;
};
static std::unordered_map<void*, _EntryModuleCache> module_entry_cache;
static auto get_module_cache(EntryModule & module) -> std::tuple<
                                                              std::string_view // Name
                                                            , std::string_view // Name Hover
                                                            , std::string_view // Status
                                                            > {
  if (module_entry_cache.find(&module) == module_entry_cache.end()) {
    std::string pathstr = module.path().string();
    const auto pos = pathstr.find_last_of("/\\");
    if (pos == std::string::npos) {
      return std::make_tuple("<err>", "<err>", "<err>");
    }

    module_entry_cache[&module] = { pathstr.substr(pos + 1), pathstr, "<todo>" };
    gulaman_log("Cached {} for {}", (void*)&module, pathstr);
  }

  const auto & entry = module_entry_cache[&module];
  return std::make_tuple(entry.name.c_str(), entry.name_hover.c_str(), entry.status.c_str());
}

static auto on_render(kitakit::EventRender & e) noexcept -> void {
  int width, height;
  e.instance.get_wsize_cache(width, height);

  ImGui::kk_BeginFilled(e, "##menu", 0, ImGuiWindowFlags_MenuBar); {
    mpp_defer { ImGui::End(); };

    if (ImGui::BeginMenuBar()) {
      mpp_defer { ImGui::EndMenuBar(); };
      static int frame_index_indicator = 0;
      ImGui::Text("[%c]", "|/-|\\-/-\\"[frame_index_indicator = (frame_index_indicator + 1) % 9]);
    }

    ImGui::SetNextItemWidth(width * 0.2);
    if (ImGui::BeginCombo("##provider", global::state::active_provider ? global::state::active_provider->name.c_str() : "<select>")) {
      mpp_defer { ImGui::EndCombo(); };
      for (int i : global::cache::providers) {
        auto & plugin = global::entries::plugins[static_cast<mpp::u32>(i)];
        if (!plugin->is_registered()) {
          continue;
        }
        for (auto & provider : plugin->providers) {
          if (ImGui::Selectable(provider.name.c_str(), &provider == global::state::active_provider)) {
            global::state::active_provider = &provider;
          }
        }
      }
    }

    ImGui::SameLine();

    ImGui::SetNextItemWidth(width * 0.75);
    if (ImGui::BeginCombo("##target", []{
                                        if (!global::state::active_provider) return "<no provider>";
                                        return "<select>";
                                      }())) {
      mpp_defer { ImGui::EndCombo(); };
    }

#if defined(ASYNC_IMPORT)
    static kitakit::AsyncTask<void> task_import;
#endif
    if (ImGui::Button("Import")
#if defined(ASYNC_IMPORT)
      && !task_import) { ImGui::OpenPopup("##openfilemodal"); task_import = []() noexcept {
#else
    ) {
#endif
      char file[256] = {};
#if defined(_WIN32)
      OPENFILENAMEA ofn   = {};
      ofn.lStructSize     = sizeof(ofn);
      ofn.lpstrFilter     = "Module (.bin,.dll)\0*.bin;*.dll\0All Files\0*.*\0\0";
      ofn.nMaxFile        = 128;
      ofn.lpstrTitle      = "Import module";
      ofn.hwndOwner       = FindWindowA(NULL, global::state::title.c_str());
      ofn.lpstrFile       = file;

      if (!GetOpenFileNameA(&ofn)) {
        if (auto code = GetLastError(); code != 0) {
          gulaman_log("An error occured on GetOpenFileNameA (Code:{:x})", code);
        }
        return;
      }
#endif
      gulaman_log("Flushing string cache for module entry...");
      module_entry_cache.clear();
      global::entries::modules.emplace_back(file);
    }
#if defined(ASYNC_IMPORT)
    ;} else if (bool modal = task_import; ImGui::BeginPopupModal("##openfilemodal", &modal, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize)) {
      ImGui::Text("Waiting for file dialog...");
      ImGui::EndPopup();
    }
#endif

    if (ImGui::BeginTable("##modules", 4, ImGuiTableFlags_ScrollY)) {
      mpp_defer { ImGui::EndTable(); };
      ImGui::TableSetupScrollFreeze(3, 1);
      ImGui::TableSetupColumn("Enable");
      ImGui::TableSetupColumn("Name");
      ImGui::TableSetupColumn("Status");
      ImGui::TableHeadersRow();

      float btn_offset_precalc = 0.f;
      for (EntryModule & entry : global::entries::modules) {
        const auto & [ name, name_hover, status ] = get_module_cache(entry);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();
        ImGui::PushID(&entry);
        ImGui::Checkbox("##enable_module", &entry.enabled);
        ImGui::PopID();
        ImGui::TableNextColumn();
        ImGui::Text("%s", name.data());
        if (ImGui::IsItemHovered()) {
          ImGui::SetTooltip("%s", name_hover.data());
        }
        ImGui::TableNextColumn();
        ImGui::Text("%s", status.data());
        ImGui::TableNextColumn();
        if (btn_offset_precalc == 0.f) {
          const auto avail = ImGui::GetContentRegionAvail();
          const auto current = ImGui::GetCursorPos();
          btn_offset_precalc = current.x + avail.x - 30;
        }
        ImGui::SetCursorPosX(btn_offset_precalc);
        ImGui::Button("...");
      }
    }
  }
}

GULAMAN_EXPORT auto gulaman_get_interface(const char * _name) noexcept -> void* {
  const std::string_view name = _name;
  if (name.starts_with("api")) {
    return get_api(_name);
  }

  return nullptr;
}

GULAMAN_EXPORT auto gulaman_entry() noexcept -> int {
#if defined(_WIN32)
  AllocConsole();
  FILE * f = nullptr;
  _wfreopen_s(&f, L"CONOUT$", L"w", stdout);
#endif


  gulaman_log(
    "Initializing...\n"
    "Do not close this window!"
  );

  {
    const auto file = std::filesystem::temp_directory_path() / "gulaman.log";
    gulaman_log("Creating log file {}...", file.string());
    if (logging::init_logfile(file)) {
      logging::details::write_log(" -- gulaman log -- ");
    } else {
      gulaman_log("Failed to create log file. Relying on standard out.");
    }
  }

  constexpr char env_plugin_dir_name[] = "GULAMAN_PLUGIN_DIR";
  if (const char * env_plugin_dir = std::getenv(env_plugin_dir_name); env_plugin_dir) {
    if (std::filesystem::exists(env_plugin_dir) && std::filesystem::is_directory(env_plugin_dir)) {
      gulaman_log("env {} detected. Overriding.", env_plugin_dir_name);
      global::directories::plugins_dir = env_plugin_dir;
    } else {
      gulaman_log("env {} Invalid -> {}", env_plugin_dir_name, env_plugin_dir);
    }
  }

  {
    constexpr int max = 16;
    srand((unsigned int)clock());
    char buff[max + 1] = {};
    for (int i = 0; i < max; ++i) {
      buff[i] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"[rand() % 62];
    }
    global::state::title = buff;
  }

  const auto get_tid = [] {
#if defined(_WIN32)
    return GetThreadId(GetCurrentThread());
#endif
  };

  gulaman_log(
    "Setting up threads:\n"
    "  Render: {}\n"
    "  Tick: {}",
    get_tid(),
    -1
  );

  gulaman_log("Initializing core plugin...");
  if (!core_plugin_init()) {
    return 1;
  }

  gulaman_log("Starting render UI...");
  kitakit::run(500, 400, global::state::title.c_str(), nullptr, on_render);

  gulaman_log("Shutting down...");
  for (auto & plugin : global::entries::plugins) {
    plugin->unregister();
  }

  return 0;
}
