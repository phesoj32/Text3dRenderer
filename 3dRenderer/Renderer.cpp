#include <iostream>
#include <vector>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#endif

/**
 * @brief Blazing-Fast Raw Terminal Print
 * @param data Pointer to the first character in memory (No copy overhead)
 * @param size Total number of bytes/characters to write
 */
void bPrint(const char* data, size_t size) {
    if (!data || size == 0) return;

    #if defined(_WIN32)
        // Windows Kernel Write
        static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD written;
        WriteFile(hConsole, data, static_cast<DWORD>(size), &written, nullptr);
    #else
        // POSIX (Linux / macOS) Kernel Write
        ::write(1, data, size);
    #endif
}

int main() {
    // 1. Optimize traditional streams just in case they are used elsewhere
    std::ios_base::sync_with_stdio(false);

    // 2. Allocate your dynamic screen buffer once
    std::vector<char> screen_buffer(5000, 'A');

    // Game/Render Loop
    while (true) {
        // ... Update your screen_buffer array elements here ...
        screen_buffer[4999] = '\n'; // Inject trailing newline

        // 3. Pass via pointer to the function (Instantly fast)
        bPrint(screen_buffer.data(), screen_buffer.size());

        break; // Remove for infinite loop
    }

    return 0;
}
