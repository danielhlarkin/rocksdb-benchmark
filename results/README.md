# Results Summary

The full results for each data run can be found in the corresponding `.out` files. In particular, the naming convention is `results.[keyCount].[threadCount].out`, where the other parameters have been chosen programmatically based on `keyCount` (`lookupCount = keyCount * 5`, `hotsetCount = keyCount / 20`, and `rangeLimit = keyCount / 5`). We summarize results in the tables below. In each table, each row corresponds to the `keyCount` value given at left, and each column corresponds to the `threadCount` value given at the top. Throughput is measured in operations per second.

# BatchInsert performance

Insert a batch of documents in random order, splitting the work over all threads.

## Throughput

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 | 15528.5 | 28826.4 | 26477.2 | 59441.7 | 68940.6 | 54981.4 |
|  1000000 | 14461.7 | 24779.6 | 35328.5 | 51555.6 | 25664.3 |  8431.6 |
| 10000000 |  9623.6 | 15694.3 | 25935.8 | 36907.2 | 44151.5 | 37388.0 |

## Median Latency

|          |      1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------ | ------- | ------- | ------- | ------- | ------- |
|   100000 | 56.5μs |  54.8μs |  69.4μs |  84.6μs | 148.0μs | 468.9μs |
|  1000000 | 62.4μs |  70.4μs |  90.9μs | 115.6μs | 134.7μs | 235.2μs |
| 10000000 | 98.2μs | 119.3μs | 143.4μs | 192.2μs | 311.3μs | 494.8μs |

## 99th Percentile Latency

|          |       1 |       2 |       4 |       8 |      16 |      32 |
| -------- | ------- | ------- | ------- | ------- | ------- | ------- |
|   100000 |  94.2μs | 108.0μs | 123.9μs | 178.2μs | 235.5μs |   1.1ms |
|  1000000 |  97.3μs | 118.8μs | 178.7μs | 237.6μs | 301.1μs | 892.9μs |
| 10000000 | 136.4μs | 180.2μs | 221.2μs | 375.8μs | 589.8μs | 993.3μs |

# RandomLookup performance

Perform lots of lookups using a uniform distribution over the keys.

## Throughput

|          |       1 |        2 |        4 |        8 |       16 |        32 |
| -------- | ------- | -------- | -------- | -------- | -------- | --------- |
|   100000 | 77422.7 | 215909.8 | 419294.5 | 752425.6 | 833915.6 | 1216290.1 |
|  1000000 | 30774.3 |  56720.6 | 110143.4 | 196622.8 | 207290.8 |  161732.6 |
| 10000000 | 29287.6 |  52490.0 | 102010.6 | 156826.7 | 201241.6 |  277812.7 |

## Median Latency

|          |      1 |      2 |      4 |      8 |     16 |      32 |
| -------- | ------ | ------ | ------ | ------ | ------ | ------- |
|   100000 | 12.1μs |  8.5μs |  8.8μs |  9.5μs | 16.8μs |  18.3μs |
|  1000000 | 31.7μs | 33.8μs | 34.9μs | 38.8μs | 75.1μs |  99.9μs |
| 10000000 | 32.8μs | 36.5μs | 37.3μs | 48.3μs | 79.5μs | 108.4μs |

## 99th Percentile Latency

|          |      1 |      2 |      4 |      8 |      16 |      32 |
| -------- | ------ | ------ | ------ | ------ | ------- | ------- |
|   100000 | 16.0μs | 12.4μs | 16.6μs | 13.9μs |  24.2μs |  26.1μs |
|  1000000 | 41.0μs | 45.0μs | 49.2μs | 56.6μs | 110.1μs | 157.7μs |
| 10000000 | 39.4μs | 44.5μs | 51.2μs | 72.2μs | 106.0μs | 146.4μs |

# HotsetLookup performance

Same as RandomLookup, but restricting the keyspace to the hot set.

## Throughput

|          |       1 |        2 |        4 |        8 |        16 |        32 |
| -------- | ------- | -------- | -------- | -------- | --------- | --------- |
|   100000 | 80132.1 | 226993.6 | 421468.3 | 776940.8 | 1416978.5 | 1214016.4 |
|  1000000 | 33343.7 |  63449.9 | 121030.2 | 222480.3 |  254656.3 |  125083.9 |
| 10000000 | 29461.5 |  53260.9 | 103361.4 | 160111.6 |  210307.5 |  288378.8 |

## Median Latency

|          |      1 |      2 |      4 |      8 |     16 |      32 |
| -------- | ------ | ------ | ------ | ------ | ------ | ------- |
|   100000 | 11.7μs |  8.0μs |  8.7μs |  9.2μs | 11.5μs |  18.7μs |
|  1000000 | 28.4μs | 30.1μs | 31.1μs | 33.8μs | 57.1μs |  88.0μs |
| 10000000 | 32.6μs | 36.1μs | 36.9μs | 47.1μs | 75.7μs | 104.3μs |

## 99th Percentile Latency

|          |      1 |      2 |      4 |      8 |      16 |      32 |
| -------- | ------ | ------ | ------ | ------ | ------- | ------- |
|   100000 | 15.2μs | 11.4μs | 12.5μs | 13.8μs |  22.9μs |  26.2μs |
|  1000000 | 40.1μs | 43.0μs | 46.3μs | 50.7μs |  98.6μs | 148.2μs |
| 10000000 | 39.6μs | 44.1μs | 50.2μs | 73.2μs | 101.6μs | 143.1μs |
