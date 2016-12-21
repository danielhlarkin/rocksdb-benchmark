#include <benchmark/RandomNumber.h>

using namespace benchmark;

RandomNumber::RandomNumber(uint64_t seed) : _current(seed) {}

uint64_t RandomNumber::next() {
  // https://en.wikipedia.org/wiki/Linear_congruential_generator
  _current = (48271 * _current % 2147483647);
  return _current;
}

uint64_t RandomNumber::nextInRange(int range) {
  if (range == 0) {
    return 0;
  }
  next();
  return next() % range;
}
