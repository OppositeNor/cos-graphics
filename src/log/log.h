#ifndef _LOG_H_
#define _LOG_H_

#define CG_PRINT(...) CGPrint("CosGraphics", "Output", __VA_ARGS__)
#define CG_ERROR(...) CGPrint("CosGraphics", "Error", __VA_ARGS__)

void CGPrint(const char* sender, const char* type, const char* fmt, ...);



#endif  // _ERROR_H_