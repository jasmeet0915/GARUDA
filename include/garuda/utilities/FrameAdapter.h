#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>

#include <nx/sdk/analytics/i_uncompressed_video_frame.h>
#include <nx/kit/debug.h>


namespace garuda
{
  namespace utilities
  {
    /**
     * Stores frame data and cv::Mat. Note, there is no copying of image data in the constructor.
     */
    struct FrameAdapter
    {
      int width;
      int height;
      int64_t timestampUs;
      int64_t index;
      cv::Mat cvImg;

    public:
      void frameToCvImg(const nx::sdk::analytics::IUncompressedVideoFrame* frame)
      {
        width = frame->width();
        height = frame->height();
        timestampUs = frame->timestampUs();
        cvImg = cv::Mat(height, width, CV_8UC3,
          (void*) frame->data(0), (size_t) frame->lineSize(0));
      }
    };
  }
}
