#ifndef ROCKSDB_BENCHMARK_LOOKUP_RANGE_H
#define ROCKSDB_BENCHMARK_LOOKUP_RANGE_H 1

#include <benchmark/RandomNumber.h>
#include <benchmark/Workload.h>

namespace benchmark {

struct LookupRangeInput {
  benchmark::Database* db;
  uint64_t expectedCount;
  uint64_t rangeLimit;
  std::time_t lower;
  std::time_t upper;
  std::atomic<uint64_t>* opCount;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  LookupRangeInput(benchmark::Database* d, uint64_t e, uint64_t r,
                   std::time_t l, std::time_t u, std::atomic<uint64_t>* ops,
                   std::atomic<uint64_t>* done, std::atomic<uint64_t>* errored)
      : db(d),
        expectedCount(e),
        rangeLimit(r),
        lower(l),
        upper(u),
        opCount(ops),
        workersDone(done),
        workersErrored(errored) {}
};

class LookupRange : public Workload {
 public:
  LookupRange(WorkloadInitializationData const& data, std::time_t minTimestamp,
              std::time_t maxTimestamp);
  ~LookupRange();

 protected:
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void lookupRanges(void* input, qdigest::QDigest* opDigest,
                           timepoint* start, timepoint* end);
};
}

#endif
