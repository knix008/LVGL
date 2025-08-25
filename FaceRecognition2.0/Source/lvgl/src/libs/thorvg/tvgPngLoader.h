#ifndef _TVG_PNG_LOADER_H_
#define _TVG_PNG_LOADER_H_

#include "tvgCommon.h"
#include "tvgLoadModule.h"

#ifdef THORVG_PNG_LOADER_SUPPORT
    // Real PNG loader implementation would go here
    // For now, this is just a placeholder
#else
    // Dummy class when PNG loader support is disabled
    class PngLoader : public LoadModule
    {
    public:
        bool open(const string& path) override { return false; }
        bool open(const char* data, uint32_t size, const string& mimeType, bool copy) override { return false; }
        bool read() override { return false; }
        bool close() override { return false; }
        unique_ptr<Surface> bitmap() override { return nullptr; }
        uint32_t color() override { return 0; }
        bool viewbox(float* x, float* y, float* w, float* h) override { return false; }
        const string& path() override { static string empty; return empty; }
    };
#endif

#endif // _TVG_PNG_LOADER_H_
