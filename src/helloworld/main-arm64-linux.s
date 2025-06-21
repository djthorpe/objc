// Assembler program to print "Hello World!" to stdout.
///////////////////////////////////////////////////////////////////////////////

.text // Code section
.global main
.align 4

main: 
  // Print hello world
  mov	  X0, #1	            // 1 = stdout
  adr	  X1, helloworld      // string to print
  mov	  X2, #len	          // length of our string
  mov	  X8, #64	            // write system call
  svc   #0x00               // Call kernel to output the string

  // Exit the program
  mov   X0, #0              // Use 0 return code
  mov   X8, #93             // exit system call
  svc   #0x00		            // Call kernel to terminate the program

helloworld:
  .ascii  "Hello World!\n"
len = . - helloworld  // Calculate length of the string
