#include "object_detection/ObjectDetector.h"

using namespace garuda::object_detection;
using namespace garuda::utilities;

ObjectDetector::ObjectDetector(const std::string &onnxModelPath, const cv::Size &modelInputShape,
    const std::string &classesTxtFile, const bool &runWithCuda)
{
    this->modelPath = onnxModelPath;
    this->modelShape = modelInputShape;
    this->classesPath = classesTxtFile;
    this->cudaEnabled = runWithCuda;

    // loadClassesFromFile(); The classes are hard-coded for this example
}

void ObjectDetector::setModelPath(std::string modelPath)
{
    this->modelPath = modelPath;
}

std::vector<DetectionInfo> ObjectDetector::runDetection(const cv::Mat &input)
{
    cv::Mat modelInput = input;
    if (letterBoxForSquare && modelShape.width == modelShape.height)
        modelInput = formatToSquare(modelInput);

    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0/255.0, modelShape, cv::Scalar(), true, false);
    net.setInput(blob);

    std::cout << "Created Blob from Input image: " << blob.size << std::endl;

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    std::cout << "Ran Detections" << std::endl;

    int rows = outputs[0].size[1];
    int dimensions = outputs[0].size[2];

    bool yolov8 = false;
    // yolov5 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
    // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
    if (dimensions > rows) // Check if the shape[2] is more than shape[1] (yolov8)
    {
        yolov8 = true;
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];

        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
    }
    float *data = (float *)outputs[0].data;

    float x_factor = modelInput.cols / modelShape.width;
    float y_factor = modelInput.rows / modelShape.height;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {
        if (yolov8)
        {
            float *classes_scores = data+4;

            cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

            if (maxClassScore > modelScoreThreshold)
            {
                confidences.push_back(maxClassScore);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);

                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        else // yolov5
        {
            float confidence = data[4];

            if (confidence >= modelConfidenceThreshold)
            {
                float *classes_scores = data+5;

                cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > modelScoreThreshold)
                {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);

                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);

    std::vector<DetectionInfo> detections{};
    for (unsigned long i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        // Set the class id, class name and confidence of the detection
        DetectionInfo result;
        result.classId = class_ids[idx];
        result.confidence = confidences[idx];
        result.className = classes[result.classId];

        // Set the bounding box of the detected object after normalizing them with image width and height
        float topLeftX = float(boxes[idx].x) / float(modelInput.cols);
        float topLeftY = float(boxes[idx].y) / float(modelInput.rows);
        float width = float(boxes[idx].width) / float(modelInput.cols);
        float height = float(boxes[idx].height) / float(modelInput.rows);

        std::cout << "Bounding Box Coords: " << boxes[idx].x << ", " << boxes[idx].y
            << ", " << boxes[idx].width << ", " << boxes[idx].height << std::endl;
        std::cout << "Image Size: " << modelInput.cols << ", " << modelInput.rows << std::endl;
        std::cout << "Bounding Box Coords: " << topLeftX << ", " << topLeftY
            << ", " << width << ", " << height << std::endl;

        result.boundingBox = nx::sdk::analytics::Rect(topLeftX, topLeftY,
            width, height);

        detections.push_back(result);
    }

    return detections;
}

void ObjectDetector::loadClassesFromFile()
{
    std::ifstream inputFile(classesPath);
    if (inputFile.is_open())
    {
        std::string classLine;
        while (std::getline(inputFile, classLine))
            classes.push_back(classLine);
        inputFile.close();
    }
}

void ObjectDetector::loadOnnxNetwork()
{
    net = cv::dnn::readNetFromONNX(modelPath);
    if (cudaEnabled)
    {
        std::cout << "\nRunning on CUDA" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    else
    {
        std::cout << "\nRunning on CPU" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
}

cv::Mat ObjectDetector::formatToSquare(const cv::Mat &source)
{
    int col = source.cols;
    int row = source.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    source.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}