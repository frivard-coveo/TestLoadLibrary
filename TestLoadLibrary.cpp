// TestLoadLibrary.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#include <iostream>
#include <vector>

int wmain(int argc, wchar_t* argv[], wchar_t* envp[])
{
    int bufferSize = 4096;
    std::vector<WCHAR> buffer;
    buffer.resize(bufferSize, 0);
    DWORD actual = GetCurrentDirectoryW((DWORD)bufferSize, buffer.data());
    if (actual == 0)
    {
        return GetLastError();
    }

    std::wstring cwd{ buffer.data() };
    std::wcout << L"Current directory is " << cwd << std::endl;

    std::wstring theLib{ argv[1] };
    std::wcout << L"Will try to load " << theLib << std::endl;

    HMODULE theModule = LoadLibraryExW(argv[1], NULL, 0); // normal load
    if (theModule != NULL)
    {
        std::wcout << "LoadLibrary succeeded." << std::endl;

        if (argc > 2)
        {
            std::wstring theProc{ argv[2] };
            std::wcout << L"Will try to load procedure " << theProc << std::endl;
            char theProcArray[256];
            WideCharToMultiByte(CP_UTF8, 0, argv[2], theProc.size(), theProcArray, 256, NULL, NULL);
            FARPROC procAddr = GetProcAddress(theModule, theProcArray);
            if (procAddr != NULL)
            {
                std::wcout << "GetProcAddress succeeded." << std::endl;
            }
            else
            {
                DWORD lastError = GetLastError();
                std::wcout << "GetProcAddress failed. Error code was " << lastError << std::endl;
                std::wcout << "More info about that error code might be available at https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-" << std::endl;
            }
        }

        FreeLibrary(theModule);
    }
    else {
        DWORD lastError = GetLastError();
        std::wcout << "LoadLibrary failed. Error code was " << lastError << std::endl;
        std::wcout << "More info about that error code might be available at https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-" << std::endl;

        int messageBufferSize = 60 * 1024;
        std::vector<WCHAR> msgBuffer;
        msgBuffer.resize(messageBufferSize, 0);
        DWORD formatResult = 0;

        if (lastError == ERROR_BAD_EXE_FORMAT)
        {
            std::wcout << L"ERROR_BAD_EXE_FORMAT" << std::endl;
            LPWSTR pArgs[] = {argv[1]};
            formatResult = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY, NULL, lastError, 0, msgBuffer.data(), messageBufferSize, (va_list*)pArgs);
        }
        else
        {
            formatResult = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, lastError, 0, msgBuffer.data(), messageBufferSize, NULL);
        }

        
        if (formatResult == 0)
        {
            std::wcout << "failed to load a more detailed message. Sorry." << std::endl;
        }
        else
        {
            std::wstring message{ msgBuffer.data() };
            std::wcout << message << std::endl;
        }
    }
}
