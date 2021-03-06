#
# MegaMol™ Console Front End
# Copyright 2017, by MegaMol TEAM
# Alle Rechte vorbehalten. All rights reserved.
#
option(BUILD_CONSOLE "build console" ON)

if(BUILD_CONSOLE)
  project(${CONSOLE_NAME})
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

  # Check option dependencies: First list item is always the current build option,
  # afterwards list the dependencies.
  set(DEP_LIST "${DEP_LIST};BUILD_MMSTD_DATATOOLS_PLUGIN BUILD_CORE" CACHE INTERNAL "")

  file(GLOB_RECURSE header_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.h")
  file(GLOB_RECURSE source_files RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

  # Add target
  add_executable(${PROJECT_NAME} ${header_files} ${source_files})
  set_target_properties(${PROJECT_NAME}
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_NAME ${PROJECT_NAME})
  target_include_directories(${PROJECT_NAME} PRIVATE "src" "include")
  target_link_libraries(${PROJECT_NAME} PRIVATE core)

  if(WIN32)
    set(VERSION_LIBRARY "Version.lib")
  endif()

  # GLFW
  if(USE_GLFW)
    if (MSVC)
        set(GLFW_IMPORT_LIBRARY "lib/glfw3dll.lib")
        set(GLFW_LIBRARY "lib/glfw3.dll")
    else()
        set(GLFW_IMPORT_LIBRARY "")
        set(GLFW_LIBRARY "lib/libglfw.so")
    endif()
    add_external_project(glfw_ext
      GIT_REPOSITORY https://github.com/glfw/glfw.git
      GIT_TAG "3.2.1"
      BUILD_BYPRODUCTS "<INSTALL_DIR>/${GLFW_IMPORT_LIBRARY}"
      CMAKE_ARGS
        -DBUILD_SHARED_LIBS=ON
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF)
    add_external_library(glfw3 SHARED
      DEPENDS glfw_ext
      INCLUDE_DIR "include"
      IMPORT_LIBRARY ${GLFW_IMPORT_LIBRARY}
      LIBRARY ${GLFW_LIBRARY})
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw3)
    ExternalProject_Get_property(glfw_ext SOURCE_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE "${SOURCE_DIR}/include")
  endif()

  target_link_libraries(${PROJECT_NAME} PRIVATE ${VERSION_LIBRARY} ${CMAKE_DL_LIBS})

  # Grouping in Visual Studio
  set_target_properties(${CONSOLE_NAME} PROPERTIES FOLDER base)
  source_group("Header Files" FILES ${header_files})
  source_group("Source Files" FILES ${source_files})

  # install mmconsole executable
  install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "bin" ARCHIVE DESTINATION "lib")
  if(USE_GLFW)
    install_external(TARGETS glfw3)
  endif()
  if (WIN32)
    if(MSVC)
      install(FILES $<TARGET_PDB_FILE:${PROJECT_NAME}> DESTINATION bin OPTIONAL)
    endif()
  endif ()


  # Configuration of the megamol.cfg megamol.sh remoteconsole.sh files
  set(cfg_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib")

  set(cfg_MEGAMOLCON "${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}")

  if(UNIX)
    include(GNUInstallDirs)
    set(cfg_EXTERNAL_LIB_PATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    set(MEGAMOL_SHELL_START_SCRIPT "megamol.sh")
    configure_file(extra/megamol.sh.in ${CMAKE_BINARY_DIR}/${MEGAMOL_SHELL_START_SCRIPT} @ONLY)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${MEGAMOL_SHELL_START_SCRIPT} DESTINATION "bin")
  endif()

  set(MMPLG_DIR "lib")
  if(WIN32)
    set(MMPLG_DIR "bin")
  endif()

  configure_file(extra/megamolconfig.lua.in ${CMAKE_BINARY_DIR}/megamolconfig.lua)
  configure_file(extra/install_megamolconfig_lua.cmake.in ${CMAKE_BINARY_DIR}/install_megamolconfig_lua.cmake @ONLY)

  install(SCRIPT ${CMAKE_BINARY_DIR}/install_megamolconfig_lua.cmake)
endif()
