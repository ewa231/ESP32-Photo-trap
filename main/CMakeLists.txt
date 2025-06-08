# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

set(sources "source/main.cpp")

idf_component_register(
    SRCS ${sources}        # list the source files of this component
    INCLUDE_DIRS "include" # public include directories
)
