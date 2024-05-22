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

Nx Desktop client is also powered by a React frontend that displays a national park map showing the last known locations of different animals. Location data from cameras is sent to the server, which then updates the React frontend. The locations can be inferred using GPS coordinates. Animal-type data is sent from Nx Data Metadata SDK Plugin.


<img src="https://github.com/jasmeet0915/GARUDA/assets/12881364/d3e2f21a-9eed-4eba-81e7-adb2bacb2492" width=550px height=520px/>


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

1. <b>For Tourists</b>
- Finding Animals: Use the map to locate the last known position of the animals you are interested in. Click on the animal icons for more details.
- Watching Live Feeds: Switch to the live stream window to watch real-time footage of animals in their natural habitat.
2. <b>For Rangers</b>
- Monitoring Wildlife: Keep track of animal locations and movements using the map view which can be used in case of injured animals.
- Poacher Alerts: Receive real-time notifications about potential poachers.

## Demo Screenshots

![Screenshot from 2024-05-21 00-27-53](https://github.com/jasmeet0915/GARUDA/assets/23265149/66d56f11-c345-4cd1-886b-f968601d7529)

![Screenshot from 2024-05-23 01-27-22](https://github.com/jasmeet0915/GARUDA/assets/23265149/41053cc2-04f4-40f2-8f2d-ab6c77a12ad9)

![Screenshot from 2024-05-23 01-28-32](https://github.com/jasmeet0915/GARUDA/assets/23265149/d0598e31-aff4-489b-af12-641d372fa4f3)

![Screenshot from 2024-05-23 01-29-40](https://github.com/jasmeet0915/GARUDA/assets/23265149/5e0f2c8a-db81-45e4-a2eb-21250481f6e4)


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
The project relies on the source build of OpenCV version >= 4.7. Instructions for the same can be found [here](https://docs.opencv.org/4.x/d7/d9f/tutorial_linux_install.html).
  > **Note:** Make sure to use flags: `WITH_ONNX`, `WITH_CUDA` while configuring the build with cmake and set the `CMAKE_INSTALL_PREFIX` to the install location of nx-meta mediaserver (it is generally `/opt/networkoptix-metavms/mediaserver`).
    This would allow Nx Mediaserver to find OpenCV's shared libraries.
After building & installing OpenCV, you can start building the projects with following instructions:
* Clone the project:
  ```bash
  git clone https://github.com/jasmeet0915/GARUDA.git
  ```
* Navigate to the directory and create a build directory:
  ```bash
  cd GARUDA && mkdir build
  ```
* Configure the build using cmake:
  ```bash
  cmake -DmetadataSdkDir=$NX_METADATA_SDK -B ./build
  ```
  > **Note:** Here $NX_METADATA_SDK is an environment variable with the path where the metadata sdk is installed.
* Finally build the project:
  ```bash
  sudo cmake --build ./build --target install --config Release -- -j
  ```
  This command will build the required plugin shared library and install it in the `/bin/plugins/` directory of the mediaserver automatically.
 * After this, restart the mediaserver systemctl service:
   ```bash
   sudo systemctl restart networkoptix-metavms-mediaserver.service
   ```
 * Start the frontend and video streams by executing the provided script:
   ```bash
   cd GARUDA/scripts && ./start_garuda.bash
   ```
   > **Note:** This command would require videos to be installed in `resources/videos` path and the $PROJECT_MEDIA_PATH environment variable to be set. Update the names of the videos accordingly in the script.
 * Start the NX Meta Desktop Client and enjoy!

# Video
[![Watch the video](https://img.youtube.com/vi/XK4F4SSR39w/maxresdefault.jpg)](https://youtu.be/XK4F4SSR39w)

