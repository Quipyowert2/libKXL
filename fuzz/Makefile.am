bin_PROGRAMS = sample
sample_SOURCES = sample.c
sample_CFLAGS = -g -O3 -I../src -L/usr/local/lib64
sample_LDFLAGS = -lKXL -lX11 -lpulse-simple -lpulse

#sample.bc is LLVM IR bitcode
if ENABLE_FUZZ
FUZZ_TARGETS = sample sample.bc sample.o
endif

clean-local:
	rm -f $(FUZZ_TARGETS)