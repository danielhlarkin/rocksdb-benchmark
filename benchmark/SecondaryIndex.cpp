#include <benchmark/SecondaryIndex.h>

using namespace benchmark;
SecondaryIndex::SecondaryIndex(std::string const& folder,
                               std::string const& prefix)
    : _instance(folder),
      _db(_instance.db()),
      _cmp(_instance.comparator()),
      _prefix(prefix),
      _tombstone("1"),
      _nonTombstone("\0") {}

SecondaryIndex::~SecondaryIndex() {}

std::string SecondaryIndex::buildPrefix(uint64_t databaseId,
                                        uint64_t collectionId,
                                        uint64_t indexId) {
  return std::string("i")
      .append(utility::intToString(databaseId))
      .append(utility::intToString(collectionId))
      .append(utility::intToString(indexId));
}

bool SecondaryIndex::insert(std::string const& key, uint64_t revision,
                            VPackSlice const& value, bool tombstone) {
  // uniqueness/validity check
  // seek to maxRevision and work backwards checking tombstone
  std::string keyPrefix = buildKeyPrefix(key, value);
  std::string sentinel = buildKeySentinel(key, value);
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

  std::string rocksKey = buildKey(key, revision, value);
  auto status =
      _db->Put(_writeOptions, rocksKey, tombstone ? _tombstone : _nonTombstone);
  return status.ok();
}

bool SecondaryIndex::remove(std::string const& key, uint64_t revision,
                            VPackSlice const& value) {
  std::string rocksKey = buildKey(key, revision, value);
  auto status = _db->Delete(_writeOptions, rocksKey);
  return status.ok();
}

std::vector<uint64_t> SecondaryIndex::lookup(VPackSlice const& start,
                                             VPackSlice const& end,
                                             uint64_t maxRevision) const {
  std::vector<uint64_t> revisions;

  // seek to maxRevision and work backwards checking tombstone
  std::string valuePrefix = buildValuePrefix(start);
  std::string endSentinel = buildValueSentinel(end);
  rocksdb::Slice prefixSlice(valuePrefix);
  auto it = _db->NewIterator(_readOptions);
  for (it->SeekForPrev(endSentinel);
       it->Valid() && (*_cmp).Compare(it->key(), prefixSlice) >= 0;
       it->Prev()) {
    if (unwrap(it) > maxRevision) {
      continue;  // revision too new, doesn't count
    }
    while (isTombstoned(it)) {  // all previous entries with same value/key pair
      // are invalid so we skip back until we find a
      // different pair
      std::string keyPrefix = buildKeyPrefix(it);
      for (; it->Valid() && it->key().starts_with(keyPrefix); it->Prev()) {
      }
      if (!it->Valid() || (*_cmp).Compare(it->key(), prefixSlice) < 0) {
        // this was the last valid pair; get out
        delete it;
        std::reverse(std::begin(revisions),
                     std::end(revisions));  // list was built in reverse order
        return revisions;
      }
    }

    uint64_t r = unwrap(it);
    if (r <= maxRevision) {
      revisions.emplace_back(r);  // found a good one; continue
    }
  }

  delete it;
  std::reverse(std::begin(revisions),
               std::end(revisions));  // list was built in reverse order
  return revisions;
}

std::string SecondaryIndex::buildKey(std::string const& key, uint64_t revision,
                                     VPackSlice const& value) const {
  return std::string(_prefix)
      .append(reinterpret_cast<const char*>(value.begin()), value.byteSize())
      .append(key)
      .append(utility::intToString(revision));
}

std::string SecondaryIndex::buildKeyPrefix(std::string const& key,
                                           VPackSlice const& value) const {
  return std::string(_prefix)
      .append(reinterpret_cast<char const*>(value.begin()), value.byteSize())
      .append(key);
}

std::string SecondaryIndex::buildKeyPrefix(rocksdb::Iterator const* it) const {
  return std::string(it->key().data(), (it->key().size() - sizeof(uint64_t)));
}

std::string SecondaryIndex::buildKeySentinel(std::string const& key,
                                             VPackSlice const& value) const {
  return buildKeyPrefix(key, value)
      .append(utility::intToString(0xFFFFFFFFFFFFFFFFULL));
}

std::string SecondaryIndex::buildValuePrefix(VPackSlice const& value) const {
  return std::string(_prefix).append(
      reinterpret_cast<char const*>(value.begin()), value.byteSize());
}

std::string SecondaryIndex::buildValueSentinel(VPackSlice const& value) const {
  VPackSliceContainer nextValue = utility::generateNextSecondarySlice(value);
  return buildValuePrefix(nextValue.slice());
}

bool SecondaryIndex::isTombstoned(rocksdb::Iterator const* it) const {
  return (std::string(reinterpret_cast<char const*>(it->value().data()),
                      it->value().size()) == _tombstone);
}

SecondaryIndex::VPackSlice SecondaryIndex::extract(
    rocksdb::Iterator const* it) const {
  char const* base = it->key().data() + _prefix.size();
  return VPackSlice(base);
}

std::time_t SecondaryIndex::timestamp(VPackSlice const& value) const {
  return value.at(0).getNumericValue<std::time_t>();
}

std::time_t SecondaryIndex::timestamp(std::string const& s) const {
  return timestamp(extract(s));
}

std::time_t SecondaryIndex::timestamp(rocksdb::Iterator const* it) const {
  return timestamp(extract(it));
}

SecondaryIndex::VPackSlice SecondaryIndex::extract(std::string const& s) const {
  char const* base = s.data() + _prefix.size();
  return VPackSlice(base);
}

uint64_t SecondaryIndex::unwrap(rocksdb::Iterator const* it) const {
  char const* base = it->key().data() + it->key().size() - sizeof(uint64_t);
  return utility::stringToInt(base);
}
