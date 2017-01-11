#ifndef ROCKSDB_BENCHMARK_SECONDARY_INDEX_H
#define ROCKSDB_BENCHMARK_SECONDARY_INDEX_H 1

#include <benchmark/RocksDBInstance.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>
#include <algorithm>
#include <chrono>
#include <vector>

namespace benchmark {
class SecondaryIndex {
 private:
  typedef arangodb::velocypack::Slice VPackSlice;
  typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

  ArangoComparator* _cmp;
  std::string _prefix;
  rocksdb::ReadOptions _readOptions;

  std::string _tombstone;
  std::string _nonTombstone;

 public:
  SecondaryIndex(uint32_t slug, benchmark::ArangoComparator* cmp);
  ~SecondaryIndex();

  static std::string buildPrefix(uint32_t slug);

  bool insert(rocksdb::Transaction* tx, std::string const& key,
              uint64_t revision, VPackSlice const& value, bool tombstone);
  bool remove(rocksdb::Transaction* tx, std::string const& key,
              uint64_t revision, VPackSlice const& value);
  std::vector<uint64_t> lookup(rocksdb::Transaction* tx,
                               VPackSlice const& start, VPackSlice const& end,
                               uint64_t maxRevision) const;

 private:
  std::string buildKey(std::string const& key, uint64_t revision,
                       VPackSlice const& value) const;
  std::string buildKeyPrefix(std::string const& key,
                             VPackSlice const& value) const;
  std::string buildKeyPrefix(rocksdb::Iterator const* it) const;
  std::string buildKeySentinel(std::string const& key,
                               VPackSlice const& value) const;
  std::string buildValuePrefix(VPackSlice const& value) const;
  std::string buildValueSentinel(VPackSlice const& value) const;

  bool isTombstoned(rocksdb::Iterator const* it) const;
  VPackSlice extract(rocksdb::Iterator const* it) const;
  std::time_t timestamp(VPackSlice const& value) const;
  std::time_t timestamp(std::string const& s) const;
  std::time_t timestamp(rocksdb::Iterator const* it) const;
  VPackSlice extract(std::string const& s) const;
  std::string extractKey(rocksdb::Iterator const* it) const;
  bool sameKey(rocksdb::Iterator const* it, std::string const& key) const;
  uint64_t unwrap(rocksdb::Iterator const* it) const;
};
}

#endif
