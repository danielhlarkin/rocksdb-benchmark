#ifndef ROCKSDB_BENCHMARK_LOOKUP_SINGLE_RANDOM_H
#define ROCKSDB_BENCHMARK_LOOKUP_SINGLE_RANDOM_H 1

#include <benchmark/RandomNumber.h>
#include <benchmark/Workload.h>

namespace benchmark {

struct LookupSingleRandomInput {
  benchmark::Database* db;
  uint64_t keyCount;
  uint64_t lookupCount;
  std::atomic<uint64_t>* opCount;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  LookupSingleRandomInput(benchmark::Database* d, uint64_t k, uint64_t l,
                          std::atomic<uint64_t>* ops,
                          std::atomic<uint64_t>* done,
                          std::atomic<uint64_t>* errored)
      : db(d),
        keyCount(k),
        lookupCount(l),
        opCount(ops),
        workersDone(done),
        workersErrored(errored) {}
};

class LookupSingleRandom : public Workload {
 public:
  LookupSingleRandom(WorkloadInitializationData const& data);
  ~LookupSingleRandom();

 protected:
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void lookupRandom(void* input, qdigest::QDigest* opDigest,
                           timepoint* start, timepoint* end);
};
}

#endif
