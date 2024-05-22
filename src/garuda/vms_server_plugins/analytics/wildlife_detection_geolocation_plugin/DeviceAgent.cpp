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
    NX_PRINT << "PLUGIN HOME DIR: " << pluginHomeDir;
    std::string modelPath = (pluginHomeDir / std::filesystem::path("yolov8n.onnx")).string();
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
            "id": ")json" + kPoacherDetectionEventType + R"json(",
            "name": "Animal Poacher Detected"
        }
    ],
    "objectTypes": [
        {
            "id": ")json" + kBirdObjectType + R"json(",
            "name": "Bird"
        },
        {
            "id": ")json" + kCatObjectType + R"json(",
            "name": "Cat"
        },
        {
            "id": ")json" + kDogObjectType + R"json(",
            "name": "Dog"
        },
        {
            "id": ")json" + kHorseObjectType + R"json(",
            "name": "Horse"
        },
        {
            "id": ")json" + kSheepObjectType + R"json(",
            "name": "Sheep"
        },
        {
            "id": ")json" + kCowObjectType + R"json(",
            "name": "Cow"
        },
        {
            "id": ")json" + kElephantObjectType + R"json(",
            "name": "Elephant"
        },
        {
            "id": ")json" + kBearObjectType + R"json(",
            "name": "Bear"
        },
        {
            "id": ")json" + kZebraObjectType + R"json(",
            "name": "Zebra"
        },
        {
            "id": ")json" + kGiraffeObjectType + R"json(",
            "name": "Giraffe"
        },
        {
            "id": ")json" + kPersonObjectType + R"json(",
            "name": "Person"
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
        // Clear previous frame and detection info
        frameAdapter = {};
        frameDetectionInfo.clear();
        m_trackIds.clear();

        NX_OUTPUT << "Running Object Detection for frame index: " << m_frameIndex;

        // Create a FrameAdapter instance with the video frame to get the cv Mat instance
        frameAdapter.frameToCvImg(videoFrame);
        NX_OUTPUT << "Converted frame to cv::Mat";

        frameDetectionInfo = objectDetector->runDetection(frameAdapter.cvImg);
        NX_OUTPUT << "Ran Detections and detected: " << frameDetectionInfo.size() << " objects";

        detectionMetadataPacket = detectionsToObjectMetadataPacket(frameDetectionInfo,
            m_lastVideoFrameTimestampUs);
        NX_OUTPUT << "Converted DetectionInfo to ObjectMetadata";

        if (!detectionMetadataPacket)
        {
            NX_PRINT << "No animals detected";
            return true;
        }

        eventMetadataPacket = generateEventMetadataPacket(frameDetectionInfo,
            m_lastVideoFrameTimestampUs);
        NX_OUTPUT << "Created Event Metadata Packet";

        // Create the metadata packet list with detection and event metadata
        MetadataPacketList resultMetadata;
        if (detectionMetadataPacket)
        {
            resultMetadata.push_back(detectionMetadataPacket);
        }
        if (eventMetadataPacket)
        {
            resultMetadata.push_back(eventMetadataPacket);
        }

        for (const Ptr<IMetadataPacket>& metadataPacket: resultMetadata)
        {
            metadataPacket->addRef();
            pushMetadataPacket(metadataPacket.get());
        }
    }

    return true; //< There were no errors while processing the video frame.
}

void DeviceAgent::doSetNeededMetadataTypes(
    nx::sdk::Result<void>* /*outValue*/,
    const nx::sdk::analytics::IMetadataTypes* /*neededMetadataTypes*/)
{
}

//-------------------------------------------------------------------------------------------------
// private

