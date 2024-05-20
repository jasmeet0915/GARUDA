# GARUDA-Nx-Evos-Hackathon
GARUD: Geo-spatial Animal Reckon User Dashboard

# Problem
Tourism in national parks offers a chance to observe wildlife and nature up close. However, there are many challenges faced by rangers, tourists, and researchers.

1. Tourists: Need to know the last known location of the animals they’ve traveled to see.
2. Researchers: Require data for locating animals and tracking their daily habits.
3. Rangers: Need alerts about injured animals or poachers hunting illegally.

# Solution
GARUD relies on drones and cameras across the national park for camera streams. RTSP streams from these cameras are accessed on the Nx Meta Desktop Client.  This desktop client has a plugin developed with the Nx Metadata SDK, which enables:

- Wildlife Detection: Utilizing YOLO v8 and OpenCV.
- Poaching Alerts: Triggered by event metadata when a poacher is detected.

The object metadata and event metadata are exposed to the Nx Server, which is finally shown on the Nx Desktop client.

# Highlights for users
Nx Desktop Client addresses challenges for three types of users: rangers, tourists, and researchers. It offers the following features:

- Locating Wildlife on the National Park Map: View the last known locations of various animals on an interactive map.
- Real-Time Stream for Detected Wildlife: Watch live streams of wildlife detected by cameras and drones.
- Poacher Alerts: Receive real-time notifications about detected poachers.

Future Enhancements:
- Water Tracking and Analytics: Monitor water sources like ponds and lakes to track animal activity and water levels.
- Feeder Control: An automated feeder system controlled via Nx Event metadata to assist in feeding animals.

## Interface Overview
- Map View: Shows a map of the national park with icons representing the last known locations of different animals.
- Live Stream: Displays real-time footage from drones and cameras.
- Notification Window: Alerts for poachers or other significant events appear here.

1. For Tourists
- Finding Animals: Use the map to locate the last known position of the animals you are interested in. Click on the animal icons for more details.
- Watching Live Feeds: Switch to the live stream window to watch real-time footage of animals in their natural habitat.
2. For Rangers
- Monitoring Wildlife: Keep track of animal locations and movements using the map view which can be used in case of injured animals.
- Poacher Alerts: Receive real-time notifications about potential poachers.

<Picture here>

# Technologies used
1. Nx Meta Desktop Client
2. Nx Metadata SDK
3. Nx Meta MVS Media Server
4. YOLOv8
5. OpenCV: C++
6. RTSP server
7. FFMPEG
8. React Frontend

# Build & Run

# Video

