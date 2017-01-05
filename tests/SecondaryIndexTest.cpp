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
  SecondaryIndex index(folder, 0x0ULL, 0x1ULL, 0x2ULL);
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
    bool ok = index.insert(std::to_string(i), revisions[i],
                           timestamps[i].slice(), false);
    assert(ok);
    std::cout << "Inserted document " << i << " with revision " << revisions[i]
              << std::endl;
  }

  for (int i = 0; i < count; i++) {
    std::vector<uint64_t> r = index.lookup(
        timestamps[i].slice(), timestamps[i].slice(), clock.getTimeStamp());
    assert(r.size() == 1);
    assert(r[0] == revisions[i]);
    std::cout << "Found document " << i << std::endl;
  }

  for (int i = 0; i < (count - 2); i++) {
    std::vector<uint64_t> r = index.lookup(
        timestamps[i].slice(), timestamps[i + 2].slice(), clock.getTimeStamp());
    assert(r.size() == 3);
    for (int j = 0; j < 3; j++) {
      assert(r[j] == revisions[i + j]);
    }
    std::cout << "Found documents " << i << " through " << i + 2 << std::endl;
  }
  std::cout << "Partial-range lookup works!" << std::endl;

  {
    std::vector<uint64_t> r =
        index.lookup(timestamps[0].slice(), timestamps[count - 1].slice(),
                     clock.getTimeStamp());
    assert(r.size() == count);
    for (int i = 0; i < count; i++) {
      assert(r[i] == revisions[i]);
    }
    std::cout << "Full-range lookup works!" << std::endl;
  }

  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    bool ok = index.insert(std::to_string(i), revisions[i + count],
                           timestamps[i].slice(), true);
    assert(ok);
    std::vector<uint64_t> r = index.lookup(
        timestamps[i].slice(), timestamps[i].slice(), clock.getTimeStamp());
    assert(r.size() == 0);
    std::cout << "Tombstoned document " << i << " with revision "
              << revisions[i + count] << std::endl;
  }

  for (int i = 0; i < count; i++) {
    std::vector<uint64_t> r = index.lookup(
        timestamps[0].slice(), timestamps[i].slice(), (revisions[i] - 1));
    assert(r.size() == i);
    for (int j = 0; j < r.size(); j++) {
      assert(r[j] == revisions[j]);
    }
  }
  for (int i = count; i < (count * 2); i++) {
    std::vector<uint64_t> r =
        index.lookup(timestamps[0].slice(), timestamps[i - count].slice(),
                     (revisions[i] - 1));

    assert(r.size() == 1);
    assert(r[0] == revisions[i - count]);
  }
  std::cout << "Maximum revision restriction works!" << std::endl;

  for (int i = 0; i < count; i++) {
    bool ok = index.remove(std::to_string(i), revisions[i + count],
                           timestamps[i].slice());
    assert(ok);
    std::vector<uint64_t> r = index.lookup(
        timestamps[i].slice(), timestamps[i].slice(), clock.getTimeStamp());
    assert(r.size() == 1);
    assert(r[0] == revisions[i]);
    ok = index.remove(std::to_string(i), revisions[i], timestamps[i].slice());
    assert(ok);
    r = index.lookup(timestamps[i].slice(), timestamps[i].slice(),
                     clock.getTimeStamp());
    assert(r.size() == 0);
    std::cout << "Removed document " << i << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
