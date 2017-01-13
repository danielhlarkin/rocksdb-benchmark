#include <benchmark/Suite.h>
#include <stdlib.h>
#include <velocypack/vpack.h>
#include <iostream>
#include <string>

using namespace arangodb::velocypack;

int main(int argc, char** argv) {
  if (argc < 8) {
    std::cerr << "Incorrect number of parameters! Correct usage is "
                 "`rocksdb-benchmark [threadCount] [keyCount] [lookupCount] "
                 "[hotsetCount] [rangeLimit] [transactionSize] [folder]`."
              << std::endl;
    return -1;
  }
  uint64_t threadCount = atoll(argv[1]);
  uint64_t keyCount = atoll(argv[2]);
  uint64_t lookupCount = atoll(argv[3]);
  uint64_t hotsetCount = atoll(argv[4]);
  uint64_t rangeLimit = atoll(argv[5]);
  uint64_t transactionSize = atoll(argv[6]);
  std::string folder(argv[7]);

  benchmark::Suite suite(threadCount, keyCount, lookupCount, hotsetCount,
                         rangeLimit, transactionSize, folder);
  Builder builder;
  builder.add(Value(ValueType::Object));
  suite.printConfig(builder);
  builder.add("workloads", Value(ValueType::Array));
  suite.runAll(builder);
  builder.close();
  builder.close();

  Options options;
  options.prettyPrint = true;
  std::cout << builder.slice().toJson(&options) << std::endl;

  return 0;
}
