#ifndef ROCKSDB_BENCHMARK_RANDOM_LOOKUP_H
#define ROCKSDB_BENCHMARK_RANDOM_LOOKUP_H 1

#include <benchmark/RandomNumber.h>
#include <benchmark/Workload.h>

namespace benchmark {

struct RandomLookupInput {
  benchmark::Database* db;
  uint64_t keyCount;
  uint64_t lookupCount;
  std::atomic<uint64_t>* opCount;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  RandomLookupInput(benchmark::Database* d, uint64_t k, uint64_t l,
                    std::atomic<uint64_t>* ops, std::atomic<uint64_t>* done,
                    std::atomic<uint64_t>* errored)
      : db(d),
        keyCount(k),
        lookupCount(l),
        opCount(ops),
        workersDone(done),
        workersErrored(errored) {}
};

class RandomLookup : public Workload {
 public:
  RandomLookup(WorkloadInitializationData const& data);
  ~RandomLookup();

 protected:
  std::string resultsHeader();
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void lookupRandom(void* input, qdigest::QDigest* opDigest,
                           timepoint* start, timepoint* end);
};
}

#endif
