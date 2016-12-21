cmake_minimum_required(VERSION 3.0)
project(RocksDBBenchmark)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(jemalloc REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

#list(APPEND CMAKE_CXX_FLAGS "-fprofile-arcs -ftest-coverage -DNDEBUG ${CMAKE_CXX_FLAGS}")
#list(APPEND CMAKE_EXE_LINKER_FLAGS "-O0 -g -fprofile-arcs -ftest-coverage -DNDEBUG ${CMAKE_EXE_LINKER_FLAGS}")

add_library(qdigest INTERFACE)
target_include_directories(qdigest INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/qdigest
)

add_library(rocksdb INTERFACE)
target_include_directories(rocksdb INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rocksdb/include
)

add_library(velocypack INTERFACE)
target_include_directories(velocypack INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/velocypack/include
)

add_library(benchmark INTERFACE)
target_include_directories(benchmark INTERFACE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(RocksDBBenchmark STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/HybridLogicalClock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/Mutex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/MutexLocker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/PrimaryIndex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/RandomNumber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/RocksDBInstance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/SecondaryIndex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/StorageEngine.cpp
)
target_link_libraries(RocksDBBenchmark PRIVATE
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rocksdb/librocksdb.a
  ${ZLIB_LIBRARIES}
  ${BZIP2_LIBRARIES}
  snappy
  jemalloc
  -ldl
  rocksdb
  qdigest
  velocypack
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/velocypack/build/libvelocypack.a
  benchmark
)

add_executable(rocksdb-benchmark
  benchmark/rocksdb-benchmark.cpp
)
target_link_libraries(rocksdb-benchmark PRIVATE
  RocksDBBenchmark
)
