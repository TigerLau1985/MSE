include_directories(. inc)
aux_source_directory(src SRC_DIRS)

file(GLOB_RECURSE DISRUPTOR_LIST "disruptor/*.h")
file(GLOB_RECURSE INC_LIST "inc/*.h")
source_group("Disruptor" FILES ${DISRUPTOR_LIST})
source_group("Inc" FILES ${INC_LIST})

add_library(MseBase ${SRC_DIRS} ${DISRUPTOR_LIST} ${INC_LIST})