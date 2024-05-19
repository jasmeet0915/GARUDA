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

    virtual void doSetNeededMetadataTypes(
        nx::sdk::Result<void>* outValue,
        const nx::sdk::analytics::IMetadataTypes* neededMetadataTypes) override;

private:
    nx::sdk::Ptr<nx::sdk::analytics::IMetadataPacket> generateEventMetadataPacket();
    Ptr<ObjectMetadataPacket> detectionsToObjectMetadataPacket(
        std::vector<utilities::DetectionInfo>& detections,
        int64_t timestampUs);
    nx::sdk::Uuid trackIdByTrackIndex(int trackIndex);

private:
    std::filesystem::path pluginHomeDir;

    // Create object_detector instance
    std::unique_ptr<object_detection::ObjectDetector> objectDetector;

    /** Lenght of the the track (in frames). The value was chosen arbitrarily. */
    static constexpr int kTrackFrameCount = 256;

    nx::sdk::Uuid m_trackId = nx::sdk::UuidHelper::randomUuid();
    int m_frameIndex = 0; /**< Used for generating the detection in the right place. */
    int m_trackIndex = 0; /**< Used in the description of the events. */
    int objectDetectionPeriod = 1;
    bool m_terminated = false;
    float confidenceThreshold = 0.68;

    std::vector<utilities::DetectionInfo> frameDetectionInfo;
    std::vector<nx::sdk::Uuid> m_trackIds;
    utilities::FrameAdapter frameAdapter;
    Ptr<ObjectMetadataPacket> detectionMetadataPacket; 

    // Supported types for object detection
    const std::string kBirdObjectType = "nx.base.Bird";
    const std::string kCatObjectType = "nx.base.Cat";
    const std::string kDogObjectType = "nx.base.Dog";
    const std::string kHorseObjectType = "nx.base.Horse";
    const std::string kSheepObjectType = "nx.base.Sheep";
    const std::string kCowObjectType = "nx.base.Cow";
    const std::string kElephantObjectType = "nx.base.Elephant";
    const std::string kBearObjectType = "nx.base.Bear";
    const std::string kZebraObjectType = "nx.base.Zebra";
    const std::string kGiraffeObjectType = "nx.base.Giraffe";

    // Supported event types
    const std::string kDetectionEventType = "sample.opencv_object_detection.detection";
    const std::string kDetectionEventCaptionSuffix = " detected";
    const std::string kDetectionEventDescriptionSuffix = " detected";
    const std::string kProlongedDetectionEventType =
        "sample.opencv_object_detection.prolongedDetection";

    /** Used for binding object and event metadata to the particular video frame. */
    int64_t m_lastVideoFrameTimestampUs = 0;
};

} // namespace sample
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
