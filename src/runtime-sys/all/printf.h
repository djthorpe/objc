#pragma once

/**
 * @brief Initializes the printf mutex for thread-safe printing.
 * @ingroup System
 */
extern void sys_printf_init(void);

/**
 * @brief Finalizes the printf mutex and cleans up resources.
 * @ingroup System
 */
extern void sys_printf_finalize(void);
