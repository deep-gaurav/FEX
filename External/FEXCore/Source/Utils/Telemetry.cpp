#include <FEXCore/Config/Config.h>
#include <FEXCore/Utils/LogManager.h>
#include <FEXCore/Utils/Telemetry.h>

#include <array>
#include <filesystem>
#include <fstream>
#include <stddef.h>
#include <string>
#include <string_view>
#include <system_error>

namespace FEXCore::Telemetry {
#ifndef FEX_DISABLE_TELEMETRY
  static std::array<Value, FEXCore::Telemetry::TelemetryType::TYPE_LAST> TelemetryValues = {{ }};
  const std::array<std::string_view, FEXCore::Telemetry::TelemetryType::TYPE_LAST> TelemetryNames {
    "64byte Split Locks",
    "16byte Split atomics",
    "VEX instructions (AVX)",
    "EVEX instructions (AVX512)",
    "16bit CAS Tear",
    "32bit CAS Tear",
    "64bit CAS Tear",
    "128bit CAS Tear",
  };
  void Initialize() {
    auto DataDirectory = Config::GetDataDirectory();
    DataDirectory += "Telemetry/";

    // Ensure the folder structure is created for our configuration
    std::error_code ec{};
    if (!std::filesystem::exists(DataDirectory, ec) &&
        !std::filesystem::create_directories(DataDirectory, ec)) {
      LogMan::Msg::IFmt("Couldn't create telemetry Folder");
    }
  }

  void Shutdown(std::filesystem::path &ApplicationName) {
    auto DataDirectory = Config::GetDataDirectory();
    DataDirectory += "Telemetry/" + ApplicationName.string() + ".telem";

    std::error_code ec{};
    if (std::filesystem::exists(DataDirectory, ec)) {
      // If the file exists, retain a single backup
      auto Backup = DataDirectory + ".1";
      std::filesystem::copy_file(DataDirectory, Backup, std::filesystem::copy_options::overwrite_existing, ec);
    }

    std::fstream fs(DataDirectory, std::ios_base::out | std::ios_base::trunc);
    if (fs.is_open()) {
      for (size_t i = 0; i < TelemetryType::TYPE_LAST; ++i) {
        auto &Name = TelemetryNames.at(i);
        auto &Data = TelemetryValues.at(i);
        fs << Name << ": " << *Data << std::endl;
      }

      fs.flush();
      fs.close();
    }
  }

  Value &GetObject(TelemetryType Type) {
    return TelemetryValues.at(Type);
  }
#endif
}
