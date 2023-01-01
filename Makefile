OPT?=-O3
CFLAGS?=
override CFLAGS+=-MMD -MP $(OPT)
LDFLAGS?=

SRC=main.c
STATIC_LIBS=cJSON/libcjson.a
APP=JSONParse

.PHONY:all
all:$(APP)

$(APP):$(patsubst %.c,%.o,$(SRC)) $(STATIC_LIBS)
	$(CC) $(LDFLAGS) -o $@ $^

cJSON/libcjson.a:
	$(MAKE) -C cJSON libcjson.a CFLAGS=$(OPT)

%.o:%.c
	$(CC) $(CFLAGS) -c $<

-include $(SRC:.c=.d)

.PHONY:clean
clean:
	$(MAKE) -C cJSON clean
	rm -f *.o *.d $(APP) 