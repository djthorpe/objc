// Assembler program to print "Hello World!"
// to stdout.

.global _main                 // Provide program starting address to linker
.align 4

// Print hello world
_main: 
  mov	  X0, #1	            // 1 = stdout
  adr	  X1, helloworld      // string to print
  mov	  X2, #13	            // length of our string
  mov	  X16, #4	            // write system call
  svc   #0x80               // Call kernel to output the string

// Exit the program
  mov   X0, #0              // Use 0 return code
  mov   X16, #1             // call number 1 terminates this program
  svc   #0x80		            // Call kernel to terminate the program

helloworld:      .ascii  "Hello World!\n"

