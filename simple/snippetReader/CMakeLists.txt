project(snippetReader LANGUAGES CXX)

add_executable(snippetReader
               config.cpp
               snippetReader.cpp
               kernel_elf_helper.cpp
               amdgpu-tooling/KernelDescriptor.cpp
               )

target_include_directories(snippetReader PRIVATE ${DYNINST_INCLUDE_DIR} ${DYNINST_Boost_INCLUDE_DIRS} ${DYNINST_TBB_INCLUDE_DIRS})
target_compile_definitions(snippetReader PRIVATE ${DYNINST_Boost_DEFINITIONS})

target_link_directories(snippetReader PRIVATE ${DYNINST_Boost_LIBRARY_DIRS})
target_link_libraries(snippetReader
                      dyninstAPI
                      instructionAPI
                      parseAPI
                      symtabAPI
                      dynElf
                      common
                      ${DYNINST_Boost_LIBRARIES}
                      )
