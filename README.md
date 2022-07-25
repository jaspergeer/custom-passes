# custom-passes
Custom LLVM passes written as a learning excercise.

## Passes
### [mul-to-bitshift](lib/MulToBitshift.cpp)
Converts `mul` instructions with a power-of-2 operand to equivalent `shl` bitshift instructions.
