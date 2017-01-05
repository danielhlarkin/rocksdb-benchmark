#ifndef ROCKSDB_BENCHMARK_UTILITY_H
#define ROCKSDB_BENCHMARK_UTILITY_H 1

#include <benchmark/RandomNumber.h>
#include <stdint.h>
#include <velocypack/vpack.h>
#include <chrono>
#include <string>

namespace benchmark {
namespace utility {
std::string shortToString(uint32_t n);
std::string intToString(uint64_t n);
uint32_t stringToShort(char const* c);
uint64_t stringToInt(char const* c);
arangodb::velocypack::SliceContainer generateDocument(std::string const& key);
arangodb::velocypack::SliceContainer generateDocument(std::string const& key,
                                                      std::time_t timestamp);
arangodb::velocypack::SliceContainer generatePrimarySlice();
arangodb::velocypack::SliceContainer generateSecondarySlice(
    std::time_t timestamp);
arangodb::velocypack::SliceContainer generateNextSecondarySlice(
    arangodb::velocypack::Slice const& value);

std::string hexDump(std::string const& input);
}
}

#endif
