#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <GL/gl.h>
#include <linux/input.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "xdg-shell-client-protocol.h"

struct display {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shm *shm;
    struct xdg_wm_base *xdg_wm_base;
};

struct window {
    struct display *display;
    int width, height;
    struct wl_surface *surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct wl_buffer *buffer;
    void *shm_data;
    int configured;
};

static void
xdg_surface_handle_configure(void *data, struct xdg_surface *xdg_surface,
                            uint32_t serial)
{
    struct window *window = data;
    xdg_surface_ack_configure(xdg_surface, serial);
    window->configured = 1;
}

static const struct xdg_surface_listener xdg_surface_listener = {
    xdg_surface_handle_configure,
};

static void
xdg_toplevel_handle_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                             int32_t width, int32_t height,
                             struct wl_array *states)
{
    struct window *window = data;
    if (width > 0 && height > 0) {
        window->width = width;
        window->height = height;
    }
}

static void
xdg_toplevel_handle_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
    exit(0);
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    xdg_toplevel_handle_configure,
    xdg_toplevel_handle_close,
};

static void
xdg_wm_base_ping(void *data, struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
    xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
    xdg_wm_base_ping,
};

static int
create_shm_file(off_t size)
{
    static const char template[] = "/weston-hello-XXXXXX";
    const char *path;
    char *name;
    int fd;

    path = getenv("XDG_RUNTIME_DIR");
    if (!path) {
        errno = ENOENT;
        return -1;
    }

    name = malloc(strlen(path) + sizeof(template));
    if (!name)
        return -1;

    strcpy(name, path);
    strcat(name, template);

    fd = mkstemp(name);
    if (fd >= 0)
        unlink(name);

    free(name);

    if (fd < 0)
        return -1;

    if (ftruncate(fd, size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

static struct wl_buffer *
create_buffer(struct display *display, int width, int height)
{
    struct wl_shm_pool *pool;
    int stride = width * 4; // 4 bytes per pixel
    int size = stride * height;
    int fd;
    void *data;
    struct wl_buffer *buffer;

    fd = create_shm_file(size);
    if (fd < 0) {
        fprintf(stderr, "creating a buffer file for %d B failed: %m\n",
                size);
        exit(1);
    }

    data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        fprintf(stderr, "mmap failed: %m\n");
        close(fd);
        exit(1);
    }

    pool = wl_shm_create_pool(display->shm, fd, size);
    buffer = wl_shm_pool_create_buffer(pool, 0,
                                       width, height,
                                       stride,
                                       WL_SHM_FORMAT_XRGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    return buffer;
}

static void
paint_pixels(void *image, int width, int height)
{
    uint32_t *pixel = image;
    int n;
    
    // Fill with light blue background
    for (n = 0; n < width * height; n++) {
        *pixel++ = 0xFFADD8E6; // Light blue (ARGB format)
    }
    
    // Draw a simple geometric pattern instead of broken text
    int center_x = width / 2;
    int center_y = height / 2;
    
    // Draw a white circle
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int dx = x - center_x;
            int dy = y - center_y;
            int dist = dx * dx + dy * dy;
            if (dist < (width/4) * (width/4)) {
                ((uint32_t *)image)[y * width + x] = 0xFFFFFFFF; // White circle
            }
        }
    }
    
    // Draw a red rectangle in the center
    int rect_width = width / 3;
    int rect_height = height / 3;
    int rect_x = center_x - rect_width / 2;
    int rect_y = center_y - rect_height / 2;
    
    for (int y = rect_y; y < rect_y + rect_height; y++) {
        for (int x = rect_x; x < rect_x + rect_width; x++) {
            if (y >= 0 && y < height && x >= 0 && x < width) {
                ((uint32_t *)image)[y * width + x] = 0xFFFF0000; // Red rectangle
            }
        }
    }
}

static void
registry_handle_global(void *data, struct wl_registry *registry,
                      uint32_t id, const char *interface, uint32_t version)
{
    struct display *d = data;

    if (strcmp(interface, "wl_compositor") == 0) {
        d->compositor = wl_registry_bind(registry, id,
                                        &wl_compositor_interface, 1);
    } else if (strcmp(interface, "wl_shm") == 0) {
        d->shm = wl_registry_bind(registry, id,
                                 &wl_shm_interface, 1);
    } else if (strcmp(interface, "xdg_wm_base") == 0) {
        d->xdg_wm_base = wl_registry_bind(registry, id,
                                         &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(d->xdg_wm_base,
                                &xdg_wm_base_listener, d);
    }
}

static void
registry_handle_global_remove(void *data, struct wl_registry *registry,
                             uint32_t name)
{
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove
};

static struct display *
create_display(void)
{
    struct display *display;

    display = malloc(sizeof *display);
    if (display == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    
    printf("Connecting to Wayland display...\n");
    display->display = wl_display_connect(NULL);
    if (!display->display) {
        fprintf(stderr, "Failed to connect to Wayland display\n");
        exit(1);
    }
    printf("Connected to Wayland display\n");

    display->registry = wl_display_get_registry(display->display);
    wl_registry_add_listener(display->registry,
                            &registry_listener, display);

    printf("Waiting for registry...\n");
    wl_display_roundtrip(display->display);
    printf("Registry roundtrip complete\n");

    return display;
}

static void
destroy_display(struct display *display)
{
    if (display->xdg_wm_base)
        xdg_wm_base_destroy(display->xdg_wm_base);

    if (display->shm)
        wl_shm_destroy(display->shm);

    if (display->compositor)
        wl_compositor_destroy(display->compositor);

    wl_registry_destroy(display->registry);
    wl_display_disconnect(display->display);
    free(display);
}

static struct window *
create_window(struct display *display, int width, int height)
{
    struct window *window;

    window = calloc(1, sizeof *window);
    if (!window)
        return NULL;

    window->display = display;
    window->width = width;
    window->height = height;

    window->surface = wl_compositor_create_surface(display->compositor);
    window->xdg_surface = xdg_wm_base_get_xdg_surface(display->xdg_wm_base,
                                                       window->surface);
    xdg_surface_add_listener(window->xdg_surface,
                            &xdg_surface_listener, window);

    window->xdg_toplevel = xdg_surface_get_toplevel(window->xdg_surface);
    xdg_toplevel_add_listener(window->xdg_toplevel,
                             &xdg_toplevel_listener, window);

    xdg_toplevel_set_title(window->xdg_toplevel, "Wayland Hello World - Large Window");
    xdg_toplevel_set_app_id(window->xdg_toplevel, "hello-world");

    wl_surface_commit(window->surface);

    return window;
}

static void
destroy_window(struct window *window)
{
    if (window->buffer) {
        wl_buffer_destroy(window->buffer);
        munmap(window->shm_data, window->width * window->height * 4);
    }

    xdg_toplevel_destroy(window->xdg_toplevel);
    xdg_surface_destroy(window->xdg_surface);
    wl_surface_destroy(window->surface);
    free(window);
}

static void
redraw(struct window *window, void *data, uint32_t time)
{
    if (!window->configured)
        return;

    if (!window->buffer) {
        window->buffer = create_buffer(window->display,
                                      window->width, window->height);
        window->shm_data = mmap(NULL, window->width * window->height * 4,
                               PROT_READ | PROT_WRITE, MAP_SHARED,
                               -1, 0);
    }

    paint_pixels(window->shm_data, window->width, window->height);

    wl_surface_attach(window->surface, window->buffer, 0, 0);
    wl_surface_damage(window->surface, 0, 0, window->width, window->height);
    wl_surface_commit(window->surface);
}

int
main(int argc, char **argv)
{
    struct display *display;
    struct window *window;

    printf("Starting hello-world application...\n");
    printf("WAYLAND_DISPLAY: %s\n", getenv("WAYLAND_DISPLAY"));
    printf("DISPLAY: %s\n", getenv("DISPLAY"));
    
    display = create_display();
    printf("Display created successfully\n");
    
    window = create_window(display, 400, 300);
    printf("Window created successfully\n");

    // Initial draw
    printf("Drawing initial frame...\n");
    redraw(window, NULL, 0);
    printf("Initial draw complete\n");

    printf("Entering main loop...\n");
    while (wl_display_dispatch(display->display) != -1) {
        /* This space deliberately left blank */
    }

    printf("Cleaning up...\n");
    destroy_window(window);
    destroy_display(display);

    return 0;
}