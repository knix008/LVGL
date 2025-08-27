#ifndef _TVG_TVG_SAVER_H_
#define _TVG_TVG_SAVER_H_

#include "tvgCommon.h"
#include "tvgSaveModule.h"

#ifdef THORVG_TVG_SAVER_SUPPORT
    // Real TVG saver implementation would go here
    // For now, this is just a placeholder
#else
    // Dummy class when TVG saver support is disabled
    class TvgSaver : public SaveModule
    {
    public:
        bool save(Paint* paint, const string& path) override { return false; }
        bool save(Paint* paint, const char* data, uint32_t size, const string& mimeType, bool copy) override { return false; }
        bool close() override { return false; }
        bool write(const char* data, uint32_t size) override { return false; }
    };
#endif

#endif // _TVG_TVG_SAVER_H_
