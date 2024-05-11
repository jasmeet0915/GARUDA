#pragma once

#include <string>

#include <nx/sdk/analytics/rect.h>

namespace garuda
{
  namespace utilities
  {
    struct DetectionInfo
    {
      int classId{0};
      std::string className{};
      float confidence{0.0};
      nx::sdk::analytics::Rect boundingBox;
    };
  }
}
