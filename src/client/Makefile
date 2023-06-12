BINARY_NAME = uchat

SRC_FILES = src/*.c

INCLUDES = -I./

STATUS = Finished with status 
FAIL = "\t-> \033[0;34m$(STATUS)\033[1;3;31m[FAIL]\033[0m"
SUCCESS = "\t-> \033[0;34m$(STATUS)\033[1;3;32m[SUCCESS]\033[0m\n"

INCS =  $(INCLUDES) `pkg-config --cflags gtk4`  `pkg-config --cflags libevent` `pkg-config --cflags jansson` -I `brew --prefix openssl@3`/include
LIBS =  `pkg-config --libs gtk4` `pkg-config --libs libevent` `pkg-config --libs jansson`  -L `brew --prefix openssl@3`/lib
CFLAGS = -Wall $(INCS)
LDFLAGS = $(LIBS) -lssl -lcrypto

all: install clean

install:
	@echo "\t-> \033[0;34mStart compiling the \033[1;35mCLIENT\033[0;34m part...\033[0m"
	@clang $(CFLAGS) -c $(SRC_FILES) && \
	clang *.o $(LDFLAGS) -L ../../resources/libmx -lmx -o $(BINARY_NAME) && \
	echo $(SUCCESS) || (echo $(FAIL); exit 1)

clean:
	@rm -rf *.o

uninstall:
	@rm -rf $(BINARY_NAME)

reinstall: uninstall all
