#ifndef ROCKSDB_BENCHMARK_ROCKSDB_INSTANCE_H
#define ROCKSDB_BENCHMARK_ROCKSDB_INSTANCE_H 1

#include <benchmark/ArangoComparator.h>
#include <benchmark/ArangoPrefixTransform.h>
#include <benchmark/Mutex.h>
#include <benchmark/MutexLocker.h>
#include <rocksdb/cache.h>
#include <rocksdb/db.h>
#include <rocksdb/filter_policy.h>
#include <rocksdb/options.h>
#include <rocksdb/slice_transform.h>
#include <rocksdb/table.h>
#include <rocksdb/utilities/transaction.h>
#include <rocksdb/utilities/transaction_db.h>
#include <stdint.h>
#include <cassert>
#include <iostream>
#include <string>

namespace benchmark {

class RocksDBInstance {
 private:
  static benchmark::Mutex _rocksDbMutex;
  static ArangoComparator* _comparator;
  static rocksdb::TransactionDB* _db;           // single global instance
  static uint32_t _maxSlug;                     // maximum slug granted
  static std::atomic<uint64_t> _instanceCount;  // number of active maps
  std::string _dbFolder;
  rocksdb::ReadOptions _readOptions;
  rocksdb::WriteOptions _writeOptions;
  rocksdb::TransactionOptions _txOptions;

 public:
  RocksDBInstance(std::string const& folder);
  ~RocksDBInstance();

  rocksdb::TransactionDB* db();
  ArangoComparator* comparator();

  uint32_t getDocumentSlug(uint64_t databaseId, uint64_t collectionId);
  uint32_t getIndexSlug(uint64_t databaseId, uint64_t collectionId,
                        uint64_t indexId);

 private:
  rocksdb::Options generateOptions();
  void getMaxSlug();
};
}  // namespace benchmark

#endif
