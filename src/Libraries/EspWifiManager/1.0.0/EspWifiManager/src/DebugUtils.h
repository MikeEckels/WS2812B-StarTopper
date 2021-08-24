#ifndef DEBUG_UTILS_H
#define DEBUG_UTILS_H

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINT_ERR(...) DEBUG_PRINT("[!] ");  DEBUG_PRINTLN(__VA_ARGS__)
#define DEBUG_PRINT_INFO(...) DEBUG_PRINT("    ");  DEBUG_PRINTLN(__VA_ARGS__)
#define DEBUG_PRINT_INFO_NO_LN(...) DEBUG_PRINT("    "); DEBUG_PRINT(__VA_ARGS__)
#define DEBUG_PRINT_NOTICE(...) DEBUG_PRINT("[+] ");  DEBUG_PRINTLN(__VA_ARGS__)
#define DEBUG_PRINTF(...)Serial.printf(__VA_ARGS__)
#define DEBUG_PRINT_FULL(...)  \
        Serial.print(millis());     \
        Serial.print(": ");    \
        Serial.print(__PRETTY_FUNCTION__); \
        Serial.print(' ');      \
        Serial.print(__LINE__);     \
        Serial.print(' ');      \
        Serial.print(__VA_ARGS__)
#define DEBUG_PRINT_FULLLN(...)  \
        Serial.print(millis());     \
        Serial.print(": ");    \
        Serial.print(__PRETTY_FUNCTION__); \
        Serial.print(' ');      \
        Serial.print(__LINE__);     \
        Serial.print(' ');      \
        Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINT_ERR(...)
#define DEBUG_PRINT_INFO(...)
#define DEBUG_PRINT_INFO_NO_LN(...)
#define DEBUG_PRINT_NOTICE(...)
#define DEBUG_PRINTF(...)
#define DEBUG_PRINT_FULL(...)
#define DEBUG_PRINT_FULLLN(...)
#endif

#endif //DEBUG_UTILS_H