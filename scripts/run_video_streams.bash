#!/bin/bash

# Paths to your video files
VIDEO_PATH1="$PROJECT_MEDIA_PATH/Birds_feeding.mkv"
VIDEO_NAME1="Birds_feeding"
VIDEO_PATH2="$PROJECT_MEDIA_PATH/Buffalo.mkv"
VIDEO_NAME2="Buffalo"
VIDEO_PATH3="$PROJECT_MEDIA_PATH/Cattle_Drone_Footage.mp4"
VIDEO_NAME3="Cattle_Drone_Footage"
VIDEO_PATH4="$PROJECT_MEDIA_PATH/Elephant.mkv"
VIDEO_NAME4="Elephant"
VIDEO_PATH5="$PROJECT_MEDIA_PATH/Hunter.mp4"
VIDEO_NAME5="Hunter"


# Name of the tmux session
SESSION_NAME="video_streams"

# Create a new tmux session and run the RTSP server in the first window
tmux new-session -d -s $SESSION_NAME -n "RTSP_Server"
tmux send-keys -t $SESSION_NAME:0 "docker run --rm -it --network=host bluenviron/mediamtx:latest" C-m

# Create a new window for video streams
tmux new-window -t $SESSION_NAME -n "Video_Streams"

# Sleep for 2 seconds and allow the server to start
sleep 2

# Split the window into 4 panes
tmux split-window -h
tmux split-window -v
tmux select-pane -t 0
tmux split-window -v

# Start the virutal environment and Launch the Python scripts in each pane
tmux send-keys -t 0 "ffmpeg -re -stream_loop -1 -i $VIDEO_PATH1 -c copy -f rtsp rtsp://localhost:8554/$VIDEO_NAME1" C-m
tmux send-keys -t 1 "ffmpeg -re -stream_loop -1 -i $VIDEO_PATH2 -c copy -f rtsp rtsp://localhost:8554/$VIDEO_NAME2" C-m
tmux send-keys -t 2 "ffmpeg -re -stream_loop -1 -i $VIDEO_PATH3 -c copy -f rtsp rtsp://localhost:8554/$VIDEO_NAME3" C-m
tmux send-keys -t 3 "ffmpeg -re -stream_loop -1 -i $VIDEO_PATH4 -c copy -f rtsp rtsp://localhost:8554/$VIDEO_NAME4" C-m

# Create a new window for video streams
tmux new-window -t $SESSION_NAME -n "Video_Streams"

# Sleep for 2 seconds and allow the server to start
sleep 2

# Split the window into 4 panes
tmux split-window -h
tmux split-window -v
tmux select-pane -t 0
tmux split-window -v

# Start the virutal environment and Launch the Python scripts in each pane
tmux send-keys -t 0 "ffmpeg -re -stream_loop -1 -i $VIDEO_PATH5 -c copy -f rtsp rtsp://localhost:8554/$VIDEO_NAME5" C-m

# Attach to the tmux session
tmux attach-session -t $SESSION_NAME