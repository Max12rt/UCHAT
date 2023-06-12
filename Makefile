MAKE = @make -C

LIBMX = resources/libmx
CLIENT = src/client
SERVER = src/server

all: install clean

install:
	@brew install gtk4
	@brew install libevent
	@brew install openssl@3
	@brew install jansson
	@brew install sqlite3
	@echo "\033[1;33mINSTALL:\n\033[0m"
	$(MAKE) $(LIBMX)
	$(MAKE) $(SERVER)
	@mv $(SERVER)/uchat_server .
	$(MAKE) $(CLIENT)
	@mv $(CLIENT)/uchat .
	@echo "\033[1;33mUSAGE:\033[0m"
	@echo "\t-> \033[0;32m./uchat_server\033[0m - (running server)"
	@echo "\t-> \033[0;32m./uchat\033[0m - (client part, it will run chat app)\n"
	

clean:
	$(MAKE) $(LIBMX) uninstall clean
	$(MAKE) $(CLIENT) clean
	$(MAKE) $(SERVER) clean

uninstall:
	@echo "\033[1;33mUNINSTALL:\n\033[0m"
	@rm -rf uchat
	@echo "\t-> \033[1;91muchat \033[0;91mremoved\033[0m"
	@rm -rf uchat_server
	@echo "\t-> \033[1;91muchat_server \033[0;91mremoved\033[0m"
	@echo

reinstall: uninstall install clean
