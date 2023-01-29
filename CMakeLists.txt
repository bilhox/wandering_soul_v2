cmake_minimum_required(VERSION 3.21)
project(wandering_soul_v2 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(wandering_soul_v2 
src/main.cpp 
src/entity.cpp 
src/collision_utils.cpp 
src/tilemap.cpp 
src/utils.cpp
src/player.cpp
src/animation.cpp
src/asset_manager.cpp
src/spark.cpp
src/particle_system.cpp
src/enemy.cpp
src/text.cpp
src/mana.cpp
)
target_link_libraries(wandering_soul_v2 PRIVATE sfml-graphics sfml-audio)
target_compile_features(wandering_soul_v2 PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET wandering_soul_v2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:wandering_soul_v2> $<TARGET_FILE_DIR:wandering_soul_v2> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS wandering_soul_v2)
