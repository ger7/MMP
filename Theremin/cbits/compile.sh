gcc -c -Wall -Werror -fPIC utils.c -I../../aubio-0.4.0/src
gcc -shared -fPIC -o libCaubio.so utils.c -I../../aubio-0.4.0/src -L../../aubio-0.4.0/build/src/libaubio.a
