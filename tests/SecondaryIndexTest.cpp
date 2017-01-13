#include <benchmark/HybridLogicalClock.h>
#include <benchmark/SecondaryIndex.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>
#include <cassert>
#include <chrono>
#include <iostream>
#include <vector>

using namespace benchmark;

typedef arangodb::velocypack::Slice VPackSlice;
typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

int main(int, char**) {
  std::string folder("/tmp/test_rocksdb-benchmark_secondary-index");
  RocksDBInstance instance(folder);
  auto writeOptions = rocksdb::WriteOptions();
  auto txOptions = rocksdb::TransactionOptions();
  auto db = instance.db();
  auto cmp = instance.comparator();
  SecondaryIndex index(instance.getIndexSlug(0x0ULL, 0x1ULL, 0x2ULL), cmp);
  HybridLogicalClock clock;
  std::chrono::system_clock systemClock;
  auto start = std::chrono::system_clock::to_time_t(systemClock.now());

  int count = 100;

  std::vector<uint64_t> revisions;
  std::vector<VPackSliceContainer> timestamps;
  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    timestamps.emplace_back(
        utility::generateSecondarySlice(start + (i * 1000)));
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    bool ok = index.insert(tx, std::to_string(i), revisions[i],
                           timestamps[i].slice(), false);
    assert(ok);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Inserted document " << i << " with revision " << revisions[i]
              << std::endl;
  }

  for (int i = 0; i < count; i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r = index.lookup(
        tx, timestamps[i].slice(), timestamps[i].slice(), clock.getTimeStamp());
    assert(r.size() == 1);
    assert(r[0] == revisions[i]);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Found document " << i << std::endl;
  }

  for (int i = 0; i < (count - 2); i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r =
        index.lookup(tx, timestamps[i].slice(), timestamps[i + 2].slice(),
                     clock.getTimeStamp());
    assert(r.size() == 3);
    for (int j = 0; j < 3; j++) {
      assert(r[j] == revisions[i + j]);
    }
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Found documents " << i << " through " << i + 2 << std::endl;
  }
  std::cout << "Partial-range lookup works!" << std::endl;

  {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r =
        index.lookup(tx, timestamps[0].slice(), timestamps[count - 1].slice(),
                     clock.getTimeStamp());
    assert(r.size() == count);
    for (int i = 0; i < count; i++) {
      assert(r[i] == revisions[i]);
    }
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Full-range lookup works!" << std::endl;
  }

  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    bool ok = index.insert(tx, std::to_string(i), revisions[i + count],
                           timestamps[i].slice(), true);
    assert(ok);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r = index.lookup(
        tx, timestamps[i].slice(), timestamps[i].slice(), clock.getTimeStamp());
    assert(r.size() == 0);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Tombstoned document " << i << " with revision "
              << revisions[i + count] << std::endl;
  }

  for (int i = 0; i < count; i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r = index.lookup(
        tx, timestamps[0].slice(), timestamps[i].slice(), (revisions[i] - 1));
    assert(r.size() == i);
    for (int j = 0; j < r.size(); j++) {
      assert(r[j] == revisions[j]);
    }
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
  }
  for (int i = count; i < (count * 2); i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r =
        index.lookup(tx, timestamps[0].slice(), timestamps[i - count].slice(),
                     (revisions[i] - 1));

    assert(r.size() == 1);
    assert(r[0] == revisions[i - count]);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
  }
  std::cout << "Maximum revision restriction works!" << std::endl;

  for (int i = 0; i < count; i++) {
    auto tx = db->BeginTransaction(writeOptions, txOptions);
    bool ok = index.remove(tx, std::to_string(i), revisions[i + count],
                           timestamps[i].slice());
    assert(ok);
    auto status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    std::vector<uint64_t> r = index.lookup(
        tx, timestamps[i].slice(), timestamps[i].slice(), clock.getTimeStamp());
    assert(r.size() == 1);
    assert(r[0] == revisions[i]);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    ok = index.remove(tx, std::to_string(i), revisions[i],
                      timestamps[i].slice());
    assert(ok);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    tx = db->BeginTransaction(writeOptions, txOptions);
    r = index.lookup(tx, timestamps[i].slice(), timestamps[i].slice(),
                     clock.getTimeStamp());
    assert(r.size() == 0);
    status = tx->Commit();
    assert(status.ok());
    delete tx;
    std::cout << "Removed document " << i << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
