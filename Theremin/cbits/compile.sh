
gcc -c -Wall -Werror -fPIC utils.c -o utils.o -I../../aubio-0.4.0/src
gcc -shared -fPIC -o libCaubio.so *.o -I../../aubio-0.4.0/src -L../../aubio-0.4.0/build/src/libaubio.a
