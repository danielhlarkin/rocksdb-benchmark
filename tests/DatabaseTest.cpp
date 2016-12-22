#include <benchmark/Database.h>
#include <benchmark/HybridLogicalClock.h>
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
  std::string folder("/tmp/test_rocksdb-benchmark_database");
  Database db(folder);
  HybridLogicalClock clock;
  std::chrono::system_clock systemClock;
  auto start = std::chrono::system_clock::to_time_t(systemClock.now());

  int count = 100;

  std::vector<VPackSliceContainer> documents;
  std::vector<VPackSliceContainer> timestamps;
  for (int i = 0; i < count; i++) {
    documents.emplace_back(
        utility::generateDocument(std::to_string(i), (start + (i * 1000))));
    timestamps.emplace_back(
        utility::generateSecondarySlice(start + (i * 1000)));
    std::cout << arangodb::velocypack::HexDump(timestamps[i].slice())
              << std::endl;
    std::cout << "Generated document " << i << std::endl;
  }

  std::vector<uint64_t> revisions;
  for (int i = 0; i < count; i++) {
    uint64_t r = db.insert(std::to_string(i), documents[i].slice());
    assert(r > 0);
    revisions.emplace_back(r);
    std::cout << "Inserted document " << i << std::endl;
  }

  for (int i = 0; i < count; i++) {
    VPackSliceContainer data =
        db.lookupSingle(std::to_string(i), clock.getTimeStamp());
    assert(data.slice() == documents[i].slice());
    std::cout << "Found document " << i << std::endl;
  }
  std::cout << "Single lookup works!" << std::endl;

  for (int i = 0; i < (count - 2); i++) {
    std::vector<VPackSliceContainer> data = db.lookupRange(
        timestamps[i].slice(), timestamps[i + 2].slice(), clock.getTimeStamp());
    if (data.size() != 3) {
      std::cout << "size = " << data.size() << std::endl;
      std::cout << arangodb::velocypack::HexDump(timestamps[i].slice())
                << std::endl;
      std::cout << arangodb::velocypack::HexDump(timestamps[i + 2].slice())
                << std::endl;
      std::cout << "expected revisions: " << std::endl;
      for (int j = 0; j < 3; j++) {
        std::cout << "  " << revisions[i + j] << std::endl;
      }
    }
    assert(data.size() == 3);
    for (int j = 0; j < 3; j++) {
      assert(data[j].slice() == documents[i + j].slice());
    }
    std::cout << "Found documents " << i << " through " << i + 2 << std::endl;
  }

  for (int i = 0; i < count; i++) {
    bool ok = db.remove(std::to_string(i));
    assert(ok);
    VPackSliceContainer data =
        db.lookupSingle(std::to_string(i), clock.getTimeStamp());
    assert(data.slice() == VPackSlice());
    std::cout << "Removed document " << i << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
