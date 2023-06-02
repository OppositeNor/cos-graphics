#ifndef _CG_LOG_H_
#define _CG_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CG_MODIFY_OUTPUT_MESSAGE
    #define CG_PRINT(...) CGPrint("CosGraphics", "Output", __VA_ARGS__)
    #define CG_ERROR(...) CGPrint("CosGraphics", "Error", __VA_ARGS__)
    #define CG_WARNING(...) CGPrint("CosGraphics", "Warning", __VA_ARGS__)
#endif

void CGPrint(const char* sender, const char* type, const char* fmt, ...);

#if CG_DEBUG
#include <stdlib.h>

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and exit the function. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_EXPRESSION instead.
 */
#define CG_ERROR_CONDITION(cond, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            return;                     \
        }                               \
        ((void)0)
/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return the return_value. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_EXP_RETURN instead.
 */
#define CG_ERROR_COND_RETURN(cond, return_value, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            return return_value;        \
        }                               \
        ((void)0)
/**
 * @brief Should print out error in Debug. If condition is true, then print out error and crash the program. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_EXP_EXIT instead.
 */
#define CG_ERROR_COND_EXIT(cond, exit_value, ...) \
        if (cond){                      \
            CG_ERROR(__VA_ARGS__);      \
            exit(exit_value);           \
        }                               \
        ((void)0)
/**
 * @brief Run the expression, and check the value of expression. If it is a non-true value, then print out error and exit the function. 
 * NOTE: The expression will run in other building type; however, in release mode it will not check error for the condition.
 */
#define CG_ERROR_EXPRESSION(expression, ...) CG_ERROR_CONDITION(expression, __VA_ARGS__)
/**
 * @brief Run the expression, and check the value of expression. If it is  a non-true value, then print out error and return return_value. 
 * NOTE: The expression will run in other building type; however, in release mode it will not check error for the condition.
 */
#define CG_ERROR_EXP_RETURN(expression, return_value, ...) CG_ERROR_COND_RETURN(expression, return_value, __VA_ARGS__)
/**
 * @brief Run the expression, and check the value of expression. If return a non-true value, then print out error and return return_value. 
 * NOTE: The expression will run in  other building type; however, in release mode it will not check error for the condition.
 */
#define CG_ERROR_EXP_EXIT(expression, exit_value, ...) CG_ERROR_COND_EXIT(expression, exit_value, __VA_ARGS__)
#else

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_CONDITION instead.
 */
#define CG_ERROR_CONDITION(cond, ...)

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and return the return_value. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_COND_RETURN instead.
 */
#define CG_ERROR_COND_RETURN(cond, return_value, ...)

/**
 * @brief Should print out error in Debug. If condition is true, then print out error and crash the program. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CG_ERROR_COND_EXIT instead.
 */
#define CG_ERROR_COND_EXIT(cond, exit_value, ...)

/**
 * @brief Run the expression, and check the value of expression. If it is a non-true value, then print out error and exit the function. 
 * NOTE: The expression will run in other building type; however, in release mode it will not check error for the condition.
 */
#define CG_ERROR_EXPRESSION(expression, ...) expression
/**
 * @brief Run the expression, and check the value of expression. If it is  a non-true value, then print out error and return return_value. 
 * NOTE: The expression will run in other building type; however, in release mode it will not check error for the condition.
 */
#define CG_ERROR_EXP_RETURN(expression, return_value, ...) expression
/**
 * @brief Run the expression, and check the value of expression. If return a non-true value, then print out error and return return_value. 
 * NOTE: The expression will run in  other building type; however, in release mode it will not check error for the condition.
 */
#define CG_ERROR_EXP_EXIT(expression, exit_value, ...) expression
#endif

#ifdef __cplusplus
}
#endif

#endif  // _CG_LOG_H_