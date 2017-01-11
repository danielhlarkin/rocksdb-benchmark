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

  std::string _prefix;
  rocksdb::ReadOptions _readOptions;

  std::string _tombstone;
  std::string _nonTombstone;

 public:
  PrimaryIndex(uint32_t slug);
  ~PrimaryIndex();

  static std::string buildPrefix(uint32_t slug);

  bool insert(rocksdb::Transaction* tx, std::string const& key,
              uint64_t revision, bool tombstone);
  bool remove(rocksdb::Transaction* tx, std::string const& key,
              uint64_t revision);
  uint64_t lookup(rocksdb::Transaction* tx, std::string const& key,
                  uint64_t maxRevision) const;

 private:
  std::string buildKey(std::string const& key, uint64_t revision) const;
  std::string buildKeyPrefix(std::string const& key) const;
  std::string buildKeySentinel(
      std::string const& key,
      uint64_t maxRevision = 0xFFFFFFFFFFFFFFFFULL) const;

  bool isTombstoned(rocksdb::Iterator const* it) const;
  bool sameKey(rocksdb::Iterator const* it, std::string const& key) const;
  uint64_t unwrap(rocksdb::Iterator const* it) const;
};
}

#endif
