BUILDTARGET = lcdTest
BUILDOBJECTS = lcdTest.o i2c.o lcdlib.o

CFLAGS += -Wall -Werror

all: $(BUILDTARGET)


$(BUILDTARGET) : $(BUILDOBJECTS)
	gcc -o $@ $^ $(LFLAGS)

