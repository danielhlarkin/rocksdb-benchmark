#include <benchmark/RocksDBInstance.h>
#include <cassert>

using namespace benchmark;

Mutex RocksDBInstance::_rocksDbMutex;
rocksdb::DB* RocksDBInstance::_db = nullptr;
std::atomic<uint64_t> RocksDBInstance::_instanceCount(0);

rocksdb::Options RocksDBInstance::generateOptions() {
  rocksdb::BlockBasedTableOptions table_options;
  table_options.block_cache =
      rocksdb::NewLRUCache(100 * 1048576);  // 100MB uncompressed cache

  rocksdb::Options options;
  options.table_factory.reset(
      rocksdb::NewBlockBasedTableFactory(table_options));
  options.create_if_missing = true;
  options.prefix_extractor.reset(
      rocksdb::NewFixedPrefixTransform(_prefixLength));
}

RocksDBInstance::RocksDBInstance(std::string const& folder)
    : _dbFolder(folder) {
  MUTEX_LOCKER(locker, _rocksDbMutex);
  if (_db == nullptr) {
    auto options = generateOptions();
    auto status = rocksdb::DB::Open(options, _dbFolder, &_db);
    if (!status.ok()) {
      std::cerr << status.ToString() << std::endl;
    }
    assert(status.ok());
  }
  _instanceCount++;
}

RocksDBInstance::~RocksDBInstance() {
  MUTEX_LOCKER(locker, _rocksDbMutex);
  _instanceCount--;
  if (_instanceCount.load() == 0) {
    delete _db;
    _db = nullptr;
  }
}

rocksdb::DB* RocksDBInstance::db() { return _db; }
