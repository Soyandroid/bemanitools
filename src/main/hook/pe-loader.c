#include <stdio.h>

#include "hook/pe.h"
#include "hook/pe-loader.h"

HRESULT pe_loader_load_from_file(const char* name, HMODULE* module)
{
    FILE* file;
    void* data;
    size_t size;

    assert(name);
    assert(module);

    *module = NULL;

    file = fopen(name, "rb");

    if (!file) {
        return ERROR_OPEN_FAILED;
    }

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);

    data = malloc(size);

    if (!data) {
        fclose(file);
        return ERROR_OUTOFMEMORY;
    }

    if (fread(data, size, 1, file) != 1) {
        free(data);
        fclose(file);
        return ERROR_READ_FAULT;
    }

    fclose(file);

    return pe_loader_load_from_mem(data, size, module);
}

HRESULT pe_loader_load_from_mem(void* data, size_t size, HMODULE* module)
{
    assert(data);
    assert(size > 0);
    assert(module);

    *module = NULL;

    
}