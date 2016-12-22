#ifndef ROCKSDB_BENCHMARK_RANDOM_NUMBER_H
#define ROCKSDB_BENCHMARK_RANDOM_NUMBER_H 1

#include <benchmark/Mutex.h>
#include <benchmark/MutexLocker.h>
#include <cstdint>

namespace benchmark {
class RandomNumber {
 private:
  static benchmark::Mutex _mutex;
  static uint64_t _current;

 public:
  RandomNumber(uint64_t seed);
  uint64_t next();
  uint64_t nextInRange(int range);
};
}

#endif
