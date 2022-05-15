OUTPATH = build
PROJECT = signal

LIB_DIR = digital_signal

CFLAGS += -I$(LIB_DIR)
CFLAGS += -I$(LIB_DIR)/protocols

C_SOURCES += $(wildcard $(LIB_DIR)/*.c)
C_SOURCES += $(wildcard $(LIB_DIR)/protocols/*.c)

C_SOURCES += main.c

CC = gcc
LD = gcc

CFLAGS += -Wall -Wextra -g

OBJECTS_DIR = $(OUTPATH)
# OBJECTS = $(addprefix $(OBJECTS_DIR)/, $(C_SOURCES:.c=.o))
OBJECTS = $(C_SOURCES:.c=.o)


all: dirs $(PROJECT)

dirs: $(OUTPATH)

$(OUTPATH):
	mkdir -p $(OUTPATH)

clean:
	rm -rf $(OUTPATH)/*

$(PROJECT): $(OBJECTS)
	$(LD) $(CFLAGS) $(OBJECTS) -o $(OUTPATH)/$(PROJECT)

$(OBJECTS_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
