project(snippetGenerator LANGUAGES CXX)

add_executable(snippetGenerator
               config.cpp
               snippetGenerator.cpp
               kernel_elf_helper.cpp
               amdgpu-tooling/KernelDescriptor.cpp
               )

target_include_directories(snippetGenerator PRIVATE ${DYNINST_INCLUDE_DIR} ${DYNINST_Boost_INCLUDE_DIRS} ${DYNINST_TBB_INCLUDE_DIRS})
target_compile_definitions(snippetGenerator PRIVATE ${DYNINST_Boost_DEFINITIONS})

target_link_directories(snippetGenerator PRIVATE ${DYNINST_Boost_LIBRARY_DIRS})
target_link_libraries(snippetGenerator
                      dyninstAPI
                      instructionAPI
                      parseAPI
                      symtabAPI
                      dynElf
                      common
                      ${DYNINST_Boost_LIBRARIES}
                      )
