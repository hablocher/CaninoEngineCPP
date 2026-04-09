#include <canino/core/config_parser.h>
#include <stdio.h>
#include <string.h>

namespace canino {

// Cache estatico DOD pra arquivos de ate 1MB sem dar malloc() runtime
static char s_ConfigBuffer[1024 * 1024]; 
static bool s_IsLoaded = false;

void ConfigParser::LoadFromFile(const char* filepath) {
    s_IsLoaded = false;
    FILE* file = nullptr;
    if (fopen_s(&file, filepath, "rb") == 0 && file) {
        size_t readSize = fread(s_ConfigBuffer, 1, sizeof(s_ConfigBuffer) - 1, file);
        s_ConfigBuffer[readSize] = '\0';
        fclose(file);
        s_IsLoaded = true;
    }
}

bool ConfigParser::GetString(const char* key, char* outBuffer, unsigned int bufferSize) {
    if (!s_IsLoaded || !key || !outBuffer || bufferSize == 0) return false;

    const char* pLine = s_ConfigBuffer;
    size_t keyLen = strlen(key);

    while (pLine && *pLine != '\0') {
        if (strncmp(pLine, key, keyLen) == 0 && pLine[keyLen] == '=') {
            const char* valueStart = pLine + keyLen + 1;
            const char* lineEnd = strchr(valueStart, '\r');
            if (!lineEnd) lineEnd = strchr(valueStart, '\n');
            if (!lineEnd) lineEnd = valueStart + strlen(valueStart);

            size_t valueLen = lineEnd - valueStart;
            if (valueLen >= bufferSize) valueLen = bufferSize - 1;

            strncpy_s(outBuffer, bufferSize, valueStart, valueLen);
            outBuffer[valueLen] = '\0';
            return true;
        }

        // Jump to next line
        pLine = strchr(pLine, '\n');
        if (pLine) pLine++;
    }
    return false;
}

}
