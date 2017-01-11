#include <benchmark/HybridLogicalClock.h>
#include <benchmark/PrimaryIndex.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>
#include <cassert>
#include <iostream>
#include <vector>

using namespace benchmark;

typedef arangodb::velocypack::Slice VPackSlice;
typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

int main(int, char**) {
  std::string folder("/tmp/test_rocksdb-benchmark_primary-index");
  RocksDBInstance instance(folder);
  auto writeOptions = rocksdb::WriteOptions();
  auto txOptions = rocksdb::OptimisticTransactionOptions();
  auto db = instance.db();
  PrimaryIndex index(instance.getIndexSlug(0x0ULL, 0x1ULL, 0x2ULL));
  HybridLogicalClock clock;

  int count = 100;

  std::vector<uint64_t> revisions;
  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    bool ok = index.insert(tx, std::to_string(i), revisions[i], false);
    assert(ok);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Inserted document " << i << " with revision " << revisions[i]
              << std::endl;
  }

  for (int i = 0; i < count; i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    uint64_t r = index.lookup(tx, std::to_string(i), clock.getTimeStamp());
    assert(r == revisions[i]);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Found document " << i << std::endl;
  }

  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    bool ok = index.insert(tx, std::to_string(i), revisions[i + count], true);
    assert(ok);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    uint64_t r = index.lookup(tx, std::to_string(i), clock.getTimeStamp());
    assert(r == 0);
    r = index.lookup(tx, std::to_string(i), (revisions[i] + 1));
    assert(r == revisions[i]);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Tombstoned document " << i << " with revision "
              << revisions[i + count] << std::endl;
  }

  for (int i = 0; i < count; i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    uint64_t r = index.lookup(tx, std::to_string(i), (revisions[i] - 1));
    assert(r == 0);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
  }
  for (int i = count; i < (count * 2); i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    uint64_t r =
        index.lookup(tx, std::to_string(i - count), (revisions[i] - 1));
    assert(r == revisions[i - count]);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
  }
  std::cout << "Maximum revision restiction works!" << std::endl;

  for (int i = 0; i < count; i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    bool ok = index.remove(tx, std::to_string(i), revisions[i + count]);
    assert(ok);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    uint64_t r = index.lookup(tx, std::to_string(i), clock.getTimeStamp());
    assert(r == revisions[i]);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    ok = index.remove(tx, std::to_string(i), revisions[i]);
    assert(ok);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    r = index.lookup(tx, std::to_string(i), clock.getTimeStamp());
    assert(r == 0);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Removed document " << i << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
