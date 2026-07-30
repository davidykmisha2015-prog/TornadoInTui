#include <algorithm>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>

bool running = true;

void restoreTerminal() {
	std::cout << "\x1b[0m"
	 << "\x1b[?25h"
	 << std::flush;
}

void handleSignal(int) {
	running = false;
}

struct TerminalSize {
	int columns;
	int rows;
};

TerminalSize getTerminalSize() {
	winsize size{};

	if(ioctl(STDOUT_FILENO
			      , TIOCGWINSZ, &size) == -1) {
		return{80, 24};
	}

	return {
		static_cast<int>(size.ws_col),
		static_cast<int>(size.ws_row)
	};
}

int main() {
	std::signal(SIGINT, handleSignal);
	std::signal(SIGINT, handleSignal);

        std::cout << "\x1b[2J"
		  << "\x1b[?25l";

	float time = 0.0f;

	while (running) {
    const TerminalSize terminal = getTerminalSize();

    const int screenWidth =
        std::max(terminal.columns, 20);

    const int screenHeight =
        std::max(terminal.rows - 1, 10);

    const int tornadoHeight =
        std::clamp(screenHeight * 4 / 5, 8, screenHeight);

    const int verticalOffset =
        std::max(0, (screenHeight - tornadoHeight) / 2);

    const float topHalfWidth =
        std::max(8.0f, screenWidth * 0.28f);

    const float bottomHalfWidth =
        std::max(5.0f, screenWidth * 0.14f);

    std::string frame;

			frame.reserve(
				static_cast<std::size_t>(screenWidth * screenHeight * 3)
			     );


		frame += "\x1b[H";
		frame += "\x1b[90m";

	        for (int screenY = 0; screenY < screenHeight; ++screenY) {
            const int tornadoY = screenY - verticalOffset;

            for (int x = 0; x < screenWidth; ++x) {
                if (tornadoY < 0 || tornadoY >= tornadoHeight) {
                    frame += ' ';
                    continue;
                }

                const float progress =
                    static_cast<float>(tornadoY) /
                    static_cast<float>(tornadoHeight - 1);

                /*
                 * Верх рухається сильніше за низ.
                 * Різна фаза на кожній висоті викручує форму.
                 */
                const float swayStrength =
                    1.5f + (1.0f - progress) * screenWidth * 0.025f;

                const float centerMovement =
                    std::sin(time * 0.75f + progress * 4.2f) *
                    swayStrength;

                const float centerX =
                    screenWidth / 2.0f + centerMovement;

                // Основна wedge-форма.
                float halfWidth =
                    topHalfWidth +
                    (bottomHalfWidth - topHalfWidth) * progress;

                /*
                 * Краї трохи стискаються й розширюються.
                 * На різній висоті це відбувається неодночасно.
                 */
                const float breathing =
                    std::sin(time * 1.1f - progress * 6.0f) *
                    screenWidth * 0.012f;

                halfWidth += breathing;
                halfWidth = std::max(3.0f, halfWidth);

                const float distance =
                    std::abs(static_cast<float>(x) - centerX);

                if (distance > halfWidth) {
                    frame += ' ';
                    continue;
                }

                /*
                 * Нормалізована координата всередині торнадо:
                 * -1 біля лівого краю, 0 у центрі, +1 справа.
                 */
                const float localX =
                    (static_cast<float>(x) - centerX) / halfWidth;

                /*
                 * Основний рух текстури.
                 * progress змушує різні шари обертатися
                 * з трохи різною фазою.
                 */
                const float rotation =
                    std::sin(
                        localX * 8.0f
                        - time * 2.0f
                        + progress * 7.0f
                    );

                // Друга хвиля прибирає вигляд простих рівних смуг.
                const float turbulence =
                    0.45f * std::sin(
                        localX * 17.0f
                        + time * 1.25f
                        - progress * 11.0f
                    );

                const float shade = rotation + turbulence;

                if (shade < -0.75f) {
                    frame += "░";
                } else if (shade < -0.15f) {
                    frame += "▒";
                } else if (shade < 0.55f) {
                    frame += "▓";
                } else {
                    frame += "█";
                }
            }

            if (screenY < screenHeight - 1) {
                frame += '\n';
            }
        }

        std::cout << frame << std::flush;

        time += 0.035f;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(16)
        );
    }

    restoreTerminal();
    std::cout << '\n';

    return 0;
}
