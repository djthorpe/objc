.text
.globl objc_msgSend
.globl objc_msgSend_impl

objc_msgSend:
    // Prologue
    stp     x29, x30, [sp, #-16]!    // Save frame pointer and link register
    mov     x29, sp                  // Set up frame pointer

    // Save callee-saved registers
    stp     x19, x20, [sp, #-16]!    // Save x19 and x20
    stp     x21, x22, [sp, #-16]!    // Save x21 and x22
    stp     x23, x24, [sp, #-16]!    // Save x23 and x24
    stp     x25, x26, [sp, #-16]!    // Save x25 and x26
    stp     x27, x28, [sp, #-16]!    // Save x27 and x28

    // Prepare arguments for the C function
    mov     x19, x0                 // Receiver is in x0
    mov     x20, x1                 // Selector is in x1
    // TODO: Handle other arguments

    // Call objc_msgSend_impl
    bl      objc_msgSend_impl

    // Restore callee-saved registers
    ldp     x27, x28, [sp], #16     // Restore x27 and x28
    ldp     x25, x26, [sp], #16     // Restore x25 and x26
    ldp     x23, x24, [sp], #16     // Restore x23 and x24
    ldp     x21, x22, [sp], #16     // Restore x21 and x22
    ldp     x19, x20, [sp], #16     // Restore x19 and x20

    // Epilogue
    ldp     x29, x30, [sp], #16     // Restore frame pointer and link register
    ret                             // Return from function
