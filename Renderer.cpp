#include "Renderer.h"
#include "Config.h"
#include <vector>
#include <algorithm>
#include <cmath>

void RenderFrame(
	wchar_t* screen,
	HANDLE hConsole,
	const std::wstring& map,
	const Player& player,
	float fElapsedTime
)
{
    for (int x = 0; x < ScreenWidth; x++)
    {
        // Calculate ray angle
        float fRayAngle =
            (player.angle - FOV / 2.0f) +
            ((float)x / (float)ScreenWidth) * FOV;

        float fStepSize = 0.1f;
        float fDistanceToWall = 0.0f;

        bool bHitWall = false;
        bool bBoundary = false;

        float fEyeX = sinf(fRayAngle);
        float fEyeY = cosf(fRayAngle);

        // Ray cast
        while (!bHitWall && fDistanceToWall < Depth)
        {
            fDistanceToWall += fStepSize;

            int nTestX = (int)(player.x + fEyeX * fDistanceToWall);
            int nTestY = (int)(player.y + fEyeY * fDistanceToWall);

            // Out of bounds
            if (nTestX < 0 || nTestX >= MapWidth ||
                nTestY < 0 || nTestY >= MapHeight)
            {
                bHitWall = true;
                fDistanceToWall = Depth;
            }
            else
            {
                if (map[nTestX * MapWidth + nTestY] == '#')
                {
                    bHitWall = true;

                    // Boundary detection
                    std::vector<std::pair<float, float>> p;

                    for (int tx = 0; tx < 2; tx++)
                        for (int ty = 0; ty < 2; ty++)
                        {
                            float vy = (float)nTestY + ty - player.y;
                            float vx = (float)nTestX + tx - player.x;
                            float d = std::sqrt(vx * vx + vy * vy);
                            float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
                            p.emplace_back(d, dot);
                        }

                    std::sort(p.begin(), p.end(),
                        [](const auto& a, const auto& b)
                        {
                            return a.first < b.first;
                        });

                    float fBound = 0.01f;
                    if (acos(p[0].second) < fBound) bBoundary = true;
                    if (acos(p[1].second) < fBound) bBoundary = true;
                    if (acos(p[2].second) < fBound) bBoundary = true;
                }
            }
        }

        int nCeiling =
            (float)(ScreenHeight / 2.0f)
            - ScreenHeight / fDistanceToWall;
        int nFloor = ScreenHeight - nCeiling;

        short nShade = ' ';
        if (fDistanceToWall <= Depth / 4.0f)       nShade = 0x2588;
        else if (fDistanceToWall < Depth / 3.0f)   nShade = 0x2593;
        else if (fDistanceToWall < Depth / 2.0f)   nShade = 0x2592;
        else if (fDistanceToWall < Depth)          nShade = 0x2591;
        else                                       nShade = ' ';

        if (bBoundary)
            nShade = ' ';

        for (int y = 0; y < ScreenHeight; y++)
        {
            if (y <= nCeiling)
            {
                screen[y * ScreenWidth + x] = ' ';
            }
            else if (y > nCeiling && y <= nFloor)
            {
                screen[y * ScreenWidth + x] = nShade;
            }
            else
            {
                float b = 1.0f - (
                    ((float)y - ScreenHeight / 2.0f) /
                    ((float)ScreenHeight / 2.0f)
                    );

                if (b < 0.25f)       nShade = '#';
                else if (b < 0.5f)   nShade = 'x';
                else if (b < 0.75f)  nShade = '.';
                else if (b < 0.9f)   nShade = '-';
                else                 nShade = ' ';

                screen[y * ScreenWidth + x] = nShade;
            }
        }
    }

    // Stats
    swprintf_s(
        screen,
        40,
        L"X=%3.2f Y=%3.2f A=%3.2f FPS=%3.2f ",
        player.x,
        player.y,
        player.angle,
        1.0f / fElapsedTime
    );

    // Map display
    for (int nx = 0; nx < MapWidth; nx++)
        for (int ny = 0; ny < MapHeight; ny++)
            screen[(ny + 1) * ScreenWidth + nx] =
            map[ny * MapWidth + nx];

    screen[((int)player.x + 1) * ScreenWidth + (int)player.y] = 'P';

    screen[ScreenWidth * ScreenHeight - 1] = '\0';
}