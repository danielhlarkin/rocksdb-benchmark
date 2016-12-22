#ifndef ROCKSDB_BENCHMARK_PRIMARY_INDEX_H
#define ROCKSDB_BENCHMARK_PRIMARY_INDEX_H 1

#include <benchmark/RocksDBInstance.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>
#include <vector>

namespace benchmark {
class PrimaryIndex {
 private:
  typedef arangodb::velocypack::Slice VPackSlice;
  typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

  benchmark::RocksDBInstance _instance;
  rocksdb::DB* _db;
  std::string _prefix;
  rocksdb::ReadOptions _readOptions;
  rocksdb::WriteOptions _writeOptions;

  std::string _tombstone;
  std::string _nonTombstone;

 public:
  PrimaryIndex(std::string const& folder, std::string const& prefix);
  ~PrimaryIndex();

  static std::string buildPrefix(uint64_t databaseId, uint64_t collectionId,
                                 uint64_t indexId);

  bool insert(std::string const& key, uint64_t revision, bool tombstone);
  bool remove(std::string const& key, uint64_t revision);
  uint64_t lookup(std::string const& key, uint64_t maxRevision) const;

 private:
  std::string buildKey(std::string const& key, uint64_t revision) const;
  std::string buildKeyPrefix(std::string const& key) const;
  std::string buildKeySentinel(
      std::string const& key,
      uint64_t maxRevision = 0xFFFFFFFFFFFFFFFFULL) const;

  bool isTombstoned(rocksdb::Iterator const* it) const;
  uint64_t unwrap(rocksdb::Iterator const* it) const;
};
}

#endif
