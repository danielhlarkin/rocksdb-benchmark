#include <benchmark/PrimaryIndex.h>
#include <chrono>
#include <thread>

using namespace benchmark;
PrimaryIndex::PrimaryIndex(uint32_t slug)
    : _prefix(buildPrefix(slug)),
      _readOptions(rocksdb::ReadOptions()),
      _tombstone("1"),
      _nonTombstone("\0") {}

PrimaryIndex::~PrimaryIndex() {}

std::string PrimaryIndex::buildPrefix(uint32_t slug) {
  return std::string("i").append(utility::shortToString(slug));
}

bool PrimaryIndex::insert(rocksdb::Transaction* tx, std::string const& key,
                          uint64_t revision, bool tombstone) {
  // uniqueness/validity check
  // seek to maxRevision and work backwards checking tombstone
  std::string keyPrefix = buildKeyPrefix(key);
  std::string sentinel = buildKeySentinel(key);
  auto it = tx->GetIterator(_readOptions);
  for (it->SeekForPrev(sentinel);
       it->Valid() && it->key().starts_with(keyPrefix); it->Prev()) {
    if (sameKey(it, key)) {
      if ((!tombstone && isTombstoned(it)) ||
          (tombstone && !isTombstoned(it))) {
        break;  // no matching entry; proceed to insert
      } else {
        delete it;
        return false;  // matching entry; don't insert
      }
    }
  }
  delete it;

  std::string rocksKey = buildKey(key, revision);
  auto status = tx->Put(rocksKey, tombstone ? _tombstone : _nonTombstone);
  return status.ok();
}

bool PrimaryIndex::remove(rocksdb::Transaction* tx, std::string const& key,
                          uint64_t revision) {
  std::string rocksKey = buildKey(key, revision);
  auto status = tx->Delete(rocksKey);
  return status.ok();
}

uint64_t PrimaryIndex::lookup(rocksdb::Transaction* tx, std::string const& key,
                              uint64_t maxRevision) const {
  // seek to maxRevision and work backwards checking tombstone
  std::string keyPrefix = buildKeyPrefix(key);
  std::string sentinel = buildKeySentinel(key, maxRevision);
  auto it = tx->GetIterator(_readOptions);
  for (it->SeekForPrev(sentinel);
       it->Valid() && it->key().starts_with(keyPrefix); it->Prev()) {
    if (sameKey(it, key)) {
      uint64_t r = isTombstoned(it) ? 0 : unwrap(it);
      delete it;
      return r;
    }
  }
  delete it;

  return 0;  // no instance of key with allowed revision in index
}

std::string PrimaryIndex::buildKey(std::string const& key,
                                   uint64_t revision) const {
  VPackSliceContainer value = utility::generatePrimarySlice();
  return std::string(_prefix)
      .append(reinterpret_cast<const char*>(value.begin()), value.byteSize())
      .append(key)
      .append(utility::intToString(revision));
}

std::string PrimaryIndex::buildKeyPrefix(std::string const& key) const {
  VPackSliceContainer value = utility::generatePrimarySlice();
  return std::string(_prefix)
      .append(reinterpret_cast<const char*>(value.begin()), value.byteSize())
      .append(key);
}

std::string PrimaryIndex::buildKeySentinel(std::string const& key,
                                           uint64_t maxRevision) const {
  VPackSliceContainer value = utility::generatePrimarySlice();
  return std::string(_prefix)
      .append(reinterpret_cast<const char*>(value.begin()), value.byteSize())
      .append(key)
      .append(utility::intToString(maxRevision));
}

bool PrimaryIndex::isTombstoned(rocksdb::Iterator const* it) const {
  return (std::string(reinterpret_cast<char const*>(it->value().data()),
                      it->value().size()) == _tombstone);
}

bool PrimaryIndex::sameKey(rocksdb::Iterator const* it,
                           std::string const& key) const {
  uint64_t prefixLength = _prefix.size() + 1;
  uint64_t suffixLength = sizeof(uint64_t);

  if (it->key().size() != prefixLength + key.size() + suffixLength) {
    return false;
  }

  char const* base = it->key().data() + prefixLength;
  return (memcmp(base, key.data(), key.size()) == 0);
}

uint64_t PrimaryIndex::unwrap(rocksdb::Iterator const* it) const {
  char const* base = it->key().data() + it->key().size() - sizeof(uint64_t);
  return utility::stringToInt(base);
}
