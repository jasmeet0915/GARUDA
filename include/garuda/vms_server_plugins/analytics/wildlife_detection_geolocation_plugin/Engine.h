#pragma once

#include <filesystem>

#include <nx/sdk/analytics/helpers/plugin.h>
#include <nx/sdk/analytics/helpers/engine.h>
#include <nx/sdk/analytics/i_uncompressed_video_frame.h>

namespace garuda {
namespace vms_server_plugins {
namespace analytics {
namespace wildlife_detection_geolocation_plugin {

class Engine: public nx::sdk::analytics::Engine
{
public:
    Engine(std::filesystem::path _pluginHomeDir);
    virtual ~Engine() override;

protected:
    virtual std::string manifestString() const override;

protected:
    virtual void doObtainDeviceAgent(
        nx::sdk::Result<nx::sdk::analytics::IDeviceAgent*>* outResult,
        const nx::sdk::IDeviceInfo* deviceInfo) override;

private:
    std::filesystem::path pluginHomeDir;
};

} // namespace sample
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
