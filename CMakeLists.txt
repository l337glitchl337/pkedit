cmake_minimum_required(VERSION 3.10)
project(pkedit)

# All sources
set(SRCS
  src/main.c
  src/charset.c
  src/items.c
  src/savinfo.c
  src/species.c
  src/editsav.c
  src/helpers.c
  src/pkmnstructs.c
)

add_executable(pkedit ${SRCS})
target_link_libraries(pkedit m)
# Where to find headers like "foo.h"
target_include_directories(pkedit PRIVATE src)
