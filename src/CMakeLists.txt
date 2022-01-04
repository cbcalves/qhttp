set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)

include_directories(../3rdparty)

set(SOURCES
    qhttpabstracts.hpp
    qhttpabstracts.cpp
    qhttpserverconnection.hpp
    qhttpserverconnection.cpp
    qhttpserverrequest.hpp
    qhttpserverrequest.cpp
    qhttpserverresponse.hpp
    qhttpserverresponse.cpp
    qhttpserver.hpp
    qhttpserver.cpp
    qhttpfwd.hpp
    qhttpclient.cpp
    qhttpclient.hpp
    qhttpclientrequest.cpp
    qhttpclientrequest.hpp
    qhttpclientresponse.cpp
    qhttpclientresponse.hpp
)

add_library(${PROJECT_NAME}
    SHARED
    ${SOURCES}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    3rdparty
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Network
)
