/*
 * Real Weston GUI Application - HelloWorld
 * A true Wayland client that connects to the Weston compositor
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>

/* Wayland includes */
#include <wayland-client.h>
#include <wayland-client-protocol.h>

/* XDG Shell includes */
#include "xdg-shell-client-protocol.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300
#define APP_TITLE "Real Weston GUI Application"

struct weston_helloworld {
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_compositor *compositor;
    struct wl_shm *shm;
    struct xdg_wm_base *xdg_wm_base;
    struct wl_surface *surface;
    struct xdg_surface *xdg_surface;
    struct xdg_toplevel *xdg_toplevel;
    struct wl_buffer *buffer;
    void *data;
    int fd;
    int running;
    time_t start_time;
    int frame_count;
    double animation_time;
};

/* Forward declarations */
static void draw_weston_frame(struct weston_helloworld *app);

static void
registry_global(void *data, struct wl_registry *registry,
                uint32_t name, const char *interface, uint32_t version)
{
    struct weston_helloworld *app = data;
    
    if (strcmp(interface, wl_compositor_interface.name) == 0) {
        app->compositor = wl_registry_bind(registry, name,
                                          &wl_compositor_interface, 4);
        printf("Weston HelloWorld: Connected to wl_compositor\n");
    } else if (strcmp(interface, wl_shm_interface.name) == 0) {
        app->shm = wl_registry_bind(registry, name,
                                   &wl_shm_interface, 1);
        printf("Weston HelloWorld: Connected to wl_shm\n");
    } else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
        app->xdg_wm_base = wl_registry_bind(registry, name,
                                           &xdg_wm_base_interface, 1);
        printf("Weston HelloWorld: Connected to xdg_wm_base\n");
    }
}

static void
registry_global_remove(void *data, struct wl_registry *registry, uint32_t name)
{
    /* This space deliberately left blank */
}

static const struct wl_registry_listener registry_listener = {
    registry_global,
    registry_global_remove
};

static void
xdg_surface_configure(void *data, struct xdg_surface *xdg_surface, uint32_t serial)
{
    struct weston_helloworld *app = data;
    
    printf("Weston HelloWorld: Surface configured by Weston compositor\n");
    xdg_surface_ack_configure(xdg_surface, serial);
    
    /* Draw the frame */
    draw_weston_frame(app);
}

static const struct xdg_surface_listener xdg_surface_listener = {
    xdg_surface_configure,
};

static void
xdg_toplevel_configure(void *data, struct xdg_toplevel *xdg_toplevel,
                       int32_t width, int32_t height, struct wl_array *states)
{
    printf("Weston HelloWorld: Toplevel configured by Weston: %dx%d\n", width, height);
}

static void
xdg_toplevel_close(void *data, struct xdg_toplevel *xdg_toplevel)
{
    struct weston_helloworld *app = data;
    printf("Weston HelloWorld: Close requested by Weston compositor\n");
    app->running = 0;
}

static const struct xdg_toplevel_listener xdg_toplevel_listener = {
    xdg_toplevel_configure,
    xdg_toplevel_close,
};

static int
create_shm_buffer(int width, int height, void **data)
{
    int fd, size, stride;
    char name[] = "/weston-helloworld-XXXXXX";
    
    stride = width * 4;
    size = stride * height;
    
    fd = shm_open(name, O_CREAT | O_RDWR | O_EXCL, 0600);
    if (fd < 0) {
        perror("shm_open");
        return -1;
    }
    
    shm_unlink(name);
    
    if (ftruncate(fd, size) < 0) {
        perror("ftruncate");
        close(fd);
        return -1;
    }
    
    *data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (*data == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }
    
    return fd;
}

