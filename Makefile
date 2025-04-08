CXX = g++
TARGET = build/Gomoku.exe
SRC = src/main.cpp
INCLUDE = -Iinclude
LIB = -Llib -leasyx -lgdi32 -lole32 -luuid -lmsimg32 -lcomctl32 -luxtheme -static-libgcc -static-libstdc++


all:
	$(CXX) $(SRC) -o $(TARGET) $(INCLUDE) $(LIB) -mwindows

