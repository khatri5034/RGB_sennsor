TARGET = assignment5

CC = gcc
CFLAGS = -Wall -g -O2 -Ilib
LIBS = -lm -lbcm2835

LIB_DIR = lib
DIST_DIR = dist

SRCS = main.c \
	$(LIB_DIR)/ColorLib.c \
	$(LIB_DIR)/DEV_Config.c

OBJS = $(patsubst %.c, $(DIST_DIR)/%.o, $(notdir $(SRCS)))

all: $(DIST_DIR) $(TARGET)

run: assignment5
	sudo ./assignment5

$(DIST_DIR):
	mkdir -p $(DIST_DIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	@echo "Build Successful: Executable '$(TARGET)' created."

$(DIST_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(DIST_DIR)/%.o: $(LIB_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(DIST_DIR)
	rm -f $(TARGET)
	@echo "Cleaned: dist/ and $(TARGET) removed."
