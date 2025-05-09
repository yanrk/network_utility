tar -xzf openssl-1.1.1c.tar.gz
cd openssl-1.1.1c
./config -fPIC --prefix=/home/yanrk/codes/openssl no-shared
make
make install
