#ifndef ROCKSDB_BENCHMARK_RANDOM_NUMBER_H
#define ROCKSDB_BENCHMARK_RANDOM_NUMBER_H 1

#include <cstdint>

namespace benchmark {
class RandomNumber {
 private:
  uint64_t _current;

 public:
  RandomNumber(uint64_t seed);
  uint64_t next();
  uint64_t nextInRange(int range);
};
}

#endif
