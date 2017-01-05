#include <benchmark/ArangoComparator.h>

using namespace benchmark;
using namespace arangodb::velocypack;

ArangoComparator::ArangoComparator()
    : _name("ArangoDB Comparator"), _sluggedLength(1 + sizeof(uint32_t)) {}

ArangoComparator::~ArangoComparator() {}

const char* ArangoComparator::Name() const { return _name.data(); };

int ArangoComparator::Compare(rocksdb::Slice const& lhs,
                              rocksdb::Slice const& rhs) const {
  int result = comparePrefixes(lhs, rhs);
  if (result != 0 || lhs.size() == 0) {
    return result;
  }

  char type = lhs[0];
  switch (type) {
    case 'd': {
      return compareDocumentEntries(lhs, rhs);
    }
    case 'i': {
      return compareIndexEntries(lhs, rhs);
    }
    case 'S': {
      return compareSlugEntries(lhs, rhs);
    }
    default: { return 0; }
  }
}

int ArangoComparator::comparePrefixes(rocksdb::Slice const& lhs,
                                      rocksdb::Slice const& rhs) const {
  if (lhs.size() == 0 || rhs.size() == 0) {
    if (lhs.size() == 0 && rhs.size() != 0) {
      return -1;
    }
    if (lhs.size() != 0 && rhs.size() == 0) {
      return 1;
    }
    return 0;
  }

  int result = lhs[0] - rhs[0];
  if (result != 0) {  // different types
    return result;
  }

  char type = lhs[0];
  switch (type) {
    case 'd':
    case 'i': {
      return memcmp(lhs.data(), rhs.data(), _sluggedLength);
    }
    case 'S':
    default: { return 0; }
  }
}

int ArangoComparator::compareDocumentEntries(rocksdb::Slice const& lhs,
                                             rocksdb::Slice const& rhs) const {
  uint64_t offset = _sluggedLength;
  if (lhs.size() == offset) {
    return 0;  // comparing two prefixes, all done
  }

  return memcmp(lhs.data() + offset, rhs.data() + offset, sizeof(uint64_t));
}

int ArangoComparator::compareSlugEntries(rocksdb::Slice const& lhs,
                                         rocksdb::Slice const& rhs) const {
  uint64_t minLength = (lhs.size() <= rhs.size()) ? lhs.size() : rhs.size();
  int result = memcmp(lhs.data(), rhs.data(), minLength);
  if (result != 0) {
    return result;
  }

  return (int)((int64_t)lhs.size() - (int64_t)rhs.size());
}

int ArangoComparator::compareIndexEntries(rocksdb::Slice const& lhs,
                                          rocksdb::Slice const& rhs) const {
  uint64_t offset = _sluggedLength;
  if (lhs.size() == offset) {
    return 0;  // comparing two prefixes, all done
  }

  VPackSlice const lSlice = extractKeySlice(lhs.data() + offset);
  VPackSlice const rSlice = extractKeySlice(rhs.data() + offset);

  int result = compareSlices(lSlice, rSlice);
  if (result != 0) {
    return result;
  }

  uint64_t sliceOffset = lSlice.byteSize();
  char const* lBase = lhs.data() + offset + sliceOffset;
  char const* rBase = rhs.data() + offset + sliceOffset;
  uint64_t minLength = ((lhs.size() <= rhs.size()) ? lhs.size() : rhs.size()) -
                       offset - sliceOffset;

  result = memcmp(lBase, rBase, minLength);
  if (result != 0) {
    return result;
  }

  if (lhs.size() == rhs.size()) {
    return 0;
  }

  return (lhs.size() < rhs.size()) ? -1 : 1;
}

