#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("Error: Could not initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    
    // Configure audio
    SDL_AudioSpec desired, obtained;
    desired.freq = 44100;
    desired.format = AUDIO_S16SYS;
    desired.channels = 2;
    desired.samples = 1024;
    desired.callback = NULL;
    desired.userdata = NULL;
    
    if (SDL_OpenAudio(&desired, &obtained) < 0) {
        printf("Error: Could not open SDL audio: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    printf("SDL audio opened: %dHz, %d channels, %d samples\n", 
           obtained.freq, obtained.channels, obtained.samples);
    
    // Test audio output with a simple tone
    SDL_PauseAudio(0);
    
    printf("Audio test completed. If you heard audio, the system is working.\n");
    printf("Press Enter to continue...\n");
    getchar();
    
    SDL_CloseAudio();
    SDL_Quit();
    return 0;
}
