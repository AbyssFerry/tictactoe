# 编译器设置
CXX := g++
TARGET := build/tictactoe.exe
SRC_DIR := src
BUILD_DIR := build
INC_DIR := include
LIB_DIR := lib

# 自动获取源文件列表
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# 编译选项
CXXFLAGS := -std=c++11 -I$(INC_DIR) -Wall -Wextra
LDFLAGS := -L$(LIB_DIR)
LDLIBS := -leasyx -lgdi32 -luser32 -limm32 -lmsimg32 -lole32 -loleaut32 -lwinmm -luuid

# 确保build目录存在
$(shell if not exist $(BUILD_DIR) mkdir $(BUILD_DIR))

# 默认目标
all: $(TARGET)

# 主构建规则
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# 编译源文件到对象文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理构建文件
clean:
	del /q $(BUILD_DIR)\* 2>nul || exit 0

.PHONY: all clean