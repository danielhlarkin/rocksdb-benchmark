#ifndef ROCKSDB_BENCHMARK_DATABASE_H
#define ROCKSDB_BENCHMARK_DATABASE_H 1

#include <benchmark/HybridLogicalClock.h>
#include <benchmark/PrimaryIndex.h>
#include <benchmark/RandomNumber.h>
#include <benchmark/RocksDBInstance.h>
#include <benchmark/SecondaryIndex.h>
#include <benchmark/StorageEngine.h>
#include <velocypack/vpack.h>
#include <vector>

namespace benchmark {
class Database {
 public:
  typedef arangodb::velocypack::Slice VPackSlice;
  typedef arangodb::velocypack::SliceContainer VPackSliceContainer;
  typedef arangodb::velocypack::ValueLength VPackValueLength;

 private:
  std::string _folder;
  benchmark::HybridLogicalClock _clock;
  benchmark::RandomNumber _random;
  uint64_t _databaseId;
  uint64_t _collectionId;
  uint64_t _primaryIndexId;
  uint64_t _secondaryIndexId;

  benchmark::RocksDBInstance _instance;
  rocksdb::TransactionDB* _db;
  ArangoComparator* _cmp;
  std::string _prefix;
  rocksdb::WriteOptions _writeOptions;
  rocksdb::TransactionOptions _txOptions;

  benchmark::StorageEngine _storageEngine;
  benchmark::PrimaryIndex _primaryIndex;
  benchmark::SecondaryIndex _secondaryIndex;

 public:
  Database(std::string const& folder);
  ~Database();

  uint64_t now();

  uint64_t insert(std::string const& key, VPackSlice const& value);
  std::vector<uint64_t> insertBatch(
      std::vector<std::string> const& keys,
      std::vector<VPackSliceContainer> const& values);
  bool remove(std::string const& key);
  VPackSliceContainer lookupSingle(std::string const& key,
                                   uint64_t maxRevision) const;
  std::vector<VPackSliceContainer> lookupRange(VPackSlice const& start,
                                               VPackSlice const& end,
                                               uint64_t maxRevision) const;

 private:
  bool insertInternal(rocksdb::Transaction* tx, uint64_t revision,
                      std::string const& key, VPackSlice const& value);
};
}

#endif
