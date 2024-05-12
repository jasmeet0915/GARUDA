#include <string>
#include <filesystem>

#include <object_detection/ObjectDetector.h>
#include <utilities/DetectionInfo.h>

#include <opencv2/opencv.hpp>

using namespace garuda;

int main(int argc, char** argv)
{
    std::unique_ptr<object_detection::ObjectDetector> object_detector =
        std::make_unique<object_detection::ObjectDetector>();
    
    std::filesystem::path model_home_dir = std::filesystem::path("/home/singh/projects/NetworkOptixHackathon/Garuda/models");
    std::string modelPath = (model_home_dir / std::filesystem::path("yolov8s.onnx")).generic_string();
    std::cout << "Loading path from " << modelPath << std::endl;

    object_detector->setModelPath(modelPath);
    std::cout << "Model Path Set" << std::endl;
    object_detector->loadOnnxNetwork();
    std::cout << "Loaded Onnx Model" << std::endl;

    cv::Mat test_image = cv::imread("/home/singh/projects/NetworkOptixHackathon/Garuda/resources/bus.jpg");

    std::vector<utilities::DetectionInfo> detection_results = object_detector->runDetection(test_image);

    return 0;
}