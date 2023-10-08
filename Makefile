
TARGET = ps1_sokoban
TYPE = ps-exe

SRCS = third_party/nugget/common/crt0/crt0.s \
main.c \
mem.c \
res.c \
gfx.c \
mem_card.c \
input.c \
audio.c \
third_party/nugget/modplayer/modplayer.c \
sokoban.c \
scene.c \
scene_init.c \
scene_title.c \
scene_levels.c \
scene_all_cleared.c \
scene_credits.c \
scene_level.c \
menu.c \
game.c \

CPPFLAGS += -Ithird_party/psyq-iwyu/include
LDFLAGS += -Lthird_party/psyq/lib
LDFLAGS += -Wl,--start-group
LDFLAGS += -lapi
LDFLAGS += -lc
LDFLAGS += -lc2
LDFLAGS += -lcard
LDFLAGS += -lcd
LDFLAGS += -lcomb
LDFLAGS += -lds
LDFLAGS += -letc
LDFLAGS += -lgpu
LDFLAGS += -lgs
LDFLAGS += -lgte
LDFLAGS += -lgun
LDFLAGS += -lhmd
LDFLAGS += -lmath
LDFLAGS += -lmcrd
LDFLAGS += -lmcx
LDFLAGS += -lpad
LDFLAGS += -lpress
LDFLAGS += -lsio
LDFLAGS += -lsnd
LDFLAGS += -lspu
LDFLAGS += -ltap
LDFLAGS += -Wl,--end-group

include third_party/nugget/common.mk
