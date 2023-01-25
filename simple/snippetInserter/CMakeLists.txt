project(snippetInserter LANGUAGES CXX)

add_executable(snippetInserter
               config.cpp
               snippetInserter.cpp
               kernel_elf_helper.cpp
               amdgpu-tooling/KernelDescriptor.cpp
               )

target_include_directories(snippetInserter PRIVATE ${DYNINST_INCLUDE_DIR} ${DYNINST_Boost_INCLUDE_DIRS} ${DYNINST_TBB_INCLUDE_DIRS})
target_compile_definitions(snippetInserter PRIVATE ${DYNINST_Boost_DEFINITIONS})

target_link_directories(snippetInserter PRIVATE ${DYNINST_Boost_LIBRARY_DIRS})
target_link_libraries(snippetInserter
                      dyninstAPI
                      instructionAPI
                      parseAPI
                      symtabAPI
                      dynElf
                      common
                      ${DYNINST_Boost_LIBRARIES}
                      )
