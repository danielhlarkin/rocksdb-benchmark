#include <stdlib.h>
#include <iostream>

int main(int argc, char** argv) {
  if (argc < 6) {
    std::cerr << "Incorrect number of parameters! Correct usage is "
                 "`rocksdb-benchmark [threadCount] [keyCount] [lookupCount] "
                 "[hotsetCount] [rangeLimit]`."
              << std::endl;
    return -1;
  }
  uint64_t threadCount = atoll(argv[1]);
  uint64_t keyCount = atoll(argv[2]);
  uint64_t lookupCount = atoll(argv[3]);
  uint64_t hotsetCount = atoll(argv[4]);
  uint64_t rangeLimit = atoll(argv[5]);

  std::cout << "Benchmarking RocksDB with parameters {" << std::endl
            << "  threadCount: " << threadCount << "," << std::endl
            << "  keyCount = " << keyCount << "," << std::endl
            << "  lookupCount: " << lookupCount << "," << std::endl
            << "  hotsetCount: " << hotsetCount << "," << std::endl
            << "  rangeLimit: " << rangeLimit << std::endl
            << "}" << std::endl;

  return 0;
}
