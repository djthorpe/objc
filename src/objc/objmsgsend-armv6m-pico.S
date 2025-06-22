.syntax unified
.thumb_func 
.global objc_msgSend
.global objc_msgSend_impl
.type   objc_msgSend, %function
.type   objc_msgSend_impl, %function

objc_msgSend:
    push {r4, r5, lr}
    mov r4, r0                          // Receiver is in r0
    mov r5, r1                          // Selector is in r1

    // Call objc_msgSend_impl
    bl objc_msgSend_impl
    // Return value is now in r0, we need to preserve it

    // Restore registers (without modifying r0 which has the return value)
    pop {r4, r5, pc}
