#ifndef ROCKSDB_BENCHMARK_WORKLOAD_H
#define ROCKSDB_BENCHMARK_WORKLOAD_H 1

#include <benchmark/Database.h>
#include <benchmark/RandomNumber.h>
#include <benchmark/UsageMonitor.h>
#include <benchmark/utility.h>
#include <q-digest/qdigest.h>
#include <stdint.h>
#include <velocypack/vpack.h>
#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace benchmark {
class Workload {
 protected:
  typedef arangodb::velocypack::Slice VPackSlice;
  typedef arangodb::velocypack::SliceContainer VPackSliceContainer;
  typedef std::function<void(void*, qdigest::QDigest*)> WorkerType;

  benchmark::Database* _db;
  benchmark::RandomNumber _random;
  qdigest::QDigest _opDigest;  // merged results from workers
  qdigest::QDigest _residentDigest;
  qdigest::QDigest _virtualDigest;
  qdigest::QDigest _diskDigest;

  uint64_t _threadCount;
  uint64_t _keyCount;
  uint64_t _lookupCount;
  uint64_t _hotsetCount;
  uint64_t _rangeLimit;
  std::string _folder;

  std::atomic<uint64_t> _workersDone;
  std::atomic<uint64_t> _workersErrored;
  std::chrono::nanoseconds _monitorInterval;

  Workload(benchmark::Database* db, uint64_t threadCount, uint64_t keyCount,
           uint64_t lookupCount, uint64_t hotsetCount, uint64_t rangeLimit,
           std::string const& folder);
  ~Workload();

 public:
  bool run();
  void printResults();

 protected:
  virtual std::string resultsHeader() = 0;
  virtual std::string operationName() = 0;

  virtual void* generateWorkerInput(uint64_t i) = 0;
  virtual WorkerType worker() = 0;

  void printHeader();
  void printUsageStatistics();
  void printOpStatistics();
};
}

#endif
