// Assembler program to print "Hello World!" to stdout.
///////////////////////////////////////////////////////////////////////////////

.global main

main: 
  b main  // Infinite loop to prevent the program from exiting
