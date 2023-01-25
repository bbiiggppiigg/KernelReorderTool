cmake_minimum_required(VERSION 3.20)

project(amdgpu-tooling LANGUAGES CXX)

set(CMAKE_MODULE_PATH
  "${Dyninst_DIR}"
  "${Dyninst_DIR}/Modules"
  ${CMAKE_MODULE_PATH}
)

# Import the Dyninst components
find_package(Dyninst REQUIRED COMPONENTS symtabAPI dynElf)

# Read the cache generated from building Dyninst
load_cache(${Dyninst_DIR}
  READ_WITH_PREFIX DYNINST_
  Boost_LIBRARIES
  Boost_INCLUDE_DIRS
  Boost_LIBRARY_DIRS
  Boost_DEFINES
  TBB_INCLUDE_DIRS
)

add_library(amdgpu-tooling
  KernelDescriptor.cpp
  KdUtils.cpp
)

target_include_directories(amdgpu-tooling PRIVATE
  ${DYNINST_INCLUDE_DIR}
  ${DYNINST_Boost_INCLUDE_DIRS}
  ${DYNINST_TBB_INCLUDE_DIRS}

  ${CMAKE_CURRENT_SOURCE_DIR}/third-party/msgpack/include
)

target_link_libraries(amdgpu-tooling
  dynElf
  symtabAPI
)

target_compile_definitions(amdgpu-tooling PRIVATE ${DYNINST_Boost_DEFINITIONS})
target_link_directories(amdgpu-tooling PRIVATE ${DYNINST_Boost_LIBRARY_DIRS})


# tool
add_executable(kd-tool
               kd-tool.cpp)

target_include_directories(kd-tool PRIVATE
  ${DYNINST_INCLUDE_DIR}
  ${DYNINST_Boost_INCLUDE_DIRS}
  ${DYNINST_TBB_INCLUDE_DIRS}
)

target_compile_definitions(kd-tool PRIVATE ${DYNINST_Boost_DEFINITIONS})
target_link_directories(kd-tool PRIVATE ${DYNINST_Boost_LIBRARY_DIRS})

target_link_libraries(kd-tool
  amdgpu-tooling
  symtabAPI
  dynElf
  ${DYNINST_Boost_LIBRARIES}
)
