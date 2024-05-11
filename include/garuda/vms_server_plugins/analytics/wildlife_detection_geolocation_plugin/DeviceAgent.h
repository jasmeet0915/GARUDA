#pragma once

#include <filesystem>

#include <nx/sdk/analytics/helpers/consuming_device_agent.h>
#include <nx/sdk/analytics/helpers/event_metadata_packet.h>
#include <nx/sdk/analytics/helpers/object_metadata_packet.h>
#include <nx/sdk/helpers/uuid_helper.h>
#include <nx/sdk/ptr.h>

#include "wildlife_detection_geolocation_plugin/Engine.h"
#include "object_detection/ObjectDetector.h"
#include "utilities/FrameAdapter.h"

namespace garuda {
namespace vms_server_plugins {
namespace analytics {
namespace wildlife_detection_geolocation_plugin {

using namespace nx::sdk;
using namespace nx::sdk::analytics;

class DeviceAgent: public nx::sdk::analytics::ConsumingDeviceAgent
{
public:
    DeviceAgent(const nx::sdk::IDeviceInfo* deviceInfo,
        std::filesystem::path _pluginHomeDir);
    virtual ~DeviceAgent() override;

protected:
    virtual std::string manifestString() const override;

    virtual bool pushUncompressedVideoFrame(
        const nx::sdk::analytics::IUncompressedVideoFrame* videoFrame) override;

    virtual bool pullMetadataPackets(
        std::vector<nx::sdk::analytics::IMetadataPacket*>* metadataPackets) override;

    virtual void doSetNeededMetadataTypes(
        nx::sdk::Result<void>* outValue,
        const nx::sdk::analytics::IMetadataTypes* neededMetadataTypes) override;

private:
    nx::sdk::Ptr<nx::sdk::analytics::IMetadataPacket> generateEventMetadataPacket();
    Ptr<ObjectMetadataPacket> detectionsToObjectMetadataPacket(
        std::vector<utilities::DetectionInfo>& detections,
        int64_t timestampUs);

private:
    const std::string kHelloWorldObjectType = "nx.sample.helloWorld";
    const std::string kNewTrackEventType = "nx.sample.newTrack";

    std::filesystem::path pluginHomeDir;

    // Create object_detector instance
    std::unique_ptr<object_detection::ObjectDetector> objectDetector;

    /** Lenght of the the track (in frames). The value was chosen arbitrarily. */
    static constexpr int kTrackFrameCount = 256;

private:
    nx::sdk::Uuid m_trackId = nx::sdk::UuidHelper::randomUuid();
    int m_frameIndex = 0; /**< Used for generating the detection in the right place. */
    int m_trackIndex = 0; /**< Used in the description of the events. */
    int objectDetectionPeriod = 5;

    std::vector<utilities::DetectionInfo> frameDetectionInfo;
    utilities::FrameAdapter frameAdapter;
    Ptr<ObjectMetadataPacket> detectionMetadataPacket; 

    const std::string kPersonObjectType = "nx.base.Person";
    const std::string kCatObjectType = "nx.base.Cat";
    const std::string kDogObjectType = "nx.base.Dog";

    /** Used for binding object and event metadata to the particular video frame. */
    int64_t m_lastVideoFrameTimestampUs = 0;
};

} // namespace sample
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
