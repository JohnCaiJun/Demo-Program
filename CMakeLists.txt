#/**********************************************************************   
# * * Copyright (c)2018, caijun                                     
# * * Filename: CMakeList
# * * Description：一键编译                                     
# * * Author：Cai Jun  <johncai.caijun.cn@gmail.com>                
# * * Time: 22:18  2018/9/13 
# * ************************************************************************/

# cmake最低版本号
cmake_minimum_required(VERSION 3.0.0)

# 工程名
project(cjlib)

# 编译标准及参数 （或者 set(CMAKE_CXX_STANDARD 11)）
add_definitions(-std=c++11)

# 头文件
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件
set(SRC 
		${PROJECT_SOURCE_DIR}/src/main.cpp		
)

# 生成可执行文件
link_directories(${PROJECT_SOURCE_DIR})
add_executable(output ${SRC})
