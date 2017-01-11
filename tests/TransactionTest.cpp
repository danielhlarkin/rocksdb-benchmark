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
  std::string folder("/tmp/test_rocksdb-benchmark_transactions");
  Database db(folder);
  HybridLogicalClock clock;

  int segments = 32;

  for (int s = 0; s < segments; s++) {
    std::vector<std::string> keys;
    std::vector<VPackSliceContainer> documents;

    for (int i = 0; i < (1 << s); i++) {
      keys.emplace_back(
          std::to_string(s).append("_").append(std::to_string(i)));
      documents.emplace_back(utility::generateDocument(keys[i]));
    }
    std::cout << "Generated segment " << s << std::endl;

    auto r = db.insertBatch(keys, documents);
    assert(r.size == (1 << s));
    std::cout << "Inserted segment " << s << std::endl;

    keys.clear();
    documents.clear();
  }

  std::cout << "All good!" << std::endl;

  return 0;
}
