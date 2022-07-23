cmake_minimum_required(VERSION 3.10)
project(custom-passes)

set(LLVM_INSTALL_DIR "" CACHE PATH "LLVM installation directory")

message(FATAL_ERROR ${LLVM_INSTALL_DIR})
