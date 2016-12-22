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
  std::string prefix = PrimaryIndex::buildPrefix(0x0ULL, 0x1ULL, 0x2ULL);
  PrimaryIndex index(folder, prefix);
  HybridLogicalClock clock;

  int count = 100;

  std::vector<uint64_t> revisions;
  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    bool ok = index.insert(std::to_string(i), revisions[i], false);
    assert(ok);
    std::cout << "Inserted document " << i << " with revision " << revisions[i]
              << std::endl;
  }

  for (int i = 0; i < count; i++) {
    uint64_t r = index.lookup(std::to_string(i), clock.getTimeStamp());
    assert(r == revisions[i]);
    std::cout << "Found document " << i << std::endl;
  }

  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    bool ok = index.insert(std::to_string(i), revisions[i + count], true);
    assert(ok);
    uint64_t r = index.lookup(std::to_string(i), clock.getTimeStamp());
    assert(r == 0);
    r = index.lookup(std::to_string(i), (revisions[i] + 1));
    std::cout << "Tombstoned document " << i << " with revision "
              << revisions[i + count] << std::endl;
  }

  for (int i = 0; i < count; i++) {
    uint64_t r = index.lookup(std::to_string(i), (revisions[i] - 1));
    assert(r == 0);
  }
  for (int i = count; i < (count * 2); i++) {
    uint64_t r = index.lookup(std::to_string(i - count), (revisions[i] - 1));
    assert(r == revisions[i - count]);
  }
  std::cout << "Maximum revision restiction works!" << std::endl;

  for (int i = 0; i < count; i++) {
    bool ok = index.remove(std::to_string(i), revisions[i + count]);
    assert(ok);
    uint64_t r = index.lookup(std::to_string(i), clock.getTimeStamp());
    assert(r == revisions[i]);
    ok = index.remove(std::to_string(i), revisions[i]);
    assert(ok);
    r = index.lookup(std::to_string(i), clock.getTimeStamp());
    assert(r == 0);
    std::cout << "Removed document " << i << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
