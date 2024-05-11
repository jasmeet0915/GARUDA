#pragma once

#include <nx/sdk/analytics/helpers/plugin.h>
#include <nx/sdk/analytics/i_engine.h>

namespace garuda {
namespace vms_server_plugins {
namespace analytics {
namespace wildlife_detection_geolocation_plugin {

class Plugin: public nx::sdk::analytics::Plugin
{
protected:
    virtual nx::sdk::Result<nx::sdk::analytics::IEngine*> doObtainEngine() override;
    virtual std::string manifestString() const override;
};

} // namespace sample
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
