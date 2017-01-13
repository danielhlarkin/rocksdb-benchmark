#ifndef ROCKSDB_BENCHMARK_SUITE_H
#define ROCKSDB_BENCHMARK_SUITE_H 1

#include <benchmark/Database.h>
#include <benchmark/Insert.h>
#include <benchmark/InsertBatch.h>
#include <benchmark/LookupRange.h>
#include <benchmark/LookupSingleHotset.h>
#include <benchmark/LookupSingleRandom.h>
#include <benchmark/RandomNumber.h>
#include <stdint.h>
#include <velocypack/vpack.h>
#include <iostream>
#include <vector>

namespace benchmark {
class Suite {
 private:
  typedef arangodb::velocypack::Slice Slice;
  typedef arangodb::velocypack::Builder VPackBuilder;
  typedef arangodb::velocypack::Value VPackValue;
  typedef arangodb::velocypack::ValueType VPackValueType;

  uint64_t _threadCount;
  uint64_t _keyCount;
  uint64_t _lookupCount;
  uint64_t _hotsetCount;
  uint64_t _rangeLimit;
  uint64_t _transactionSize;
  std::string _folder;

  benchmark::Database _db;

 public:
  Suite(uint64_t threadCount, uint64_t keyCount, uint64_t lookupCount,
        uint64_t hotsetCount, uint64_t rangeLimit, uint64_t transactionSize,
        std::string const& folder);
  ~Suite();

  void printConfig(VPackBuilder& builder);
  bool runAll(VPackBuilder& builder);
};
}

#endif
