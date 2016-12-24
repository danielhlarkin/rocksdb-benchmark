#include <benchmark/utility.h>

#include <iostream>

using namespace benchmark;
using namespace arangodb::velocypack;

static RandomNumber r(0xdeadbeefdeadbeefULL);

std::string utility::intToString(uint64_t n) {
  uint64_t r = __builtin_bswap64(n);  // assuming little-endian system
  return std::string(reinterpret_cast<char const*>(&r), sizeof(uint64_t));
}

uint64_t utility::stringToInt(char const* c) {
  uint64_t r = *reinterpret_cast<uint64_t const*>(c);
  return __builtin_bswap64(r);
}

SliceContainer utility::generateDocument(std::string const& key) {
  std::chrono::system_clock clock;
  auto now = clock.now();
  auto timestamp = std::chrono::system_clock::to_time_t(now);

  return utility::generateDocument(key, timestamp);
}

SliceContainer utility::generateDocument(std::string const& key,
                                         std::time_t timestamp) {
  Builder b;

  uint64_t value = r.next();
  uint64_t nameValue = r.next();
  std::string name(reinterpret_cast<char const*>(&nameValue), sizeof(uint64_t));

  b.add(Value(ValueType::Object));
  b.add("_key", Value(key));
  b.add("value", Value(value));
  b.add("name", Value(name.data()));
  b.add("timestamp", Value(timestamp));
  b.close();

  return SliceContainer(b.slice());
}

SliceContainer utility::generatePrimarySlice() {
  return SliceContainer(Slice::noneSlice());
}

SliceContainer utility::generateSecondarySlice(std::time_t timestamp) {
  Builder b;

  b.add(Value(ValueType::Array));
  b.add(Value(timestamp));
  b.close();

  return SliceContainer(b.slice());
}

SliceContainer utility::generateNextSecondarySlice(Slice const& value) {
  Builder b;

  std::time_t timestamp = value.at(0).getNumericValue<std::time_t>();
  timestamp++;

  b.add(Value(ValueType::Array));
  b.add(Value(timestamp));
  b.close();

  return SliceContainer(b.slice());
}