static int8_t const TypeWeights[256] = {
    0 /* 0x00 */,   4 /* 0x01 */,  4 /* 0x02 */, 4 /* 0x03 */,  4 /* 0x04 */,
    4 /* 0x05 */,   4 /* 0x06 */,  4 /* 0x07 */, 4 /* 0x08 */,  4 /* 0x09 */,
    5 /* 0x0a */,   5 /* 0x0b */,  5 /* 0x0c */, 5 /* 0x0d */,  5 /* 0x0e */,
    5 /* 0x0f */,   5 /* 0x10 */,  5 /* 0x11 */, 5 /* 0x12 */,  4 /* 0x13 */,
    5 /* 0x14 */,   0 /* 0x15 */,  0 /* 0x16 */, -1 /* 0x17 */, 0 /* 0x18 */,
    1 /* 0x19 */,   1 /* 0x1a */,  2 /* 0x1b */, 2 /* 0x1c */,  -50 /* 0x1d */,
    -99 /* 0x1e */, 99 /* 0x1f */, 2 /* 0x20 */, 2 /* 0x21 */,  2 /* 0x22 */,
    2 /* 0x23 */,   2 /* 0x24 */,  2 /* 0x25 */, 2 /* 0x26 */,  2 /* 0x27 */,
    2 /* 0x28 */,   2 /* 0x29 */,  2 /* 0x2a */, 2 /* 0x2b */,  2 /* 0x2c */,
    2 /* 0x2d */,   2 /* 0x2e */,  2 /* 0x2f */, 2 /* 0x30 */,  2 /* 0x31 */,
    2 /* 0x32 */,   2 /* 0x33 */,  2 /* 0x34 */, 2 /* 0x35 */,  2 /* 0x36 */,
    2 /* 0x37 */,   2 /* 0x38 */,  2 /* 0x39 */, 2 /* 0x3a */,  2 /* 0x3b */,
    2 /* 0x3c */,   2 /* 0x3d */,  2 /* 0x3e */, 2 /* 0x3f */,  3 /* 0x40 */,
    3 /* 0x41 */,   3 /* 0x42 */,  3 /* 0x43 */, 3 /* 0x44 */,  3 /* 0x45 */,
    3 /* 0x46 */,   3 /* 0x47 */,  3 /* 0x48 */, 3 /* 0x49 */,  3 /* 0x4a */,
    3 /* 0x4b */,   3 /* 0x4c */,  3 /* 0x4d */, 3 /* 0x4e */,  3 /* 0x4f */,
    3 /* 0x50 */,   3 /* 0x51 */,  3 /* 0x52 */, 3 /* 0x53 */,  3 /* 0x54 */,
    3 /* 0x55 */,   3 /* 0x56 */,  3 /* 0x57 */, 3 /* 0x58 */,  3 /* 0x59 */,
    3 /* 0x5a */,   3 /* 0x5b */,  3 /* 0x5c */, 3 /* 0x5d */,  3 /* 0x5e */,
    3 /* 0x5f */,   3 /* 0x60 */,  3 /* 0x61 */, 3 /* 0x62 */,  3 /* 0x63 */,
    3 /* 0x64 */,   3 /* 0x65 */,  3 /* 0x66 */, 3 /* 0x67 */,  3 /* 0x68 */,
    3 /* 0x69 */,   3 /* 0x6a */,  3 /* 0x6b */, 3 /* 0x6c */,  3 /* 0x6d */,
    3 /* 0x6e */,   3 /* 0x6f */,  3 /* 0x70 */, 3 /* 0x71 */,  3 /* 0x72 */,
    3 /* 0x73 */,   3 /* 0x74 */,  3 /* 0x75 */, 3 /* 0x76 */,  3 /* 0x77 */,
    3 /* 0x78 */,   3 /* 0x79 */,  3 /* 0x7a */, 3 /* 0x7b */,  3 /* 0x7c */,
    3 /* 0x7d */,   3 /* 0x7e */,  3 /* 0x7f */, 3 /* 0x80 */,  3 /* 0x81 */,
    3 /* 0x82 */,   3 /* 0x83 */,  3 /* 0x84 */, 3 /* 0x85 */,  3 /* 0x86 */,
    3 /* 0x87 */,   3 /* 0x88 */,  3 /* 0x89 */, 3 /* 0x8a */,  3 /* 0x8b */,
    3 /* 0x8c */,   3 /* 0x8d */,  3 /* 0x8e */, 3 /* 0x8f */,  3 /* 0x90 */,
    3 /* 0x91 */,   3 /* 0x92 */,  3 /* 0x93 */, 3 /* 0x94 */,  3 /* 0x95 */,
    3 /* 0x96 */,   3 /* 0x97 */,  3 /* 0x98 */, 3 /* 0x99 */,  3 /* 0x9a */,
    3 /* 0x9b */,   3 /* 0x9c */,  3 /* 0x9d */, 3 /* 0x9e */,  3 /* 0x9f */,
    3 /* 0xa0 */,   3 /* 0xa1 */,  3 /* 0xa2 */, 3 /* 0xa3 */,  3 /* 0xa4 */,
    3 /* 0xa5 */,   3 /* 0xa6 */,  3 /* 0xa7 */, 3 /* 0xa8 */,  3 /* 0xa9 */,
    3 /* 0xaa */,   3 /* 0xab */,  3 /* 0xac */, 3 /* 0xad */,  3 /* 0xae */,
    3 /* 0xaf */,   3 /* 0xb0 */,  3 /* 0xb1 */, 3 /* 0xb2 */,  3 /* 0xb3 */,
    3 /* 0xb4 */,   3 /* 0xb5 */,  3 /* 0xb6 */, 3 /* 0xb7 */,  3 /* 0xb8 */,
    3 /* 0xb9 */,   3 /* 0xba */,  3 /* 0xbb */, 3 /* 0xbc */,  3 /* 0xbd */,
    3 /* 0xbe */,   3 /* 0xbf */,  3 /* 0xc0 */, 3 /* 0xc1 */,  3 /* 0xc2 */,
    3 /* 0xc3 */,   3 /* 0xc4 */,  3 /* 0xc5 */, 3 /* 0xc6 */,  3 /* 0xc7 */,
    2 /* 0xc8 */,   2 /* 0xc9 */,  2 /* 0xca */, 2 /* 0xcb */,  2 /* 0xcc */,
    2 /* 0xcd */,   2 /* 0xce */,  2 /* 0xcf */, 2 /* 0xd0 */,  2 /* 0xd1 */,
    2 /* 0xd2 */,   2 /* 0xd3 */,  2 /* 0xd4 */, 2 /* 0xd5 */,  2 /* 0xd6 */,
    2 /* 0xd7 */,   0 /* 0xd8 */,  0 /* 0xd9 */, 0 /* 0xda */,  0 /* 0xdb */,
    0 /* 0xdc */,   0 /* 0xdd */,  0 /* 0xde */, 0 /* 0xdf */,  0 /* 0xe0 */,
    0 /* 0xe1 */,   0 /* 0xe2 */,  0 /* 0xe3 */, 0 /* 0xe4 */,  0 /* 0xe5 */,
    0 /* 0xe6 */,   0 /* 0xe7 */,  0 /* 0xe8 */, 0 /* 0xe9 */,  0 /* 0xea */,
    0 /* 0xeb */,   0 /* 0xec */,  0 /* 0xed */, 0 /* 0xee */,  0 /* 0xef */,
    3 /* 0xf0 */,   3 /* 0xf1 */,  3 /* 0xf2 */, 3 /* 0xf3 */,  3 /* 0xf4 */,
    3 /* 0xf5 */,   3 /* 0xf6 */,  3 /* 0xf7 */, 3 /* 0xf8 */,  3 /* 0xf9 */,
    3 /* 0xfa */,   3 /* 0xfb */,  3 /* 0xfc */, 3 /* 0xfd */,  3 /* 0xfe */,
    3 /* 0xff */,
};

