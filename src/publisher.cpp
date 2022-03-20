#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include <mosquitto.h>
#include "Message.pb.h"
#include "mqtt.hpp"

const char *keys =
    "{host   | localhost | mqtt host/ip to connect }"
    "{port   | 1883 | mqtt port }"
    "{topic  | planb | topic name to bind }"
    "{width | 640 | camera frame width }"
    "{height | 480 | camera frame height }"
    "{cam   |  0     | camera id }";

std::string encode(cv::Mat mat)
{
    planb::Image image;
    std::vector<uchar> imgEncoded;
    cv::imencode(".jpg", mat, imgEncoded);
    image.set_data(imgEncoded.data(), imgEncoded.size());
    std::string data;
    image.SerializeToString(&data);

    return data;
}

int main(int argc, const char **argv)
{
    int ret = 0;
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("publisher");

    std::string host = parser.get<std::string>("host");
    std::string topic = parser.get<std::string>("topic");
    int port = parser.get<int>("port");
    int camId = parser.get<int>("cam");
    int width = parser.get<int>("width");
    int height = parser.get<int>("height");
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    mosquitto_lib_init();

    auto mqtt = Mqtt();
    ret = mqtt.connect(host, port);
    assert(ret == 0);
    ret = mqtt.subscribe(topic);
    assert(ret == 0);
    ret = mqtt.loopStart();
    assert(ret == 0);

    cv::Mat frame;
    cv::VideoCapture cam(camId);
    if (!cam.isOpened()) {
        goto out;
    }
    cam.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cam.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    while (true) {
        cam >> frame;
        auto data = encode(frame);
        mqtt.publish(topic, data.size(), data.c_str());
        if (cv::waitKey(10) == 27) break;
    }

out:
    mqtt.loopStop();
    mosquitto_lib_cleanup();
    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
