#include <benchmark/ArangoPrefixTransform.h>

using namespace benchmark;

ArangoPrefixTransform::ArangoPrefixTransform()
    : _name("ArangoDB Prefix Transform"),
      _sluggedLength(1 + sizeof(uint32_t)) {}

ArangoPrefixTransform::~ArangoPrefixTransform() {}

const char* ArangoPrefixTransform::Name() const { return _name.data(); };

rocksdb::Slice ArangoPrefixTransform::Transform(
    const rocksdb::Slice& key) const {
  char type = key[0];
  switch (type) {
    case 'd':
    case 'i': {
      return rocksdb::Slice(key.data(), _sluggedLength);
    }
    case 'S': {
      return rocksdb::Slice(key.data(), 1);
    }
    default: { return rocksdb::Slice(); }
  }
}

bool ArangoPrefixTransform::InDomain(const rocksdb::Slice& key) const {
  char type = key[0];
  switch (type) {
    case 'd':
    case 'i': {
      return (key.size() > _sluggedLength);
    }
    case 'S': {
      return (key.size() > 1);
    }
    default: { return false; }
  }
}

bool ArangoPrefixTransform::InRange(const rocksdb::Slice& dst) const {
  char type = dst[0];
  switch (type) {
    case 'd':
    case 'i': {
      return (dst.size() == _sluggedLength);
    }
    case 'S': {
      return (dst.size() == 1);
    }
    default: { return false; }
  }
}
