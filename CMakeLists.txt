cmake_minimum_required(VERSION 3.10)
project(hash_storage) #дает имя проекту

find_package(absl CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROG_LIBRARY "${PROJECT_NAME}") 

#сораняем путь до директории include нашими хедарами 
set(PROG_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/src/include")
set(PROG_CLASSES "${CMAKE_CURRENT_SOURCE_DIR}/src/classes")
set(PROG_INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/src/interface")

#добавляет путь до нашей директории в область видимости всех проектов
include_directories(${PROG_INCLUDE})
include_directories(${PROG_CLASSES})
include_directories(${PROG_INTERFACE})
 
add_library(hash_map STATIC ${PROG_CLASSES}/hash_map.hpp ${PROG_CLASSES}/storage.hpp)
add_library(RBTree INTERFACE)
target_include_directories(RBTree INTERFACE ${PROG_CLASSES} ${PROG_INCLUDE})

set_target_properties(hash_map PROPERTIES LINKER_LANGUAGE CXX)
#добавляем директории наших подпроектов в область видимости CMake 
add_executable(main src/main.cpp src/Interface/controller.cpp)
target_link_libraries(main PRIVATE RBTree absl::statusor)