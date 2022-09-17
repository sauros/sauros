wget https://github.com/cpputest/cpputest/releases/download/v3.8/cpputest-3.8.tar.gz -O /tmp/cpputest.tar.gz

tar -xzvf /tmp/cpputest.tar.gz -C /tmp

if [ "$CC" = "clang" ]; then export CXX="clang++"; fi && cd /tmp/cpputest-* && ./configure --prefix=/usr/local && make && sudo make install

cd /tmp/cpputest-* && ./configure --prefix=/usr/local && make && sudo make install