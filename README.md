Implemenation of Monkey language interpreter in C++ described in "Writing An Interpreter In Go" with some additional features (while loop, additional intrinsics)

Executable targets:  
monkey_compiler_unit_tests - tests, enabled by default and can be disabled  
repl - Read, Evaluate, Print, and Loop  
exec - execute program from files. Takes files as command line argument. Example code can be found at apps\exec\resources  

Cmake flags:  
monkey_compiler_ENABLE_TESTING (ON by default)- specify if monkey_compiler_unit_tests target should be built  
monkey_compiler_ENABLE_PARSE_TRACING (OFF by default) - specify if parsing call stack should be printed  
