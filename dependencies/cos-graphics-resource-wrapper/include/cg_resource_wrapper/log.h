#ifndef _CGRWRW_LOG_H_
#define _CGRWRW_LOG_H_

#ifndef _CGRW_LOG_H_
#define _CGRW_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CGRW_USE_UTF16LE
    #include <wchar.h>
    typedef wchar_t CGRWChar;

    #define CGRW_STRLEN wcslen
    #define CGRW_PRINTF wprintf
    #define CGRW_VPRINTF vwprintf

    #define CGSTR(str) L##str

    #define CGRW_USE_WCHAR
#else
    typedef char CGRWChar;

    #define CGRW_STRLEN strlen
    #define CGRW_PRINTF printf
    #define CGRW_VPRINTF vprintf

    #define CGSTR(str) str
#endif

#define CGRW_PRINT(...) CGRWPrint(CGSTR("CGResourceWrapper"), CGSTR("Output"), __VA_ARGS__)
#define CGRW_ERROR(...) CGRWPrint(CGSTR("CGResourceWrapper"), CGSTR("Error"), __VA_ARGS__)
#define CGRW_WARNING(...) CGRWPrint(CGSTR("CGResourceWrapper"), CGSTR("Warning"), __VA_ARGS__)

#ifdef CGRW_NO_VERBOSE
#define CGRW_PRINT_VERBOSE(...) ((void)0)
#else
#define CGRW_PRINT_VERBOSE(...) CGRWPrint(CGSTR("CGResourceWrapper"), CGSTR("OutputVerbose"), __VA_ARGS__)
#endif

void CGRWPrint(const CGRWChar* sender, const CGRWChar* type, const CGRWChar* fmt, ...);

/**
 * @brief Should print out error. If condition is true, then print out error and exit the function. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CGRW_ERROR_EXPRESSION instead.
 */
#define CGRW_ERROR_CONDITION(cond, ...) \
        if (cond){                      \
            CGRW_ERROR(__VA_ARGS__);      \
            return;                     \
        }                               \
        ((void)0)
/**
 * @brief Should print out error. If condition is true, then print out error and return the return_value. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CGRW_ERROR_EXP_RETURN instead.
 */
#define CGRW_ERROR_COND_RETURN(cond, return_value, ...) \
        if (cond){                      \
            CGRW_ERROR(__VA_ARGS__);      \
            return return_value;        \
        }                               \
        ((void)0)
/**
 * @brief Should print out error. If condition is true, then print out error and crash the program. 
 * NOTE: The condition will be ignored in release mode, so please DONOT put functions that you wish
 * to run in all build modes. If you wish to also run the expression, use CGRW_ERROR_EXP_EXIT instead.
 */
#define CGRW_ERROR_COND_EXIT(cond, exit_value, ...) \
        if (cond){                      \
            CGRW_ERROR(__VA_ARGS__);      \
            exit(exit_value);           \
        }                               \
        ((void)0)
/**
 * @brief Run the expression, and check the value of expression. If it is a non-true value, then print out error and exit the function. 
 * NOTE: The expression will run in other building type; however, in release mode it will not check error for the condition.
 */
#define CGRW_ERROR_EXPRESSION(expression, ...) CGRW_ERROR_CONDITION(expression, __VA_ARGS__)
/**
 * @brief Run the expression, and check the value of expression. If it is  a non-true value, then print out error and return return_value. 
 * NOTE: The expression will run in other building type; however, in release mode it will not check error for the condition.
 */
#define CGRW_ERROR_EXP_RETURN(expression, return_value, ...) CGRW_ERROR_COND_RETURN(expression, return_value, __VA_ARGS__)
/**
 * @brief Run the expression, and check the value of expression. If return a non-true value, then print out error and return return_value. 
 * NOTE: The expression will run in  other building type; however, in release mode it will not check error for the condition.
 */
#define CGRW_ERROR_EXP_EXIT(expression, exit_value, ...) CGRW_ERROR_COND_EXIT(expression, exit_value, __VA_ARGS__)


#ifdef __cplusplus
}
#endif

#endif  // _CGRW_LOG_H_

#endif  // _CGRWRW_LOG_H_