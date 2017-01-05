#ifndef ROCKSDB_BENCHMARK_ARANGO_COMPARATOR_H
#define ROCKSDB_BENCHMARK_ARANGO_COMPARATOR_H 1

#include <benchmark/utility.h>
#include <rocksdb/comparator.h>
#include <rocksdb/slice.h>
#include <velocypack/vpack.h>
#include <string>

namespace benchmark {

class ArangoComparator final : public rocksdb::Comparator {
 private:
  typedef arangodb::velocypack::Slice VPackSlice;

  const std::string _name;
  const uint64_t _sluggedLength;

 public:
  ArangoComparator();
  ~ArangoComparator();

  int Compare(rocksdb::Slice const& lhs, rocksdb::Slice const& rhs) const;

  char const* Name() const;

  void FindShortestSeparator(std::string*, rocksdb::Slice const&) const {}
  void FindShortSuccessor(std::string*) const {}

 private:
  int comparePrefixes(rocksdb::Slice const& lhs,
                      rocksdb::Slice const& rhs) const;
  int compareDocumentEntries(rocksdb::Slice const& lhs,
                             rocksdb::Slice const& rhs) const;
  int compareIndexEntries(rocksdb::Slice const& lhs,
                          rocksdb::Slice const& rhs) const;
  int compareSlugEntries(rocksdb::Slice const& lhs,
                         rocksdb::Slice const& rhs) const;
  int compareSlices(VPackSlice const& lhs, VPackSlice const& rhs) const;
  VPackSlice extractKeySlice(rocksdb::Slice const& slice) const;
};
}  // namespace benchmark

#endif
