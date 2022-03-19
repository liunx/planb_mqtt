cmake_minimum_required(VERSION 2.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

PROJECT (planb_mqtt)
 
find_package(Protobuf REQUIRED)
if(Protobuf_NOT_FOUND)
    message(FATAL_ERROR "protobuf library is needed but cant be found")
endif()
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(OpenCV REQUIRED)
if(OpenCV_NOT_FOUND)
    message(FATAL_ERROR "protobuf library is needed but cant be found")
endif()
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(MOSQUITTO_LIBRARIES libmosquitto.so)

protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS proto/Message.proto)

ADD_EXECUTABLE(subscriber src/subscriber.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(subscriber ${PROTOBUF_LIBRARIES} ${MOSQUITTO_LIBRARIES} ${OpenCV_LIBRARIES})

ADD_EXECUTABLE(publisher src/publisher.cpp ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(publisher ${PROTOBUF_LIBRARIES} ${MOSQUITTO_LIBRARIES} ${OpenCV_LIBRARIES})

