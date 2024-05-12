#include "wildlife_detection_geolocation_plugin/DeviceAgent.h"

#include <chrono>

#include <nx/sdk/analytics/helpers/event_metadata.h>
#include <nx/sdk/analytics/helpers/event_metadata_packet.h>
#include <nx/sdk/analytics/helpers/object_metadata.h>
#include <nx/sdk/analytics/helpers/object_metadata_packet.h>
#include <nx/sdk/ptr.h>

#define NX_DEBUG_ENABLE_OUTPUT true

#include <nx/kit/debug.h>

namespace garuda {
namespace vms_server_plugins {
namespace analytics {
namespace wildlife_detection_geolocation_plugin {

using namespace nx::sdk;
using namespace nx::sdk::analytics;

/**
 * @param deviceInfo Various information about the related device, such as its id, vendor, model,
 *     etc.
 */
DeviceAgent::DeviceAgent(const nx::sdk::IDeviceInfo* deviceInfo,
    std::filesystem::path _pluginHomeDir):
    // Call the DeviceAgent helper class constructor telling it to verbosely report to stderr.
    pluginHomeDir(_pluginHomeDir),
    ConsumingDeviceAgent(deviceInfo, /*enableOutput*/ true)
{
    pluginHomeDir = std::filesystem::path("/home/singh/projects/NetworkOptixHackathon/Garuda/models");
    NX_PRINT << "PLUGIN HOME DIR: " << pluginHomeDir;
    std::string modelPath = (pluginHomeDir / std::filesystem::path("yolov8n.onnx")).generic_string();
    NX_PRINT << "[WILDLIFE DETECTION] Loading Object Detection model from: " << modelPath;

    objectDetector = std::make_unique<object_detection::ObjectDetector>();
    objectDetector->setModelPath(modelPath);
    NX_PRINT << "Created ObjectDetector instance for running object detection on every " <<
        objectDetectionPeriod << "th frame";

    // Load the object detection model from the file
    objectDetector->loadOnnxNetwork();
}

DeviceAgent::~DeviceAgent()
{
}

/**
 *  @return JSON with the particular structure. Note that it is possible to fill in the values
 * that are not known at compile time, but should not depend on the DeviceAgent settings.
 */
std::string DeviceAgent::manifestString() const
{
    // Tell the Server that the plugin can generate the events and objects of certain types.
    // Id values are strings and should be unique. Format of ids:
    // `{vendor_id}.{plugin_id}.{event_type_id/object_type_id}`.
    //
    // See the plugin manifest for the explanation of vendor_id and plugin_id.
    return /*suppress newline*/ 1 + (const char*) R"json(
{
    "eventTypes": [
        {
            "id": ")json" + kNewTrackEventType + R"json(",
            "name": "New track started"
        }
    ],
    "objectTypes": [
        {
            "id": ")json" + kHelloWorldObjectType + R"json(",
            "name": "Hello, World!"
        }
    ]
}
)json";
}

/**
 * Called when the Server sends a new uncompressed frame from a camera.
 */
bool DeviceAgent::pushUncompressedVideoFrame(const IUncompressedVideoFrame* videoFrame)
{
    // Increase the frame count
    ++m_frameIndex;
    m_lastVideoFrameTimestampUs = videoFrame->timestampUs();

    // If the frame count is not divisble by the objectDetectionPeriod return true
    if (m_frameIndex % objectDetectionPeriod == 0)
    {
        NX_OUTPUT << "Running Object Detection for frame index: " << m_frameIndex;
        auto eventMetadataPacket = generateEventMetadataPacket();
        if (eventMetadataPacket)
        {
            // // Create a FrameAdapter instance with the video frame to get the cv Mat instance
            // frameAdapter.frameToCvImg(videoFrame);
            // frameDetectionInfo = objectDetector->runDetection(frameAdapter.cvImg);
            // detectionMetadataPacket = detectionsToObjectMetadataPacket(frameDetectionInfo,
            //     m_lastVideoFrameTimestampUs);

            // detectionMetadataPacket->addRef();
            // pushMetadataPacket(detectionMetadataPacket.releasePtr());
        }
    }

    return true; //< There were no errors while processing the video frame.
}

/**
 * Serves the similar purpose as pushMetadataPacket(). The differences are:
 * - pushMetadataPacket() is called by the plugin, while pullMetadataPackets() is called by Server.
 * - pushMetadataPacket() expects one metadata packet, while pullMetadataPacket expects the
 *     std::vector of them.
 *
 * There are no strict rules for deciding which method is "better". A rule of thumb is to use
 * pushMetadataPacket() when you generate one metadata packet and do not want to store it in the
 * class field, and use pullMetadataPackets otherwise.
 */
bool DeviceAgent::pullMetadataPackets(std::vector<IMetadataPacket*>* metadataPackets)
{
    return true; //< There were no errors while filling metadataPackets.
}

void DeviceAgent::doSetNeededMetadataTypes(
    nx::sdk::Result<void>* /*outValue*/,
    const nx::sdk::analytics::IMetadataTypes* /*neededMetadataTypes*/)
{
}

//-------------------------------------------------------------------------------------------------
// private

Ptr<IMetadataPacket> DeviceAgent::generateEventMetadataPacket()
{
    // Generate event every kTrackFrameCount'th frame.
    if (m_frameIndex % kTrackFrameCount != 0)
        return nullptr;

    // EventMetadataPacket contains arbitrary number of EventMetadata.
    const auto eventMetadataPacket = makePtr<EventMetadataPacket>();
    // Bind event metadata packet to the last video frame using a timestamp.
    eventMetadataPacket->setTimestampUs(m_lastVideoFrameTimestampUs);
    // Zero duration means that the event is not sustained, but momental.
    eventMetadataPacket->setDurationUs(0);

    // EventMetadata contains an information about event.
    const auto eventMetadata = makePtr<EventMetadata>();
    // Set all required fields.
    eventMetadata->setTypeId(kNewTrackEventType);
    eventMetadata->setIsActive(true);
    eventMetadata->setCaption("New sample plugin track started");
    eventMetadata->setDescription("New track #" + std::to_string(m_trackIndex) + " started");

    eventMetadataPacket->addItem(eventMetadata.get());

    // Generate index and track id for the next track.
    ++m_trackIndex;
    m_trackId = nx::sdk::UuidHelper::randomUuid();

    return eventMetadataPacket;
}

Ptr<ObjectMetadataPacket> DeviceAgent::detectionsToObjectMetadataPacket(
    std::vector<utilities::DetectionInfo>& detections,
    int64_t timestampUs)
{
    if (detections.empty())
        return nullptr;

    const auto objectMetadataPacket = makePtr<ObjectMetadataPacket>();

    for (const auto& detection: detections)
    {
        const auto objectMetadata = makePtr<ObjectMetadata>();

        objectMetadata->setBoundingBox(detection.boundingBox);
        objectMetadata->setConfidence(detection.confidence);

        // Convert class label to object metadata type id.
        if (detection.className == "person")
            objectMetadata->setTypeId(kPersonObjectType);
        else if (detection.className == "cat")
            objectMetadata->setTypeId(kCatObjectType);
        else if (detection.className == "dog")
            objectMetadata->setTypeId(kDogObjectType);
        // There is no "else", because only the detections with those types are generated.

        objectMetadataPacket->addItem(objectMetadata.get());
    }
    objectMetadataPacket->setTimestampUs(timestampUs);

    return objectMetadataPacket;
}

} // namespace sample
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
