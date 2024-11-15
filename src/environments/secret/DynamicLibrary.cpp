//
// Created by vidal on 10/12/2024.
//

#include "DynamicLibrary.h"
#include <iostream>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

DynamicLibrary::DynamicLibrary(const std::string& path) : handle(nullptr) {
#if defined(_WIN32) || defined(_WIN64)
    handle = static_cast<void*>(LoadLibraryA(path.c_str()));
    if (!handle) {
        throw std::runtime_error("Failed to load library: " + path);
    }
#else
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle) {
        throw std::runtime_error("Failed to load library: " + std::string(dlerror()));
    }
#endif
}

DynamicLibrary::~DynamicLibrary() {
    if (handle) {
#if defined(_WIN32) || defined(_WIN64)
        FreeLibrary(static_cast<HMODULE>(handle));
#else
        dlclose(handle);
#endif
        handle = nullptr;
    }
}

DynamicLibrary::DynamicLibrary(DynamicLibrary&& other) noexcept : handle(other.handle) {
    other.handle = nullptr;
}

DynamicLibrary& DynamicLibrary::operator=(DynamicLibrary&& other) noexcept {
    if (this != &other) {
        if (handle) {
#if defined(_WIN32) || defined(_WIN64)
            FreeLibrary(static_cast<HMODULE>(handle));
#else
            dlclose(handle);
#endif
        }
        handle = other.handle;
        other.handle = nullptr;
    }
    return *this;
}

void* DynamicLibrary::loadSymbol(const std::string& name) {
#if defined(_WIN32) || defined(_WIN64)
    void* symbol = static_cast<void*>(GetProcAddress(static_cast<HMODULE>(handle), name.c_str()));
    if (!symbol) {
        throw std::runtime_error("Failed to load symbol: " + name);
    }
    return symbol;
#else
    dlerror(); // Clear existing errors
    void* symbol = dlsym(handle, name.c_str());
    const char* error = dlerror();
    if (error) {
        throw std::runtime_error("Failed to load symbol: " + name + ", error: " + std::string(error));
    }
    return symbol;
#endif
}
