#include <benchmark/Database.h>

using namespace benchmark;
Database::Database(std::string const& folder)
    : _folder(folder),
      _random(0xdeadbeefdeadbeefULL),
      _databaseId(_random.next()),
      _collectionId(_random.next()),
      _primaryIndexId(_random.next()),
      _secondaryIndexId(_random.next()),
      _instance(_folder),
      _db(_instance.db()),
      _cmp(_instance.comparator()),
      _writeOptions(rocksdb::WriteOptions()),
      _txOptions(rocksdb::OptimisticTransactionOptions()),
      _storageEngine(_instance.getDocumentSlug(_databaseId, _collectionId)),
      _primaryIndex(
          _instance.getIndexSlug(_databaseId, _collectionId, _primaryIndexId)),
      _secondaryIndex(
          _instance.getIndexSlug(_databaseId, _collectionId, _secondaryIndexId),
          _cmp) {}

Database::~Database() {}

uint64_t Database::now() { return _clock.getTimeStamp(); }

uint64_t Database::insert(std::string const& key, VPackSlice const& value) {
  uint64_t revision = _clock.getTimeStamp();
  auto tx = _db->BeginTransaction(_writeOptions, _txOptions);

  bool ok = insertInternal(tx, revision, key, value);
  if (!ok) {
    tx->Rollback();
    delete tx;
    return 0;
  }

  auto status = tx->Commit();
  if (!status.ok()) {
    tx->Rollback();
    delete tx;
    return 0;
  }
  delete tx;

  return revision;
}

std::vector<uint64_t> Database::insertBatch(
    std::vector<std::string> const& keys,
    std::vector<VPackSliceContainer> const& values) {
  std::vector<uint64_t> revisions;
  if (keys.size() != values.size()) {
    return revisions;
  }

  bool ok = true;
  auto tx = _db->BeginTransaction(_writeOptions, _txOptions);
  for (size_t i = 0; i < keys.size(); i++) {
    revisions.emplace_back(_clock.getTimeStamp());
    ok = insertInternal(tx, revisions[i], keys[i], values[i].slice());
    if (!ok) {
      break;
    }
  }
  if (!ok) {
    tx->Rollback();
    delete tx;
    revisions.clear();
    return revisions;
  }

  auto status = tx->Commit();
  if (!status.ok()) {
    tx->Rollback();
    delete tx;
    revisions.clear();
    return revisions;
  }
  delete tx;

  return revisions;
}

bool Database::insertInternal(rocksdb::Transaction* tx, uint64_t revision,
                              std::string const& key, VPackSlice const& value) {
  bool ok = _primaryIndex.insert(tx, key, revision, false);
  if (!ok) {
    return false;  // key already exists
  }

  ok = _storageEngine.insert(tx, revision, value);
  if (!ok) {
    return false;
  }

  std::time_t timestamp = value.get("timestamp").getNumber<std::time_t>();
  VPackSliceContainer timestampSlice =
      utility::generateSecondarySlice(timestamp);
  ok = _secondaryIndex.insert(tx, key, revision, timestampSlice.slice(), false);
  if (!ok) {
    return false;
  }

  return true;
}

bool Database::remove(std::string const& key) {
  uint64_t revision = _clock.getTimeStamp();
  auto tx = _db->BeginTransaction(_writeOptions, _txOptions);

  uint64_t existingRevision = _primaryIndex.lookup(tx, key, revision);
  if (existingRevision != 0) {
    VPackSliceContainer value = _storageEngine.lookup(tx, existingRevision);
    bool ok = _primaryIndex.insert(tx, key, revision, true);
    if (!ok) {
      tx->Rollback();
      delete tx;
      return false;
    }

    std::time_t timestamp =
        value.slice().get("timestamp").getNumber<std::time_t>();
    VPackSliceContainer timestampSlice =
        utility::generateSecondarySlice(timestamp);
    ok =
        _secondaryIndex.insert(tx, key, revision, timestampSlice.slice(), true);
    if (!ok) {
      tx->Rollback();
      delete tx;
      return false;
    }

    auto status = tx->Commit();
    if (!status.ok()) {
      tx->Rollback();
    }
    delete tx;

    return true;
  }
  delete tx;

  return false;
}

Database::VPackSliceContainer Database::lookupSingle(
    std::string const& key, uint64_t maxRevision) const {
  auto tx = _db->BeginTransaction(_writeOptions, _txOptions);

  uint64_t r = _primaryIndex.lookup(tx, key, maxRevision);
  VPackSliceContainer document = (r == 0) ? VPackSliceContainer(VPackSlice())
                                          : _storageEngine.lookup(tx, r);
  delete tx;

  return document;
}

std::vector<Database::VPackSliceContainer> Database::lookupRange(
    VPackSlice const& start, VPackSlice const& end,
    uint64_t maxRevision) const {
  std::vector<VPackSliceContainer> documents;
  auto tx = _db->BeginTransaction(_writeOptions, _txOptions);

  std::vector<uint64_t> revisions =
      _secondaryIndex.lookup(tx, start, end, maxRevision);
  for (int i = 0; i < revisions.size(); i++) {
    documents.emplace_back(_storageEngine.lookup(tx, revisions[i]));
  }

  delete tx;

  return documents;
}
