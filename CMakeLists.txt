project(thread_safe_queue)
cmake_minimum_required(VERSION 3.7)

set(INCLUDE_DIR /usr/local/include)
install(FILES include/ts_queue.hpp DESTINATION ${INCLUDE_DIR})