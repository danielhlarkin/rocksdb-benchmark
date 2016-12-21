#ifndef ROCKSDB_BENCHMARK_DATABASE_H
#define ROCKSDB_BENCHMARK_DATABASE_H 1

#include <benchmark/HybridLogicalClock.h>
#include <benchmark/PrimaryIndex.h>
#include <benchmark/RandomNumber.h>
#include <benchmark/SecondaryIndex.h>
#include <benchmark/StorageEngine.h>
#include <velocypack/vpack.h>
#include <vector>

namespace benchmark {
class Database {
 private:
  typedef arangodb::velocypack::Slice Slice;

  std::string _folder;
  benchmark::HybridLogicalClock _clock;
  benchmark::RandomNumber _random;
  uint64_t _databaseId;
  uint64_t _collectionId;
  uint64_t _primaryIndexId;
  uint64_t _secondaryIndexId;
  benchmark::StorageEngine _storageEngine;
  benchmark::PrimaryIndex _primaryIndex;
  benchmark::SecondaryIndex _secondaryIndex;

 public:
  Database(std::string const& folder);
  ~Database();

  bool insert(std::string const& key, Slice const& value);
  bool remove(std::string const& key);
  Slice lookupSingle(std::string const& key);
  std::vector<Slice> lookupRange(Slice const& start, Slice const& end);
};
}

#endif
