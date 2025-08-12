#ifndef _TVG_GIF_SAVER_H_
#define _TVG_GIF_SAVER_H_

#include "tvgCommon.h"
#include "tvgSaveModule.h"

#ifdef THORVG_GIF_SAVER_SUPPORT
    // Real GIF saver implementation would go here
    // For now, this is just a placeholder
#else
    // Dummy class when GIF saver support is disabled
    class GifSaver : public SaveModule
    {
    public:
        bool save(Paint* paint, const string& path) override { return false; }
        bool save(Paint* paint, const char* data, uint32_t size, const string& mimeType, bool copy) override { return false; }
        bool close() override { return false; }
        bool write(const char* data, uint32_t size) override { return false; }
    };
#endif

#endif // _TVG_GIF_SAVER_H_
