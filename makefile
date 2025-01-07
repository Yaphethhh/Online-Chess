# Compiler and flags
CC = clang
CFLAGS = -Wall -pedantic -std=c99

# Python include path (adjust this based on your system's Python version)
PYTHON_INCLUDE = /usr/include/python3.11
PYTHON_LIB = -L/usr/lib/python3.11/config-3.11-x86_64-linux-gnu

# Rule to link everything into the Python module _hclib.so
_hclib.so: hclib_wrap.o libhclib.so
	$(CC) $(CFLAGS) -shared hclib_wrap.o -L. $(PYTHON_LIB) -lpython3.11 -lhclib -o _hclib.so

# Rule to compile the SWIG wrapper
hclib_wrap.o: hclib_wrap.c
	$(CC) $(CFLAGS) -c hclib_wrap.c -I$(PYTHON_INCLUDE) -fPIC -o hclib_wrap.o

# Rule to run SWIG and generate Python wrapper
hclib_wrap.c hclib.py: hclib.i libhclib.so
	swig -python hclib.i
	
# Rule to build the shared library for C (libhclib.so)
libhclib.so: hclib.o
	$(CC) -shared -o libhclib.so hclib.o -lm

# Rule to compile hclib.o from hclib.c
hclib.o: hclib.c hclib.h
	$(CC) $(CFLAGS) -fPIC -c hclib.c -o hclib.o

# Clean up the build files
clean:
	rm -f *.o *.so main hclib_wrap.c hclib.py