Ptr<IMetadataPacket> DeviceAgent::generateEventMetadataPacket(
    std::vector<utilities::DetectionInfo>& detections,
    int64_t timestampUs)
{
    if (detections.empty())
    {
        NX_OUTPUT << "No Animals Detected";
        return nullptr;
    }

    // EventMetadataPacket contains arbitrary number of EventMetadata.
    const auto eventMetadataPacket = makePtr<EventMetadataPacket>();
    // Bind event metadata packet to the last video frame using a timestamp.
    eventMetadataPacket->setTimestampUs(timestampUs);
    // Zero duration means that the event is not sustained, but momental.
    eventMetadataPacket->setDurationUs(0);

    bool poacherDetected = false;

    for (auto& detection : detections)
    {
        if (detection.className != "person")
        {
            NX_OUTPUT << "No Poacher Detected";
            continue;
        }
        // EventMetadata contains an information about event.
        const auto eventMetadata = makePtr<EventMetadata>();
        // Set all required fields.
        eventMetadata->setTypeId(kPoacherDetectionEventType);
        eventMetadata->setIsActive(true);
        eventMetadata->setCaption("Animal Poacher Detected! Animals in Danger! Contact the Park Ranger");
        eventMetadata->setDescription("Animal Poacher Detection Started");

        eventMetadataPacket->addItem(eventMetadata.get());
        poacherDetected = true;
        // Generate index and track id for the next track.
        // ++m_trackIndex;
        // m_trackId = nx::sdk::UuidHelper::randomUuid();
    }

    // If no poacher is detected, return nullptr
    if (!poacherDetected)
    {
        return nullptr;
    }

    return eventMetadataPacket;
}

Ptr<ObjectMetadataPacket> DeviceAgent::detectionsToObjectMetadataPacket(
    std::vector<utilities::DetectionInfo>& detections,
    int64_t timestampUs)
{
    if (detections.empty())
    {
        NX_OUTPUT << "No Animals Detected";
        return nullptr;
    }

    NX_OUTPUT << "Converting detections to object metadata";

    const auto objectMetadataPacket = makePtr<ObjectMetadataPacket>();

    int objectCounter = 0;
    for (const auto& detection: detections)
    {
        const auto objectMetadata = makePtr<ObjectMetadata>();

        NX_OUTPUT << "Detected: " << detection.className << " with Confidence: " << detection.confidence;

        if (!detection.boundingBox.isValid())
        {
            NX_OUTPUT << "Object Detected but Bounding Box is invalid";
            NX_OUTPUT << "Bounding Box is: " << detection.boundingBox.x << ", " << detection.boundingBox.y <<
                ", " << detection.boundingBox.width << ", " << detection.boundingBox.height;
            continue;
        }
        NX_OUTPUT << "Bounding Box is: " << detection.boundingBox.x << ", " << detection.boundingBox.y <<
            ", " << detection.boundingBox.width << ", " << detection.boundingBox.height;

        objectMetadata->setBoundingBox(detection.boundingBox);
        objectMetadata->setConfidence(detection.confidence);

        // Reject detection if confidence is less than threshold
        if (detection.confidence < confidenceThreshold)
        {
            NX_PRINT << "Rejected detection due to low confidence";
            continue;
        }

        // Convert class label to object metadata type id.
        if (detection.className == "bird")
            objectMetadata->setTypeId(kBirdObjectType);
        else if (detection.className == "cat")
            objectMetadata->setTypeId(kCatObjectType);
        else if (detection.className == "dog")
            objectMetadata->setTypeId(kDogObjectType);
        else if (detection.className == "horse")
            objectMetadata->setTypeId(kHorseObjectType);
        else if (detection.className == "sheep")
            objectMetadata->setTypeId(kSheepObjectType);
        else if (detection.className == "cow")
            objectMetadata->setTypeId(kCowObjectType);
        else if (detection.className == "elephant")
            objectMetadata->setTypeId(kElephantObjectType);
        else if (detection.className == "bear")
            objectMetadata->setTypeId(kBearObjectType);
        else if (detection.className == "zebra")
            objectMetadata->setTypeId(kZebraObjectType);
        else if (detection.className == "giraffe")
            objectMetadata->setTypeId(kGiraffeObjectType);
        // There is no "else", because only the detections with those types are generated.

        objectCounter++;
        objectMetadata->setTrackId(trackIdByTrackIndex(objectCounter));

        objectMetadataPacket->addItem(objectMetadata.get());
    }
    objectMetadataPacket->setTimestampUs(timestampUs);

    return objectMetadataPacket;
}

Uuid DeviceAgent::trackIdByTrackIndex(int trackIndex)
{
    while (trackIndex >= m_trackIds.size())
        m_trackIds.push_back(UuidHelper::randomUuid());

    return m_trackIds[trackIndex];
}

} // namespace sample
} // namespace analytics
} // namespace vms_server_plugins
} // namespace nx
