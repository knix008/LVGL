#ifndef _TVG_GL_RENDERER_H_
#define _TVG_GL_RENDERER_H_

#include "tvgCommon.h"
#include "tvgRender.h"

#ifdef THORVG_GL_RASTER_SUPPORT
    // Real GL renderer implementation would go here
    // For now, this is just a placeholder
#else
    // Dummy class when GL support is disabled
    class GlRenderer : public RenderMethod
    {
    public:
        static bool init(uint32_t threads) { return false; }
        static void term() {}
        static RenderMethod* gen() { return nullptr; }
        
        bool target(int32_t id, uint32_t w, uint32_t h) { return false; }
        void viewport(const SwViewport& vp) {}
        bool clear() { return false; }
        bool sync() { return false; }
        bool preRender() { return false; }
        bool render(const RenderData& data) { return false; }
        bool postRender() { return false; }
        bool dispose(const RenderData& data) { return false; }
        RenderRegion region(const RenderData& data) { return {0, 0, 0, 0}; }
        RenderRegion viewport() { return {0, 0, 0, 0}; }
        bool viewport(const RenderRegion& vp) { return false; }
    };
#endif

#endif // _TVG_GL_RENDERER_H_
