cmake_minimum_required(VERSION 3.21)
project(platformer LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

add_executable(platformer 
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
target_link_libraries(platformer PRIVATE sfml-graphics sfml-audio)
target_compile_features(platformer PRIVATE cxx_std_17)
if (WIN32 AND BUILD_SHARED_LIBS)
    add_custom_command(TARGET platformer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:platformer> $<TARGET_FILE_DIR:platformer> COMMAND_EXPAND_LISTS)
endif()

install(TARGETS platformer)
