BINARY_NAME = libmx.a

SRC_FILES = src/*.c
OBJ_FILES = $(notdir $(SRC_FILES:%.c=%.o))

CLANG = clang
FLAGS = -std=c11 -Wall -Wextra -Werror -Wpedantic -g

LIB_ARCHIVE = ar
LIB_FLAGS = rsc

STATUS = Finished with status 
FAIL = "\t-> \033[0;34m$(STATUS)\033[1;3;31m[FAIL]\033[0m"
SUCCESS = "\t-> \033[0;34m$(STATUS)\033[1;3;32m[SUCCESS]\033[0m\n"

all: install clean

install:
	@echo "\t-> \033[0;34mStart compiling the \033[1;35mLIBMX\033[0;34m part...\033[0m"
	@$(CLANG) $(FLAGS) -c $(SRC_FILES) && exit 0 || (echo $(FAIL); exit 1)
	@$(LIB_ARCHIVE) $(LIB_FLAGS) $(BINARY_NAME) *.o && exit 0 || (echo $(FAIL); exit 1)
	@echo $(SUCCESS)
	
clean:
	@rm -rf *.o

uninstall:
	@rm -rf $(BINARY_NAME)

reinstall: uninstall all
