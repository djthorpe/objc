.text
.globl _objc_msgSend
.globl _objc_msgSend_impl

_objc_msgSend:
    # Prologue
    push   %rbp              # Save base pointer
    mov    %rsp, %rbp        # Set stack frame

    # Save necessary registers (callee-saved, if they are used)
    push   %rbx
    push   %r12
    push   %r13
    push   %r14
    push   %r15

	# Prepare arguments for the C function
    mov %rdi, %rbx           # Receiver is in %rdi
    mov %rsi, %r12           # Selector is in %rsi	
	# TODO: other arguments
	call  _objc_msgSend_impl

    # Restore registers
    pop    %r15
    pop    %r14
    pop    %r13
    pop    %r12
    pop    %rbx

    # Epilogue
    mov    %rbp, %rsp        # Restore stack pointer
    pop    %rbp              # Restore base pointer
    ret                      # Return from function
