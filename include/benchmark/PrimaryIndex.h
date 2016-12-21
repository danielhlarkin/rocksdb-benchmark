#ifndef ROCKSDB_BENCHMARK_PRIMARY_INDEX_H
#define ROCKSDB_BENCHMARK_PRIMARY_INDEX_H 1

#include <benchmark/RocksDBInstance.h>
#include <velocypack/vpack.h>
#include <vector>

namespace benchmark {
class PrimaryIndex {
 private:
  typedef arangodb::velocypack::Slice Slice;

  benchmark::RocksDBInstance _instance;
  rocksdb::DB* _db;
  std::string _prefix;

 public:
  PrimaryIndex(std::string const& folder, std::string const& prefix);
  ~PrimaryIndex();

  static std::string buildPrefix(uint64_t databaseId, uint64_t collectionId,
                                 uint64_t indexId);

  bool insert(std::string const& key, uint64_t revision,
              arangodb::velocypack::Slice const& value);
  bool remove(std::string const& key, uint64_t revision);
  arangodb::velocypack::Slice lookup(std::string const& key,
                                     uint64_t maxRevision);
};
}

#endif
