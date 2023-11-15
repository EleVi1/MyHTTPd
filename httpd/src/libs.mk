SRC_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

config/libconfig.a:
	$(MAKE) -C $(SRC_DIR)config

server/libserver.a:
	$(MAKE) -C $(SRC_DIR)server

logger/liblogger.a:
	$(MAKE) -C $(SRC_DIR)logger

utils/libutils.a:
	$(MAKE) -C $(SRC_DIR)utils

http/libhttp.a:
	$(MAKE) -C $(SRC_DIR)http

daemon/libdaemon.a:
	$(MAKE) -C $(SRC_DIR)daemon
