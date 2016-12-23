#ifndef ROCKSDB_BENCHMARK_SUITE_H
#define ROCKSDB_BENCHMARK_SUITE_H 1

#include <benchmark/BatchInsert.h>
#include <benchmark/Database.h>
#include <benchmark/RandomNumber.h>
#include <stdint.h>
#include <velocypack/vpack.h>
#include <iostream>
#include <vector>

namespace benchmark {
class Suite {
 private:
  typedef arangodb::velocypack::Slice Slice;

  uint64_t _threadCount;
  uint64_t _keyCount;
  uint64_t _lookupCount;
  uint64_t _hotsetCount;
  uint64_t _rangeLimit;
  std::string _folder;

  benchmark::Database _db;

 public:
  Suite(uint64_t threadCount, uint64_t keyCount, uint64_t lookupCount,
        uint64_t hotsetCount, uint64_t rangeLimit, std::string const& folder);
  ~Suite();

  void printConfig();
  bool runAll();
};
}

#endif
