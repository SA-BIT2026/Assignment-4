add_library(a4lib A4Lib.cpp)

add_executable(andersen Andersen.cpp)
target_link_libraries(andersen PRIVATE
        ${SVF_LIB}
        ${LLVM_LIB}
        a4lib
        )
set_target_properties(andersen PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})