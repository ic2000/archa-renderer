include(FetchContent)

set(SFML_VERSION 2.6.1)

FetchContent_Declare(
  sfml URL https://github.com/SFML/SFML/archive/refs/tags/${SFML_VERSION}.zip)

add_subdirectory(sfml)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG 277ae93c41314ba5f4c7444f37c4319cdf07e8cf # 1.90.4
)

FetchContent_MakeAvailable(imgui)

FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml
  GIT_TAG bf9023d1bc6ec422769559a5eff60bd00597354f # 2.6.x
)

add_subdirectory(imgui-sfml)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG bf71a834948186f4097caa076cd2663c69a10e1e # refs/tags/1.0.1
)

add_subdirectory(glm)

FetchContent_Declare(
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG v0.11.1
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE)

add_subdirectory(tracy)

set(THREAD_POOL_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/thread-pool/include
    PARENT_SCOPE)

set(TINY_OBJ_LOADER_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/tinyobjloader
    PARENT_SCOPE)

