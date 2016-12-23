#include <benchmark/Suite.h>
#include <stdlib.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  if (argc < 7) {
    std::cerr << "Incorrect number of parameters! Correct usage is "
                 "`rocksdb-benchmark [threadCount] [keyCount] [lookupCount] "
                 "[hotsetCount] [rangeLimit] [folder]`."
              << std::endl;
    return -1;
  }
  uint64_t threadCount = atoll(argv[1]);
  uint64_t keyCount = atoll(argv[2]);
  uint64_t lookupCount = atoll(argv[3]);
  uint64_t hotsetCount = atoll(argv[4]);
  uint64_t rangeLimit = atoll(argv[5]);
  std::string folder(argv[6]);

  benchmark::Suite suite(threadCount, keyCount, lookupCount, hotsetCount,
                         rangeLimit, folder);
  suite.printConfig();
  suite.runAll();

  return 0;
}
