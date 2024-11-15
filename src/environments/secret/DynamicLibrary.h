//
// Created by vidal on 10/12/2024.
//

#ifndef INC_5A_RVJV_FULL_CPP_TEMPLATE_DYNAMICLIBRARY_H
#define INC_5A_RVJV_FULL_CPP_TEMPLATE_DYNAMICLIBRARY_H


#include <string>
#include <stdexcept>

class DynamicLibrary {
public:
    explicit DynamicLibrary(const std::string& path);
    ~DynamicLibrary();

    // Désactiver la copie
    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    // Permettre le déplacement
    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    // Chargement de symboles
    void* loadSymbol(const std::string& name);

private:
    void* handle;
};

#endif //INC_5A_RVJV_FULL_CPP_TEMPLATE_DYNAMICLIBRARY_H
