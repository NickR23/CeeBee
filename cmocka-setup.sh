#!/usr/bin/env bash
curl https://cmocka.org/files/1.1/cmocka-1.1.2.tar.xz -o cmocka-1.1.2.tar.xz
tar xf cmocka-1.1.2.tar.xz
ls
mkdir -p cmocka-build
cd cmocka-build
cmake ../cmocka-1.1.2
make
# Tell the os about the cmocka's location
export LD_LIBRARY_PATH=./cmocka-build/src:$LD_LIBRARY_PATH

# Run tests
cd ..
make testing
./jumptable_test
./cpu_test
./mmu_test

# Generate coverage html
#lcov --capture --directory . --output-file coverage.info
#genhtml coverage.info --output-directory out
