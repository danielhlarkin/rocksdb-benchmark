#ifndef ROCKSDB_BENCHMARK_INSERT_H
#define ROCKSDB_BENCHMARK_INSERT_H 1

#include <benchmark/RandomNumber.h>
#include <benchmark/Workload.h>

namespace benchmark {

struct InsertInput {
  benchmark::Database* db;
  benchmark::Mutex* lock;
  uint64_t lower;
  uint64_t upper;
  std::time_t* minTimestamp;
  std::time_t* maxTimestamp;
  std::atomic<uint64_t>* opCount;
  std::atomic<uint64_t>* workersDone;
  std::atomic<uint64_t>* workersErrored;
  InsertInput(benchmark::Database* d, benchmark::Mutex* lk, uint64_t l,
              uint64_t u, std::time_t* minT, std::time_t* maxT,
              std::atomic<uint64_t>* ops, std::atomic<uint64_t>* done,
              std::atomic<uint64_t>* errored)
      : db(d),
        lock(lk),
        lower(l),
        upper(u),
        minTimestamp(minT),
        maxTimestamp(maxT),
        opCount(ops),
        workersDone(done),
        workersErrored(errored) {}
};

class Insert : public Workload {
 public:
  Insert(WorkloadInitializationData const& data);
  ~Insert();

 protected:
  std::string operationName();

  void* generateWorkerInput(uint64_t i);
  WorkerType worker();

  static void insertBatch(void* input, qdigest::QDigest* opDigest,
                          timepoint* start, timepoint* end);
};
}

#endif
