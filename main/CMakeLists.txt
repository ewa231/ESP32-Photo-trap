# See the build system documentation in IDF programming guide
# for more information about component CMakeLists.txt files.

set(sources "source/main.c")

idf_component_register(
    SRCS ${sources}        # list the source files of this component
    INCLUDE_DIRS "include" # public include directories
    PRIV_INCLUDE_DIRS   # optional, add here private include directories
    REQUIRES            # optional, list the public requirements (component names)
    PRIV_REQUIRES       # optional, list the private requirements
)
