#ifndef _TVG_TTF_LOADER_H_
#define _TVG_TTF_LOADER_H_

#include "tvgCommon.h"
#include "tvgLoadModule.h"

#ifdef THORVG_TTF_LOADER_SUPPORT
    // Real TTF loader implementation would go here
    // For now, this is just a placeholder
#else
    // Dummy class when TTF loader support is disabled
    class TtfLoader : public LoadModule
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

#endif // _TVG_TTF_LOADER_H_