static inline int8_t TypeWeight(Slice const& slice) {
  return TypeWeights[slice.head()];
}

static int compareNumberValues(ValueType lhsType, Slice const& lhs,
                               Slice const& rhs) {
  if (lhsType == rhs.type()) {
    // both types are equal
    if (lhsType == ValueType::Int || lhsType == ValueType::SmallInt) {
      // use exact comparisons. no need to cast to double
      int64_t l = lhs.getInt();
      int64_t r = rhs.getInt();
      if (l == r) {
        return 0;
      }
      return (l < r ? -1 : 1);
    }

    if (lhsType == ValueType::UInt) {
      // use exact comparisons. no need to cast to double
      uint64_t l = lhs.getUInt();
      uint64_t r = rhs.getUInt();
      if (l == r) {
        return 0;
      }
      return (l < r ? -1 : 1);
    }
    // fallthrough to double comparison
  }

  double left = lhs.getNumericValue<double>();
  double right = rhs.getNumericValue<double>();
  if (left == right) {
    return 0;
  }
  return (left < right ? -1 : 1);
}

int ArangoComparator::compareSlices(VPackSlice const& lhs,
                                    VPackSlice const& rhs) const {
  {
    int8_t lWeight = TypeWeight(lhs);
    int8_t rWeight = TypeWeight(rhs);

    if (lWeight < rWeight) {
      return -1;
    }

    if (lWeight > rWeight) {
      return 1;
    }
  }

  // lhs and rhs have equal weights

  // note that the following code would be redundant because it was already
  // checked that lhs & rhs have the same TypeWeight, which is 0 for none.
  //  and for TypeWeight 0 we always return value 0
  // if (lhs.isNone() || rhs.isNone()) {
  //   // if rhs is none. we cannot be sure here that both are nones.
  //   // there can also exist the situation that lhs is a none and rhs is a
  //   // null value
  //   // (or vice versa). Anyway, the compare value is the same for both,
  //   return 0;
  // }

  auto lhsType = lhs.type();

  switch (lhsType) {
    case ValueType::Illegal:
    case ValueType::MinKey:
    case ValueType::MaxKey:
    case ValueType::None:
    case ValueType::Null:
      return 0;
    case ValueType::Bool: {
      bool left = lhs.getBoolean();
      bool right = rhs.getBoolean();
      if (left == right) {
        return 0;
      }
      if (!left) {
        return -1;
      }
      return 1;
    }
    case ValueType::Double:
    case ValueType::Int:
    case ValueType::UInt:
    case ValueType::SmallInt: {
      return compareNumberValues(lhsType, lhs, rhs);
    }
    case ValueType::Array: {
      ArrayIterator al(lhs);
      ArrayIterator ar(rhs);

      ValueLength const n = (std::max)(al.size(), ar.size());
      for (ValueLength i = 0; i < n; ++i) {
        Slice lhsValue;
        Slice rhsValue;

        if (i < al.size()) {
          lhsValue = al.value();
          al.next();
        }
        if (i < ar.size()) {
          rhsValue = ar.value();
          ar.next();
        }

        int result = compareSlices(lhsValue, rhsValue);
        if (result != 0) {
          return result;
        }
      }

      return 0;
    }
    default:
      // Contains all other ValueTypes of VelocyPack.
      // They are not used in ArangoDB so this cannot occur
      return 0;
  }
}

Slice ArangoComparator::extractKeySlice(rocksdb::Slice const& slice) const {
  return Slice(slice.data());
}
