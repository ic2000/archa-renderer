file(GLOB SRC "*.cpp")

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
  set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_compile_options(
  ${PROJECT_NAME}
  PRIVATE -Wall
          -Wextra
          # -Wshadow
          -Wnon-virtual-dtor
          -Wold-style-cast
          -Wunused
          -Woverloaded-virtual
          -Wpedantic
          -Wconversion
          -Wcast-align
          -Wsign-conversion
          -Wmisleading-indentation
          # -Wduplicated-cond -Wduplicated-branches -Wlogical-op
          -Wnull-dereference
          # -Wuseless-cast
          -Wdouble-promotion
          -Wformat=2
          # -Wlifetime
          -Wimplicit-fallthrough
          -O3
          -flto
          # -fexpensive-optimizations
          -march=x86-64
          -msse2
          -mavx2
          -fvectorize
          -ffast-math)

target_link_options(${PROJECT_NAME} PRIVATE -flto)

target_include_directories(
  ${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../inc ${THREAD_POOL_INC}
                          ${TINY_OBJ_LOADER_INC})

target_precompile_headers(
  ${PROJECT_NAME} PRIVATE ${THREAD_POOL_INC}/BS_thread_pool.hpp
  ${glm_SOURCE_DIR}/glm/common.hpp ${glm_SOURCE_DIR}/glm/glm.hpp
  ${glm_SOURCE_DIR}/glm/ext/matrix_transform.hpp)

target_link_libraries(${PROJECT_NAME} PRIVATE ImGui-SFML::ImGui-SFML glm::glm
                                              Tracy::TracyClient)

if(WIN32)
  if(BUILD_SHARED_LIBS)
    add_custom_command(
      TARGET ${PROJECT_NAME}
      POST_BUILD
      COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:ImGui-SFML::ImGui-SFML> $<TARGET_FILE:sfml-graphics>
        $<TARGET_FILE:sfml-window> $<TARGET_FILE:sfml-system>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>)
  endif()
endif()
