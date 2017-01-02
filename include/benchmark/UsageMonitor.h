#ifndef ROCKSDB_BENCHMARK_USAGE_MONITOR_H
#define ROCKSDB_BENCHMARK_USAGE_MONITOR_H 1

#include <q-digest/qdigest.h>
#include <stdint.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

namespace benchmark {
class UsageMonitor {
 private:
  std::string _folder;

 public:
  UsageMonitor(std::string const& folder);
  ~UsageMonitor();

  uint64_t getResidentUsage();
  uint64_t getVirtualUsage();
  uint64_t getDiskUsage();

  static void monitor(std::string const& folder, uint64_t threadCount,
                      std::chrono::nanoseconds interval,
                      std::atomic<uint64_t> const* workersDone,
                      std::atomic<uint64_t> const* workersErrored,
                      qdigest::QDigest* residentDigest,
                      qdigest::QDigest* virtualDigest,
                      qdigest::QDigest* diskDigest);
};
}

#endif
