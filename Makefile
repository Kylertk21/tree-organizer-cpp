SRC := main.cpp tree.cpp
LINK := tree.h
TAR := tree
CC := g++
FLAGS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	$(CC) $(SRC) -o tree $(FLAGS)
