//
// Created by vidal on 10/12/2024.
//

#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_PATHS_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_PATHS_H

// Détection du système d'exploitation et de l'architecture
#if defined(_WIN32) || defined(_WIN64)
#define SECRET_ENV_PATH "../libs/secret_envs.dll"
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
    #if TARGET_OS_MAC
        // Détection de l'architecture sur macOS
        #if defined(__x86_64__) || defined(_M_X64)
            #define SECRET_ENV_PATH "./libs/libsecret_envs_intel_macos.dylib"
        #elif defined(__aarch64__) || defined(_M_ARM64)
            #define SECRET_ENV_PATH "./libs/libsecret_envs.dylib"
        #else
            #error "Architecture non supportée sur macOS"
        #endif
    #else
        #error "Système Apple non supporté"
    #endif
#elif defined(__linux__)
    #define SECRET_ENV_PATH "./libs/libsecret_envs.so"
#else
    #error "Système d'exploitation non supporté"
#endif


#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_PATHS_H
