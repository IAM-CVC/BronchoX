cmake_minimum_required(VERSION 3.19)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

include(FetchContent)
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.10.2)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8)
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG v1.9.2
)

PROJECT(BronchoX VERSION 1.2 LANGUAGES CXX)

# set fault options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

# json automatically download from github
FetchContent_GetProperties(json)
if(NOT json_POPULATED)
  FetchContent_Populate(json)
  add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
message(STATUS "${Green} JSON Should Be Downloaded ${ColourReset}")

# glm automatically download from github
FetchContent_GetProperties(glm)
if(NOT glm_POPULATED)
  FetchContent_Populate(glm)
  set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)
  add_subdirectory(${glm_SOURCE_DIR} ${glm_BINARY_DIR})
endif()
message(STATUS "${Green} GLM Should Be Downloaded ${ColourReset}")

# spdlog automatically download from github
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
  FetchContent_Populate(spdlog)
  add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif ()
message(STATUS "${Green} spdlog Should Be Downloaded ${ColourReset}")

# VTK packages
set(VTK_EXPECTED_VERSIOM "9.1.0")

find_package(VTK REQUIRED)
if (NOT VTK_FOUND)
  message("Skipping BronchoX: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()

message (STATUS "VTK_VERSION: ${VTK_VERSION}")
if (VTK_VERSION VERSION_LESS ${VTK_EXPECTED_VERSIOM})
  message("You required a newer version of VTK: at least ${VTK_EXPECTED_VERSIOM}")
  return()
endif()

# Qt5 packages
find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
if (NOT TARGET Qt5::Widgets)
  message("You required Qt5 ${Qt5_NOT_FOUND_MESSAGE}")
  return()
endif()

# Matlab packages
find_package(Matlab REQUIRED ENG_LIBRARY MX_LIBRARY MAT_LIBRARY)

IF(MATLAB_FOUND)
  message(STATUS "MATLAB Found, MATLAB MEX will be compiled.")
ELSE(MATLAB_FOUND)
  MESSAGE("MATLAB not found...nothing will be built.")
ENDIF(MATLAB_FOUND)

# add the sources
file(GLOB SOURCES "./src/*.cpp")
file(GLOB HEADERS "./include/*.h")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(${PROJECT_NAME} MACOSX_BUNDLE
               ${SOURCES}
               ${HEADERS}
               ui/mainview.ui
               ui/dialogDICOM.ui
               ui/style.qrc)

target_compile_definitions(${PROJECT_NAME} PUBLIC MATLAB_MEX_FILE)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ${Matlab_ROOT_DIR}/extern/include
                           ${Matlab_ROOT_DIR}/simulink/include
                           include/
                           PRIVATE external/spdlog/include
                           PRIVATE ${glm_SOURCE_DIR})
if (WIN32)
  target_link_libraries (${PROJECT_NAME}
                         PRIVATE ${VTK_LIBRARIES}
                         ${MATLAB_MX_LIBRARY}
                         ${MATLAB_MAT_LIBRARY}
                         ${Matlab_LIBRARIES}
                         Qt5::Widgets
                         PRIVATE nlohmann_json::nlohmann_json
                         PRIVATE spdlog::spdlog)
elseif (APPLE)
  target_link_libraries(${PROJECT_NAME}
                          PRIVATE ${VTK_LIBRARIES}
                          Qt5::Widgets
                          ${Matlab_ROOT_DIR}/bin/maci64/libmat.dylib
                          ${Matlab_ROOT_DIR}/bin/maci64/libmex.dylib
                          ${Matlab_ROOT_DIR}/bin/maci64/libmx.dylib
                          PRIVATE nlohmann_json::nlohmann_json
                          PRIVATE spdlog::spdlog)
endif ()

# vtk_module_autoinit is needed
vtk_module_autoinit(
        TARGETS BronchoX
        MODULES ${VTK_LIBRARIES}
)
