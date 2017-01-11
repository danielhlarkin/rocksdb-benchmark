#ifndef ROCKSDB_BENCHMARK_STORAGE_ENGINE_H
#define ROCKSDB_BENCHMARK_STORAGE_ENGINE_H 1

#include <benchmark/RocksDBInstance.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>

namespace benchmark {
class StorageEngine {
 private:
  typedef arangodb::velocypack::Slice VPackSlice;
  typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

  std::string _prefix;
  rocksdb::ReadOptions _readOptions;

 public:
  StorageEngine(uint32_t slug);
  ~StorageEngine();

  static std::string buildPrefix(uint32_t slug);

  bool insert(rocksdb::Transaction* tx, uint64_t revision,
              VPackSlice const& value);
  bool remove(rocksdb::Transaction* tx, uint64_t revision);
  VPackSliceContainer lookup(rocksdb::Transaction* tx, uint64_t revision) const;

 private:
  std::string buildKey(uint64_t revision) const;
  std::string wrapValue(VPackSlice const& value) const;
};
}

#endif
