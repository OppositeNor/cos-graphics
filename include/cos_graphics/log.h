#ifndef _CG_LOG_H_
#define _CG_LOG_H_

#define CG_PRINT(...) CGPrint("CosGraphics", "Output", __VA_ARGS__)
#define CG_ERROR(...) CGPrint("CosGraphics", "Error", __VA_ARGS__)

void CGPrint(const char* sender, const char* type, const char* fmt, ...);

#ifdef CG_DEBUG
/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_CONDITION instead.
 */
#define CG_DB_ERROR_CONDITION(cond, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            return;                     \
        }                               \
        ((void)0)
/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return the return_value. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_DB_ERROR_COND_RETURN instead.
 */
#define CG_DB_ERROR_COND_RETURN(cond, return_value, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            return return_value;        \
        }                               \
        ((void)0)
/**
 * @brief Should print out error in Debug. If condition is true, then print out error and crash the program. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_DB_ERROR_COND_EXIT instead.
 */
#define CG_DB_ERROR_COND_EXIT(cond, exit_value, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            exit(exit_value);           \
        }                               \
        ((void)0)
#else

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_CONDITION instead.
 */
#define CG_DB_ERROR_CONDITION(cond, ...)

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return the return_value. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_DB_ERROR_COND_RETURN instead.
 */
#define CG_DB_ERROR_COND_RETURN(cond, return_value, ...)

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and crash the program. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_DB_ERROR_COND_EXIT instead.
 */
#define CG_DB_ERROR_COND_EXIT(cond, exit_value, ...)
#endif

#endif  // _CG_LOG_H_