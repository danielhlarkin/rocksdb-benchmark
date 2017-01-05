#include <benchmark/HybridLogicalClock.h>
#include <benchmark/StorageEngine.h>
#include <benchmark/utility.h>
#include <velocypack/vpack.h>
#include <cassert>
#include <iostream>
#include <vector>

using namespace benchmark;

typedef arangodb::velocypack::Slice VPackSlice;
typedef arangodb::velocypack::SliceContainer VPackSliceContainer;

int main(int, char**) {
  std::string folder("/tmp/test_rocksdb-benchmark_storage-engine");
  StorageEngine engine(folder, 0x0ULL, 0x1ULL);
  HybridLogicalClock clock;

  int count = 100;

  std::vector<VPackSliceContainer> documents;
  for (int i = 0; i < count; i++) {
    documents.emplace_back(utility::generateDocument(std::to_string(i)));
    std::cout << "Generated document " << i << std::endl;
  }

  std::vector<uint64_t> revisions;
  for (int i = 0; i < count; i++) {
    revisions.emplace_back(clock.getTimeStamp());
    bool ok = engine.insert(revisions[i], documents[i].slice());
    assert(ok);
    std::cout << "Inserted document " << i << std::endl;
  }

  for (int i = 0; i < count; i++) {
    VPackSliceContainer data = engine.lookup(revisions[i]);
    assert(data.slice() == documents[i].slice());
    std::cout << "Found document " << i << std::endl;
  }

  for (int i = 0; i < count; i++) {
    bool ok = engine.remove(revisions[i]);
    assert(ok);
    VPackSliceContainer data = engine.lookup(revisions[i]);
    assert(data.slice() == VPackSlice());
    std::cout << "Removed document " << i << std::endl;
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
