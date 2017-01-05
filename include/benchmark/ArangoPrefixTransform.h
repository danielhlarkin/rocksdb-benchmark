#ifndef ROCKSDB_BENCHMARK_ARANGO_PREFIX_TRANSFORM_H
#define ROCKSDB_BENCHMARK_ARANGO_PREFIX_TRANSFORM_H 1

#include <rocksdb/slice.h>
#include <rocksdb/slice_transform.h>
#include <string>

namespace benchmark {

class ArangoPrefixTransform final : public rocksdb::SliceTransform {
 private:
  const std::string _name;
  const uint64_t _sluggedLength;

 public:
  ArangoPrefixTransform();
  ~ArangoPrefixTransform();

  const char* Name() const;
  rocksdb::Slice Transform(rocksdb::Slice const& key) const;
  bool InDomain(rocksdb::Slice const& key) const;
  bool InRange(rocksdb::Slice const& dst) const;
};
}  // namespace benchmark

#endif
