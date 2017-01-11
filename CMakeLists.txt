cmake_minimum_required(VERSION 3.0)
project(RocksDBBenchmark)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

find_package(jemalloc REQUIRED)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
find_package(BZip2 REQUIRED)

set(CMAKE_CXX_FLAGS "-O3 -fpeel-loops -g -DNDEBUG ${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "-O3 -fpeel-loops -g -DNDEBUG ${CMAKE_EXE_LINKER_FLAGS}")

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rocksdb/include
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/velocypack/include
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

add_library(RocksDBBenchmark OBJECT
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/ArangoComparator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/ArangoPrefixTransform.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/BatchInsert.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/Database.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/HotsetLookup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/HybridLogicalClock.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/Mutex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/MutexLocker.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/PrimaryIndex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/RandomLookup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/RandomNumber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/RangeLookup.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/RocksDBInstance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/SecondaryIndex.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/StorageEngine.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/Suite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/UsageMonitor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/utility.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/benchmark/Workload.cpp
)

set(LIBRARY_LIST
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/q-digest/build/libqdigest.so
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/velocypack/build/libvelocypack.so
  ${CMAKE_THREAD_LIBS_INIT}
  ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/rocksdb/librocksdb.so
  ${ZLIB_LIBRARIES}
  ${BZIP2_LIBRARIES}
  snappy
  jemalloc
  -ldl
)

add_executable(StorageEngineTest
  $<TARGET_OBJECTS:RocksDBBenchmark>
  tests/StorageEngineTest.cpp
)
target_link_libraries(StorageEngineTest PRIVATE ${LIBRARY_LIST})

add_executable(PrimaryIndexTest
  $<TARGET_OBJECTS:RocksDBBenchmark>
  tests/PrimaryIndexTest.cpp
)
target_link_libraries(PrimaryIndexTest PRIVATE ${LIBRARY_LIST})

add_executable(SecondaryIndexTest
  $<TARGET_OBJECTS:RocksDBBenchmark>
  tests/SecondaryIndexTest.cpp
)
target_link_libraries(SecondaryIndexTest PRIVATE ${LIBRARY_LIST})

add_executable(DatabaseTest
  $<TARGET_OBJECTS:RocksDBBenchmark>
  tests/DatabaseTest.cpp
)
target_link_libraries(DatabaseTest PRIVATE ${LIBRARY_LIST})

add_executable(TransactionTest
  $<TARGET_OBJECTS:RocksDBBenchmark>
  tests/TransactionTest.cpp
)
target_link_libraries(TransactionTest PRIVATE ${LIBRARY_LIST})

add_executable(rocksdb-benchmark
  $<TARGET_OBJECTS:RocksDBBenchmark>
  benchmark/rocksdb-benchmark.cpp
)
target_link_libraries(rocksdb-benchmark PRIVATE ${LIBRARY_LIST})
