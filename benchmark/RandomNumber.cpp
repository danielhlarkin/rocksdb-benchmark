#include <benchmark/RandomNumber.h>

using namespace benchmark;

Mutex RandomNumber::_mutex;
uint64_t RandomNumber::_current = 0;

RandomNumber::RandomNumber(uint64_t seed) {
  MUTEX_LOCKER(lock, _mutex);
  if (_current == 0) {
    _current = seed;
  }
}

uint64_t RandomNumber::next() {
  MUTEX_LOCKER(lock, _mutex);
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
