# BitBarrel

A log-structured K-V store based on the [Bitcask](https://riak.com/assets/bitcask-intro.pdf) paper. 

Unlike a traditional K-V storage engine that uses a hashmap, BitBarrel writes to append-only segment files,
and continously compacts and merges these segments to clean up space. 

### Features:
- Get ✅
- Set ✅
- Segments ✅
- Compacting and merging ✅
- Deleting + tombstones
- CRC checksums
- Hint files