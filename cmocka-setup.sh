#!/usr/bin/env bash
curl https://cmocka.org/files/1.1/cmocka-1.1.2.tar.xz -o cmocka-1.1.2.tar.xz
tar xf cmocka-1.1.2.tar.xz
ls
mkdir -p cmocka-build
cd cmocka-build
cmake ../cmocka-1.1.2
make
sudo make install

# Run tests
cd ..
make testing
./testing
