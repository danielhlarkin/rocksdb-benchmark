#include <benchmark/ArangoPrefixTransform.h>

using namespace benchmark;

ArangoPrefixTransform::ArangoPrefixTransform()
    : _name("ArangoDB Prefix Transform"),
      _dLength(1 + (2 * sizeof(uint64_t))),
      _iLength(1 + (3 * sizeof(uint64_t))) {}

ArangoPrefixTransform::~ArangoPrefixTransform() {}

const char* ArangoPrefixTransform::Name() const { return _name.data(); };

rocksdb::Slice ArangoPrefixTransform::Transform(
    const rocksdb::Slice& key) const {
  char type = key[0];
  switch (type) {
    case 'd': {
      return rocksdb::Slice(key.data(), _dLength);
    }
    case 'i': {
      return rocksdb::Slice(key.data(), _iLength);
    }
    default: { return rocksdb::Slice(); }
  }
}

bool ArangoPrefixTransform::InDomain(const rocksdb::Slice& key) const {
  char type = key[0];
  switch (type) {
    case 'd': {
      return (key.size() > _dLength);
    }
    case 'i': {
      return (key.size() > _iLength);
    }
    default: { return false; }
  }
}

bool ArangoPrefixTransform::InRange(const rocksdb::Slice& dst) const {
  char type = dst[0];
  switch (type) {
    case 'd': {
      return (dst.size() == _dLength);
    }
    case 'i': {
      return (dst.size() == _iLength);
    }
    default: { return false; }
  }
}
