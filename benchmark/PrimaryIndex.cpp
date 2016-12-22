#include <benchmark/PrimaryIndex.h>

using namespace benchmark;
PrimaryIndex::PrimaryIndex(std::string const& folder, std::string const& prefix)
    : _instance(folder),
      _db(_instance.db()),
      _prefix(prefix),
      _tombstone("1"),
      _nonTombstone("\0") {}

PrimaryIndex::~PrimaryIndex() {}

std::string PrimaryIndex::buildPrefix(uint64_t databaseId,
                                      uint64_t collectionId, uint64_t indexId) {
  return std::string("i")
      .append(utility::intToString(databaseId))
      .append(utility::intToString(collectionId))
      .append(utility::intToString(indexId));
}

bool PrimaryIndex::insert(std::string const& key, uint64_t revision,
                          bool tombstone) {
  // uniqueness/validity check
  // seek to maxRevision and work backwards checking tombstone
  std::string keyPrefix = buildKeyPrefix(key);
  std::string sentinel = buildKeySentinel(key);
  auto it = _db->NewIterator(_readOptions);
  for (it->SeekForPrev(sentinel);
       it->Valid() && it->key().starts_with(keyPrefix); it->Prev()) {
    if ((!tombstone && isTombstoned(it)) || (tombstone && !isTombstoned(it))) {
      break;  // no matching entry; proceed to insert
    } else {
      delete it;
      return false;  // matching entry; don't insert
    }
  }
  delete it;

  std::string rocksKey = buildKey(key, revision);
  auto status =
      _db->Put(_writeOptions, rocksKey, tombstone ? _tombstone : _nonTombstone);
  return status.ok();
}

bool PrimaryIndex::remove(std::string const& key, uint64_t revision) {
  std::string rocksKey = buildKey(key, revision);
  auto status = _db->Delete(_writeOptions, rocksKey);
  return status.ok();
}

uint64_t PrimaryIndex::lookup(std::string const& key,
                              uint64_t maxRevision) const {
  // seek to maxRevision and work backwards checking tombstone
  std::string keyPrefix = buildKeyPrefix(key);
  std::string sentinel = buildKeySentinel(key, maxRevision);
  auto it = _db->NewIterator(_readOptions);
  for (it->SeekForPrev(sentinel);
       it->Valid() && it->key().starts_with(keyPrefix); it->Prev()) {
    uint64_t r = isTombstoned(it) ? 0 : unwrap(it);
    delete it;
    return r;
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

uint64_t PrimaryIndex::unwrap(rocksdb::Iterator const* it) const {
  char const* base = it->key().data() + it->key().size() - sizeof(uint64_t);
  return utility::stringToInt(base);
}
