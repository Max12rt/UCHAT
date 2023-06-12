BINARY_NAME = uchat_server

DIR_SRC = src src/accounts src/database 
SRC_FILES = $(foreach dir, $(DIR_SRC), $(wildcard $(dir)/*.c))

CLANG = clang
FLAGS = -std=c11 -Wall -Wextra -Wpedantic -g 
LIBS = -L ../../resources/libmx \
       `pkg-config --cflags --libs libevent` \
       `pkg-config --cflags --libs jansson` \
        -L `brew --prefix openssl@3`/lib \
  	-I `brew --prefix openssl@3`/include \
       -lsqlite3 -lcrypto -lssl

STATUS = Finished with status 
FAIL = "\t-> \033[0;34m$(STATUS)\033[1;3;31m[FAIL]\033[0m"
SUCCESS = "\t-> \033[0;34m$(STATUS)\033[1;3;32m[SUCCESS]\033[0m\n"

all: install clean

install:
	@echo "\t-> \033[0;34mStart compiling the \033[1;35mSERVER\033[0;34m part...\033[0m"
	@$(CLANG) $(FLAGS) $(LIBS) -c $(SRC_FILES) && exit 0 || (echo $(FAIL); exit 1)
	@clang $(SRC_FILES) $(LIBS) -lmx -o $(BINARY_NAME) && exit 0 || (echo $(FAIL); exit 1)
	@echo $(SUCCESS)

clean:
	@rm -rf *.o

uninstall:
	@rm -rf $(BINARY_NAME)

reinstall: uninstall all