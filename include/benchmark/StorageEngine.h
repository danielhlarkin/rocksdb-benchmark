#ifndef ROCKSDB_BENCHMARK_STORAGE_ENGINE_H
#define ROCKSDB_BENCHMARK_STORAGE_ENGINE_H 1

#include <benchmark/RocksDBInstance.h>
#include <velocypack/vpack.h>

namespace benchmark {
class StorageEngine {
 private:
  typedef arangodb::velocypack::Slice Slice;

  benchmark::RocksDBInstance _instance;
  rocksdb::DB* _db;
  std::string _prefix;

 public:
  StorageEngine(std::string const& folder, std::string const& prefix);
  ~StorageEngine();

  bool insert(uint64_t revision, Slice const& value);
  bool remove(uint64_t revision);
  Slice lookup(uint64_t revision);
};
}

#endif
