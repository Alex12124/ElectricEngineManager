cmake_minimum_required(VERSION 3.23)
project(device VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/third_party/AsyncElectricEngineLib/" )

find_package(Boost 1.88 COMPONENTS system coroutine REQUIRED)

find_package(protobuf REQUIRED)

find_package(Qt6 COMPONENTS Widgets Sql Concurrent Core REQUIRED)


set(PROJECT_SOURCES
        main.cpp
        navigation.h
        navigation.cpp
        modelmanager.h
        modelmanager.cpp
        server/server.h
        server/server.cpp
        server/session.h
        server/session.cpp
        server/synchronized_queue.h
        pages/page.h
        pages/page.cpp
        pages/mainwindow.cpp
        pages/mainwindow.h
        pages/mainwindow.ui
        pages/menu.h
        pages/menu.cpp
        pages/menu.ui
        pages/engine/settings/settings.h
        pages/engine/settings/settings.cpp
        pages/engine/settings/settings.ui
        pages/engine/prepare_start/preparestart.h
        pages/engine/prepare_start/preparestart.cpp
        pages/engine/prepare_start/preparestart.ui
        pages/engine/add_engine/add_engine.h
        pages/engine/add_engine/add_engine.cpp
        pages/engine/add_engine/add_engine.ui
        pages/launch_log/launch_log.h
        pages/launch_log/launch_log.cpp
        pages/launch_log/launch_log.ui
        pages/remote/remote.h
        pages/remote/remote.cpp
        pages/remote/remote.ui
        pages/remote/remote_run_engine.h
        pages/remote/remote_run_engine.cpp
        pages/remote/remote_run_engine.ui
        model/engine/engine_model.h
        model/engine/engine_model.cpp
        model/engine/engine.h
        model/engine/engine.cpp
        model/launch/launch.h
        model/launch/launch.cpp
        model/engine/engine_dao.h
        model/engine/engine_dao.cpp
        model/launch/launch_dao.h
        model/launch/launch_dao.cpp
        model/launch/launch_model.h
        model/launch/launch_model.cpp
        resources.qrc
)


qt_add_executable(device
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

)


set(PROTO_FILES
        "${CMAKE_CURRENT_SOURCE_DIR}/proto/start.proto"
        "${CMAKE_CURRENT_SOURCE_DIR}/proto/stop.proto"
        "${CMAKE_CURRENT_SOURCE_DIR}/proto/message.proto"
)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES}
        IMPORT_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/proto/")
add_library(protobuf_generated STATIC ${PROTO_SRCS} ${PROTO_HDRS})
target_link_libraries(protobuf_generated PRIVATE protobuf::protobuf)


target_link_libraries(device PRIVATE
        Qt6::Widgets
        Qt6::Sql
        Qt6::Concurrent
        Qt6::Core
        ${Boost_LIBRARIES}
        protobuf_generated
        protobuf::protobuf
        async_electric_engine
)



qt_finalize_executable(device)

target_include_directories(device PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

