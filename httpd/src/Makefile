include flags.mk

OBJ = main.o

LIBS = config/libconfig.a server/libserver.a utils/libutils.a daemon/libdaemon.a

all: httpd

$(LIBS):
	$(MAKE) -C config
	$(MAKE) -C server
	$(MAKE) -C utils

httpd: $(OBJ) $(LIBS)
	$(CC) $(LDFLAGS) -o $@ $(OBJ) -Wl,--start-group $(LIBS) -Wl,--end-group

include libs.mk

clean:
	$(RM) $(OBJS) $(LIBS) main.o ../httpd
.PHONY: all
