#include <Windows.h>
#include <chrono>
#include "Config.h"
#include "Player.h"
#include "Map.h"
#include "Renderer.h"

int main()
{
    DWORD dwBytesWritten = 0; 
    wchar_t* screen = new wchar_t[ScreenWidth * ScreenHeight];
    HANDLE hConsole = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL
    );
    SetConsoleActiveScreenBuffer(hConsole);

    Player player;
    std::wstring map = CreateMap();

    auto tp1 = std::chrono::system_clock::now();

    while (true)
    {
        // 1) Time
        auto tp2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed = tp2 - tp1;
        tp1 = tp2;
        float dt = elapsed.count();

        // 2) INPUT + GAME LOGIC (WASD HERE)
        // Rotation
        if (GetAsyncKeyState('A') & 0x8000)
            player.angle -= (Speed * 0.75f) * dt;

        if (GetAsyncKeyState('D') & 0x8000)
            player.angle += (Speed * 0.75f) * dt;

        // Forward
        if (GetAsyncKeyState('W') & 0x8000)
        {
            float nx = player.x + sinf(player.angle) * Speed * dt;
            float ny = player.y + cosf(player.angle) * Speed * dt;

            if (map[(int)nx * MapWidth + (int)ny] != '#')
            {
                player.x = nx;
                player.y = ny;
            }
        }

        // Backward
        if (GetAsyncKeyState('S') & 0x8000)
        {
            float nx = player.x - sinf(player.angle) * Speed * dt;
            float ny = player.y - cosf(player.angle) * Speed * dt;

            if (map[(int)nx * MapWidth + (int)ny] != '#')
            {
                player.x = nx;
                player.y = ny;
            }
        }

        // 3) RENDER
        RenderFrame(screen, hConsole, map, player, dt);

        // 4) PRESENT
        DWORD dwBytesWritten;
        WriteConsoleOutputCharacter(
            hConsole,
            screen,
            ScreenWidth * ScreenHeight,
            { 0,0 },
            &dwBytesWritten
        );
    }
}