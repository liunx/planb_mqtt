#include <iostream>
#include <string>
#include <assert.h>
#include <thread>
#include <vector>
#include <chrono>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgcodecs/legacy/constants_c.h>
#include <mosquitto.h>
#include "Message.pb.h"
#include "mqtt.hpp"

const char *keys =
    "{host   | localhost | mqtt host/ip to connect }"
    "{port   | 1883 | mqtt port }"
    "{topic  | planb | topic name to bind }";

cv::Mat decode(const char *payload, int len)
{
    planb::Image image;
    image.ParseFromArray(payload, len);
    std::vector<uchar> data;
    auto mesgData = image.data();
    for (int i = 0; i < image.ByteSize(); i++)
        data.push_back(mesgData[i]);
    return cv::imdecode(data, CV_LOAD_IMAGE_COLOR);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    auto image = decode((const char *)msg->payload, msg->payloadlen);
    cv::imshow("show", image);
    cv::waitKey(1);
}

int main(int argc, const char **argv)
{
    int ret = 0;
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("subscriber");

    std::string host = parser.get<std::string>("host");
    std::string topic = parser.get<std::string>("topic");
    int port = parser.get<int>("port");
    mosquitto_lib_init();

    cv::namedWindow("show",cv::WINDOW_NORMAL);
    auto mqtt = Mqtt();
    mqtt.setMessageCallback(on_message);
    ret = mqtt.connect(host, port);
    assert(ret == 0);
    ret = mqtt.subscribe(topic);
    assert(ret == 0);

    ret = mqtt.loopStart();
    assert(ret == 0);
    for (int i{0}; i < 1000; i++) {
        std::this_thread::sleep_for (std::chrono::seconds(1));
    }
    mqtt.loopStop();

    mosquitto_lib_cleanup();
    return 0;
}