// Assembler program to print "Hello World!"
// to stdout.

.global main
.align 4

// Print hello world
main: 
  mov	  X0, #1	            // 1 = stdout
  adr	  X1, helloworld      // string to print
  mov	  X2, #13	            // length of our string
  mov	  X8, #64	            // write system call
  svc   #0x00               // Call kernel to output the string

// Exit the program
  mov   X0, #0              // Use 0 return code
  mov   X9, #93             // call number 93 terminates this program
  svc   #0x00		            // Call kernel to terminate the program

helloworld:      .ascii  "Hello World!\n"

