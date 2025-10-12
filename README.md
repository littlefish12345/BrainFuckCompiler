# BrainFuckCompiler
A compiler for extended BrainFuck written in cpp.

# How it works?
Just simply translate each symbol into assembly. I treat ebf like some kind of IR.

# Useage
Checkout bfc -h.

# Requirement
nasm and ld (I only tried gcc's ld but lld should work too).

# Platform support
Currently I only implement linux amd64 assembly and it is hard coded into the compiler. Migrate it into windows amd64 is relative simple but I don't want to mess up with windows stdio api for now.

# Extended BrainFuck?
It's just normal BrainFuck with some useful symbols. Specific symbol definition can be found [here](https://github.com/MegumiKasuga/BrainFuckJ).
