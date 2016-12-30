#ifndef ROCKSDB_BENCHMARK_WORKLOAD_H
#define ROCKSDB_BENCHMARK_WORKLOAD_H 1

#include <benchmark/Database.h>
#include <benchmark/Mutex.h>
#include <benchmark/MutexLocker.h>
#include <benchmark/RandomNumber.h>
#include <benchmark/UsageMonitor.h>
#include <benchmark/utility.h>
#include <q-digest/qdigest.h>
#include <stdint.h>
#include <velocypack/vpack.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace benchmark {
struct WorkloadInitializationData {
  benchmark::Database* db;
  uint64_t threadCount;
  uint64_t keyCount;
  uint64_t lookupCount;
  uint64_t hotsetCount;
  uint64_t rangeLimit;
  std::string folder;
  WorkloadInitializationData(benchmark::Database* d, uint64_t tc, uint64_t kc,
                             uint64_t lc, uint64_t hc, uint64_t rl,
                             std::string const& f)
      : db(d),
        threadCount(tc),
        keyCount(kc),
        lookupCount(lc),
        hotsetCount(hc),
        rangeLimit(rl),
        folder(f) {}
};

class Workload {
 protected:
  typedef arangodb::velocypack::Slice VPackSlice;
  typedef arangodb::velocypack::SliceContainer VPackSliceContainer;
  typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint;
  typedef std::function<void(void*, qdigest::QDigest*, timepoint*, timepoint*)>
      WorkerType;

  benchmark::Database* _db;
  benchmark::RandomNumber _random;

  qdigest::QDigest _opDigest;  // merged results from workers
  qdigest::QDigest _residentDigest;
  qdigest::QDigest _virtualDigest;
  qdigest::QDigest _diskDigest;
  std::atomic<uint64_t> _opCount;
  uint64_t _duration;

  uint64_t _threadCount;
  uint64_t _keyCount;
  uint64_t _lookupCount;
  uint64_t _hotsetCount;
  uint64_t _rangeLimit;
  std::string _folder;

  std::atomic<uint64_t> _workersDone;
  std::atomic<uint64_t> _workersErrored;
  std::chrono::nanoseconds _monitorInterval;

  std::time_t _minTimestamp;
  std::time_t _maxTimestamp;

  benchmark::Mutex _lock;

  Workload(benchmark::WorkloadInitializationData const& data,
           std::time_t minTimestamp = std::numeric_limits<std::time_t>::max(),
           std::time_t maxTimestamp = std::numeric_limits<std::time_t>::min());
  ~Workload();

 public:
  bool run();
  void printHeader();
  void printResults();

  std::time_t minTimestamp();
  std::time_t maxTimestamp();

 protected:
  virtual std::string resultsHeader() = 0;
  virtual std::string operationName() = 0;

  virtual void* generateWorkerInput(uint64_t i) = 0;
  virtual WorkerType worker() = 0;

  void printUsageStatistics();
  void printOpStatistics();

 private:
  std::string readableTime(uint64_t time);
  std::string readableSpace(uint64_t space);
  std::string throughput();
};
}

#endif
