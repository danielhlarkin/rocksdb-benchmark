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

  benchmark::RocksDBInstance _instance;
  rocksdb::DB* _db;
  std::string _prefix;
  rocksdb::ReadOptions _readOptions;
  rocksdb::WriteOptions _writeOptions;

 public:
  StorageEngine(std::string const& folder, uint64_t databaseId,
                uint64_t collectionId);
  ~StorageEngine();

  static std::string buildPrefix(uint32_t slug);

  bool insert(uint64_t revision, VPackSlice const& value);
  bool remove(uint64_t revision);
  VPackSliceContainer lookup(uint64_t revision) const;

 private:
  std::string buildKey(uint64_t revision) const;
  std::string wrapValue(VPackSlice const& value) const;
};
}

#endif
