#ifndef ROCKSDB_BENCHMARK_LOOKUP_SINGLE_HOTSET_H
#define ROCKSDB_BENCHMARK_LOOKUP_SINGLE_HOTSET_H 1

#include <benchmark/RandomNumber.h>
#include <benchmark/Workload.h>

namespace benchmark {

struct LookupSingleHotsetInput {
  benchmark::Database* db;
  uint64_t hotsetCount;
  uint64_t lookupCount;
  std::atomic<uint64_t>* opCount;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  LookupSingleHotsetInput(benchmark::Database* d, uint64_t h, uint64_t l,
                          std::atomic<uint64_t>* ops,
                          std::atomic<uint64_t>* done,
                          std::atomic<uint64_t>* errored)
      : db(d),
        hotsetCount(h),
        lookupCount(l),
        opCount(ops),
        workersDone(done),
        workersErrored(errored) {}
};

class LookupSingleHotset : public Workload {
 public:
  LookupSingleHotset(WorkloadInitializationData const& data);
  ~LookupSingleHotset();

 protected:
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void lookupHotset(void* input, qdigest::QDigest* opDigest,
                           timepoint* start, timepoint* end);
};
}

#endif