static void
draw_weston_frame(struct weston_helloworld *app)
{
    uint32_t *pixels = app->data;
    double time = app->animation_time;
    
    /* Clear background with dark blue */
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {
            pixels[y * WINDOW_WIDTH + x] = 0xFF2E3440; /* Dark blue background */
        }
    }
    
    /* Draw "Hello, World!!!" text as pixels */
    const char *message = "Hello, World!!!, 안녕하세요.";
    int msg_len = strlen(message);
    int start_x = (WINDOW_WIDTH - msg_len * 8) / 2;
    int start_y = WINDOW_HEIGHT / 2;
    
    /* Simple 8x8 pixel font for each character */
    for (int i = 0; i < msg_len; i++) {
        char c = message[i];
        int char_x = start_x + i * 8;
        
        /* Draw each character as a simple pattern */
        for (int cy = 0; cy < 8; cy++) {
            for (int cx = 0; cx < 8; cx++) {
                int px = char_x + cx;
                int py = start_y + cy;
                
                if (px >= 0 && px < WINDOW_WIDTH && py >= 0 && py < WINDOW_HEIGHT) {
                    /* Simple character patterns */
                    uint32_t color = 0xFFECEFF4; /* Light text color */
                    
                    /* Draw different patterns for different characters */
                    if (c == 'H') {
                        if (cx == 0 || cx == 7 || (cy == 3 && cx >= 1 && cx <= 6)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == 'e') {
                        if ((cy == 0 && cx >= 1 && cx <= 5) || (cy == 3 && cx >= 1 && cx <= 5) || 
                            (cy == 7 && cx >= 1 && cx <= 5) || (cx == 1 && cy >= 0 && cy <= 7)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == 'l') {
                        if (cx == 1 && cy >= 0 && cy <= 7) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == 'o') {
                        if ((cy == 0 && cx >= 1 && cx <= 5) || (cy == 7 && cx >= 1 && cx <= 5) ||
                            (cx == 1 && cy >= 1 && cy <= 6) || (cx == 6 && cy >= 1 && cy <= 6)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == ',') {
                        if ((cx == 2 && cy == 6) || (cx == 3 && cy == 7)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == ' ') {
                        /* Space - no pixels */
                    } else if (c == 'W') {
                        if ((cx == 0 && cy >= 0 && cy <= 6) || (cx == 7 && cy >= 0 && cy <= 6) ||
                            (cx == 1 && cy == 6) || (cx == 6 && cy == 6) ||
                            (cx == 2 && cy == 5) || (cx == 5 && cy == 5) ||
                            (cx == 3 && cy == 4) || (cx == 4 && cy == 4)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == 'r') {
                        if ((cx == 1 && cy >= 0 && cy <= 7) || (cy == 0 && cx >= 1 && cx <= 5) ||
                            (cy == 3 && cx >= 1 && cx <= 5) || (cy == 7 && cx >= 1 && cx <= 5)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == 'd') {
                        if ((cx == 6 && cy >= 0 && cy <= 7) || (cy == 0 && cx >= 1 && cx <= 5) ||
                            (cy == 7 && cx >= 1 && cx <= 5) || (cx == 1 && cy >= 1 && cy <= 6)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    } else if (c == '!') {
                        if ((cx == 3 && cy >= 0 && cy <= 5) || (cx == 3 && cy == 7)) {
                            pixels[py * WINDOW_WIDTH + px] = color;
                        }
                    }
                }
            }
        }
    }
    
    /* Draw animated border */
    int border_color = 0xFF88C0D0 + ((int)(time * 50) % 0x1000000);
    for (int x = 0; x < WINDOW_WIDTH; x++) {
        if (x < WINDOW_WIDTH) {
            pixels[0 * WINDOW_WIDTH + x] = border_color; /* Top border */
            pixels[(WINDOW_HEIGHT-1) * WINDOW_WIDTH + x] = border_color; /* Bottom border */
        }
    }
    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        if (y < WINDOW_HEIGHT) {
            pixels[y * WINDOW_WIDTH + 0] = border_color; /* Left border */
            pixels[y * WINDOW_WIDTH + (WINDOW_WIDTH-1)] = border_color; /* Right border */
        }
    }
    
    /* Commit the surface */
    wl_surface_attach(app->surface, app->buffer, 0, 0);
    wl_surface_damage(app->surface, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    wl_surface_commit(app->surface);
    
    app->frame_count++;
}

static void
run_weston_application(struct weston_helloworld *app)
{
    printf("Weston HelloWorld: Running real Weston GUI application...\n");
    printf("Weston HelloWorld: Connected to Weston compositor\n");
    printf("Weston HelloWorld: This is a true Wayland client\n");
    printf("Weston HelloWorld: Hello, World!!! - Displaying in Weston GUI\n");
    
    while (app->running) {
        /* Update animation */
        app->animation_time += 0.1;
                /* Draw frame */
        draw_weston_frame(app);
        
        /* Dispatch events */
        wl_display_dispatch_pending(app->display);
        
        /* Small delay */
        usleep(100000); /* 100ms */
    }
}

int main(int argc, char *argv[])
{
    struct weston_helloworld app = {0};
    struct wl_shm_pool *pool;
    int fd, size, stride;
    
    printf("Real Weston GUI Application\n");
    printf("============================\n");
    printf("This is a TRUE Weston GUI application\n");
    printf("It connects to the Weston compositor via Wayland\n");
    printf("Creating a real Wayland client...\n");
    
    /* Connect to Wayland display */
    app.display = wl_display_connect(NULL);
    if (app.display == NULL) {
        fprintf(stderr, "Failed to connect to Wayland display\n");
        fprintf(stderr, "Make sure Weston compositor is running!\n");
        return 1;
    }
    
    printf("Weston HelloWorld: Connected to Wayland display\n");
    
    /* Get registry */
    app.registry = wl_display_get_registry(app.display);
    wl_registry_add_listener(app.registry, &registry_listener, &app);
    wl_display_roundtrip(app.display);
    
    if (app.compositor == NULL) {
        fprintf(stderr, "No wl_compositor found\n");
        return 1;
    }
    
    if (app.xdg_wm_base == NULL) {
        fprintf(stderr, "No xdg_wm_base found\n");
        return 1;
    }
    
    if (app.shm == NULL) {
        fprintf(stderr, "No wl_shm found\n");
        return 1;
    }
    
    printf("Weston HelloWorld: All Wayland interfaces found\n");
    
    /* Create surface */
    app.surface = wl_compositor_create_surface(app.compositor);
    if (app.surface == NULL) {
        fprintf(stderr, "Failed to create surface\n");
        return 1;
    }
    
    /* Create XDG surface */
    app.xdg_surface = xdg_wm_base_get_xdg_surface(app.xdg_wm_base, app.surface);
    if (app.xdg_surface == NULL) {
        fprintf(stderr, "Failed to create xdg surface\n");
        return 1;
    }
    
    xdg_surface_add_listener(app.xdg_surface, &xdg_surface_listener, &app);
    
    /* Create toplevel */
    app.xdg_toplevel = xdg_surface_get_toplevel(app.xdg_surface);
    if (app.xdg_toplevel == NULL) {
        fprintf(stderr, "Failed to create xdg toplevel\n");
        return 1;
    }
    
    xdg_toplevel_add_listener(app.xdg_toplevel, &xdg_toplevel_listener, &app);
    xdg_toplevel_set_title(app.xdg_toplevel, APP_TITLE);
    
    /* Set window size */
    xdg_toplevel_set_min_size(app.xdg_toplevel, WINDOW_WIDTH, WINDOW_HEIGHT);
    xdg_toplevel_set_max_size(app.xdg_toplevel, WINDOW_WIDTH, WINDOW_HEIGHT);
    
    /* Create shared memory buffer */
    fd = create_shm_buffer(WINDOW_WIDTH, WINDOW_HEIGHT, &app.data);
    if (fd < 0) {
        fprintf(stderr, "Failed to create shared memory buffer\n");
        return 1;
    }
    
    stride = WINDOW_WIDTH * 4;
    size = stride * WINDOW_HEIGHT;
    pool = wl_shm_create_pool(app.shm, fd, size);
    app.buffer = wl_shm_pool_create_buffer(pool, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 
                                          stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
    
    /* Initialize application */
    app.running = 1;
    app.start_time = time(NULL);
    app.frame_count = 0;
    app.animation_time = 0.0;
    
    printf("Weston HelloWorld: Real Weston GUI application created!\n");
    printf("Weston HelloWorld: Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    printf("Weston HelloWorld: This is a TRUE Wayland client connected to Weston\n");
    
    /* Run the application */
    run_weston_application(&app);
    
    /* Cleanup */
    printf("Weston HelloWorld: Cleaning up...\n");
    munmap(app.data, size);
    wl_buffer_destroy(app.buffer);
    xdg_toplevel_destroy(app.xdg_toplevel);
    xdg_surface_destroy(app.xdg_surface);
    wl_surface_destroy(app.surface);
    xdg_wm_base_destroy(app.xdg_wm_base);
    wl_compositor_destroy(app.compositor);
    wl_shm_destroy(app.shm);
    wl_display_disconnect(app.display);
    
    printf("Weston HelloWorld: Real Weston GUI application finished\n");
    return 0;
}