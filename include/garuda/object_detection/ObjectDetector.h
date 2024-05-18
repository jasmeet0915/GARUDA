#pragma once

#include <fstream>
#include <vector>
#include <string>
#include <random>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include <utilities/DetectionInfo.h>
#include <nx/sdk/analytics/rect.h>


using namespace garuda::utilities;

namespace garuda
{
  namespace object_detection
  {
    class ObjectDetector
    {
      public:
        ObjectDetector() = default;
        ObjectDetector(const std::string &onnxModelPath, const cv::Size &modelInputShape,
          const std::string &classesTxtFile, const bool &runWithCuda);

        void setModelPath(std::string modelPath);
        void loadOnnxNetwork();

        std::vector<DetectionInfo> runDetection(const cv::Mat &input);

      private:
        void loadClassesFromFile();
        cv::Mat formatToSquare(const cv::Mat &source);

        std::string modelPath{""};
        std::string classesPath{""};
        bool cudaEnabled{true};

        std::vector<std::string> classes{"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"};

        cv::Size2f modelShape{{480, 640}};

        float modelConfidenceThreshold {0.25};
        float modelScoreThreshold      {0.45};
        float modelNMSThreshold        {0.50};

        bool letterBoxForSquare = true;

        cv::dnn::Net net;
    };
  }
}
