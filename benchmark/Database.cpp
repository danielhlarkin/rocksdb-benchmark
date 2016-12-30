#include <benchmark/Database.h>

using namespace benchmark;
Database::Database(std::string const& folder)
    : _folder(folder),
      _random(0xdeadbeefdeadbeefULL),
      _databaseId(_random.next()),
      _collectionId(_random.next()),
      _primaryIndexId(_random.next()),
      _secondaryIndexId(_random.next()),
      _storageEngine(_folder,
                     StorageEngine::buildPrefix(_databaseId, _collectionId)),
      _primaryIndex(_folder, PrimaryIndex::buildPrefix(
                                 _databaseId, _collectionId, _primaryIndexId)),
      _secondaryIndex(_folder,
                      SecondaryIndex::buildPrefix(_databaseId, _collectionId,
                                                  _secondaryIndexId)) {}

Database::~Database() {}

uint64_t Database::now() { return _clock.getTimeStamp(); }

uint64_t Database::insert(std::string const& key, VPackSlice const& value) {
  uint64_t revision = _clock.getTimeStamp();

  bool ok = _primaryIndex.insert(key, revision, false);
  if (!ok) {
    return 0;  // key already exists
  }

  ok = _storageEngine.insert(revision, value);
  if (!ok) {
    _primaryIndex.remove(key, revision);
    return 0;
  }

  std::time_t timestamp = value.get("timestamp").getNumber<std::time_t>();
  VPackSliceContainer timestampSlice =
      utility::generateSecondarySlice(timestamp);
  ok = _secondaryIndex.insert(key, revision, timestampSlice.slice(), false);
  if (!ok) {
    _primaryIndex.remove(key, revision);
    _storageEngine.remove(revision);
    return 0;
  }

  return revision;
}

bool Database::remove(std::string const& key) {
  uint64_t revision = _clock.getTimeStamp();

  uint64_t existingRevision = _primaryIndex.lookup(key, revision);
  if (existingRevision != 0) {
    VPackSliceContainer value = _storageEngine.lookup(existingRevision);
    bool ok = _primaryIndex.insert(key, revision, true);
    if (!ok) {
      return false;
    }

    std::time_t timestamp =
        value.slice().get("timestamp").getNumber<std::time_t>();
    VPackSliceContainer timestampSlice =
        utility::generateSecondarySlice(timestamp);
    ok = _secondaryIndex.insert(key, revision, timestampSlice.slice(), true);
    if (!ok) {
      _primaryIndex.remove(key, revision);
      return false;
    }

    return true;
  }

  return false;
}

Database::VPackSliceContainer Database::lookupSingle(
    std::string const& key, uint64_t maxRevision) const {
  uint64_t r = _primaryIndex.lookup(key, maxRevision);
  return (r == 0) ? VPackSliceContainer(VPackSlice())
                  : _storageEngine.lookup(r);
}

std::vector<Database::VPackSliceContainer> Database::lookupRange(
    VPackSlice const& start, VPackSlice const& end,
    uint64_t maxRevision) const {
  std::vector<VPackSliceContainer> documents;

  std::vector<uint64_t> revisions =
      _secondaryIndex.lookup(start, end, maxRevision);
  for (int i = 0; i < revisions.size(); i++) {
    documents.emplace_back(_storageEngine.lookup(revisions[i]));
  }

  return documents;
}
