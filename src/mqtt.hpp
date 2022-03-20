#ifndef MQTT_HPP   
#define MQTT_HPP 
#include <iostream>
#include <string>
#include <mosquitto.h>

class Mqtt {
public:
    Mqtt()
    {
        mosq_ = mosquitto_new(NULL, true, NULL);
        assert(mosq_ != NULL);
    }

    ~Mqtt()
    {
        mosquitto_destroy(mosq_);
    }

    int connect(const std::string host, const int port, const int keepalive=60)
    {
        int rc = mosquitto_connect(mosq_, host.c_str(), port, keepalive);
        if (rc != MOSQ_ERR_SUCCESS) {
            std::cerr << "Error: " << mosquitto_strerror(rc) << std::endl;
        }
        return rc;
    }

    int publish(std::string &topic, int len, const void *data)
    {
        int rc = 0;
        rc = mosquitto_publish(mosq_, nullptr, topic.c_str(), len, data, 0, false);
        if (rc != MOSQ_ERR_SUCCESS)
        {
            fprintf(stderr, "Error publishing: %s\n", mosquitto_strerror(rc));
        }
        return rc;
    }

    int subscribe(const std::string topic)
    {
        int rc = 0;
        rc = mosquitto_subscribe(mosq_, NULL, topic.c_str(), 1);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Error subscribing: %s\n", mosquitto_strerror(rc));
        }
        return rc;
    }

    void setConnectCallback(void (*on_connect)(struct mosquitto *, void *, int))
    {
        mosquitto_connect_callback_set(mosq_, on_connect);
    }

    void setPublishCallback(void (*on_publish)(struct mosquitto *, void *, int))
    {
        mosquitto_publish_callback_set(mosq_, on_publish);
    }

    void setSubscribeCallback(void (*on_subscribe)(struct mosquitto *, void *, int, int, const int *))
    {
        mosquitto_subscribe_callback_set(mosq_, on_subscribe);
    }

    void setMessageCallback(void (*on_message)(struct mosquitto *, void *, const struct mosquitto_message *))
    {
        mosquitto_message_callback_set(mosq_, on_message);
    }

    void loopForever()
    {
        mosquitto_loop_forever(mosq_, -1, 1);
    }

    int loopStart()
    {
        int rc = 0;
        rc = mosquitto_loop_start(mosq_);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
        }
        return rc;
    }

    int loopStop()
    {
        int rc = 0;
        rc = mosquitto_loop_stop(mosq_, true);
        if (rc != MOSQ_ERR_SUCCESS) {
            fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
        }
        return rc;
    }

private:
    struct mosquitto *mosq_;
};

#endif
