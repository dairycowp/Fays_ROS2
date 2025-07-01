LIBS = -L . -lft602
TARGET = i2c

# === Linux & macOS ===
BIN=$(TARGET)
ifneq ("$(wildcard libft602.dylib)","")
# == macOS ==
CXXLIBS = -lc++
# == End of macOS ==
else
# == Linux ==
LIBS += -pthread -lrt
CXXLIBS = -lstdc++
# == End of Linux ==
endif
# === End of Linux & macOS ===


COMMON_FLAGS = -ffunction-sections -fmerge-all-constants
COMMON_CFLAGS = -g -O3 -Wall -Wextra $(COMMON_FLAGS)
CFLAGS = -std=c99  $(COMMON_CFLAGS) -D_POSIX_C_SOURCE
CXXFLAGS = -std=c++14 $(COMMON_CFLAGS)

all: $(BIN)
	
$(BIN): $(TARGET).o
	$(CC) $(COMMON_FLAGS) -o $@ $^ $(CXXLIBS) $(LIBS)
ifneq ($(RELEASE_BUILD),)
	strip $@
endif

clean:
	-rm -f $(TARGET).o
