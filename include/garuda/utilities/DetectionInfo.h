#include <string>

namespace garuda
{
  namespace utilities
  {
    struct DetectionInfo
    {
        int class_id{0};
        std::string className{};
        float confidence{0.0};
        cv::Scalar color{};
        cv::Rect box{};
    };
  }
}
