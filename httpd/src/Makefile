include flags.mk

OBJ = main.o

LIBS = config/libconfig.a server/libserver.a utils/libutils.a daemon/libdaemon.a http/libhttp.a logger/liblogger.a

all: httpd

httpd: $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group

include libs.mk

clean:
	$(MAKE) clean -C config
	$(MAKE) clean -C http
	$(MAKE) clean -C daemon
	$(MAKE) clean -C utils
	$(MAKE) clean -C server
	$(MAKE) clean -C logger
	$(RM) $(OBJ)

.PHONY: all clean
