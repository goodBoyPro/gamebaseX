CMAKE_MINIMUM_REQUIRED(VERSION 3.26.4)
set(CMAKE_CXX_STANDARD 20)
set(PROJECT_NAME game)
project(${PROJECT_NAME})
set(CMAKE_BUILD_TYPE Debug)
add_compile_options(-finput-charset=UTF-8)
########################################################################################
#设置项目绝对路径
set(ABS_DIR ./)

get_filename_component(ABS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${ABS_DIR} ABSOLUTE)
message(RootDirectory:${ABS_DIR})
########################################################################################
#预编译任务
execute_process(
    
    COMMAND ${ABS_DIR}/script/cmakePreTasks.bat
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE SCRIPT_RESULT
)
########################################################################################
SET(CMAKE_CXX_COMPILE_FEATURES "gcc")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexec-charset=UTF-8 ")

include_directories(
    ${ABS_DIR}/Engine/ExternLibrary/SFML/include
    ${ABS_DIR}/Engine/ExternLibrary/include
    ${ABS_DIR}/Engine/gameFramework/public
    ${ABS_DIR}/build/generatedCodeFile
)

#自定义设置
# set(SFML_DIR ../extern/sfml/lib/cmake/SFML)
# find_package(SFML COMPONENTS window system graphics network audio REQUIRED)
add_definitions(-DUNICODE )  #预定义前需要加-D

set(CMAKE_BUILD_RPATH ${ABS_DIR}/build)

#启动程序
set(EXECUTABLE_OUTPUT_PATH ${ABS_DIR}/output)
add_executable(Start start.cpp version.rc)

#Game
set(exeName Game)
file(GLOB_RECURSE src 
    ${ABS_DIR}/Engine/gameFramework/*.cpp
    version.rc)
add_executable(${exeName}  ${src})
set_target_properties(${exeName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABS_DIR}/output/bin)
target_link_directories(${exeName} PRIVATE ${ABS_DIR}/Engine/ExternLibrary/SFML/lib)
target_link_libraries(${exeName} opengl32 sfml-graphics sfml-window sfml-system sfml-audio  winmm user32)
target_compile_definitions(${exeName} PRIVATE -DEDITOR)

#mainTest
set(exeName mainTest)
file(GLOB_RECURSE src 
    ${ABS_DIR}/cpptest/*.cpp
    )
add_executable(${exeName}  ${src})
set_target_properties(${exeName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABS_DIR}/output/bin)
target_link_directories(${exeName} PRIVATE ${ABS_DIR}/Engine/ExternLibrary/SFML/lib)
target_link_libraries(${exeName} opengl32 sfml-graphics sfml-window sfml-system sfml-audio  winmm)