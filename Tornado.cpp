#include <algorithm>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <string>
#include <sys/ioctl.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <fstream>

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

enum class TornadoType {
    Rope,
    Cone,
    Cylinder,
    VShaped,
    MultiVortex
};

TornadoType parseTornadoType(int argc, char* argv[]) {
    TornadoType type = TornadoType::VShaped;

    for (int i = 1; i < argc; ++i) {
        const std::string argument = argv[i];

        if (argument == "--rope") {
            type = TornadoType::Rope;
        } else if (argument == "--cone") {
            type = TornadoType::Cone;
        } else if (argument == "--cylinder") {
            type = TornadoType::Cylinder;
        } else if (argument == "--v-shaped") {
            type = TornadoType::VShaped;
        } else if (argument == "--multivortex") {
            type = TornadoType::MultiVortex;
        } else {
            std::cerr << "Unknown flag: " << argument << '\n';
        }
    }

    return type;
}

float getHalfWidth(
    TornadoType type,
    float progress,
    int screenWidth,
    float time
) {
    float halfWidth = 2.0f;

    switch (type) {
        case TornadoType::Rope: {
            halfWidth =
                screenWidth * 0.022f
                + (1.0f - progress) * screenWidth * 0.012f
                + std::sin(progress * 12.0f + time) * 0.7f;
            break;
        }

        case TornadoType::Cone: {
            const float topWidth = screenWidth * 0.18f;
            const float bottomWidth = screenWidth * 0.025f;

            halfWidth =
                topWidth * (1.0f - progress)
                + bottomWidth * progress;
            break;
        }

        case TornadoType::Cylinder: {
            halfWidth = screenWidth * 0.095f;

            // Ледь помітна нерівність країв.
            halfWidth +=
                std::sin(progress * 9.0f - time * 0.7f) * 0.5f;
            break;
        }

        case TornadoType::VShaped: {
            const float topWidth = screenWidth * 0.27f;
            const float bottomWidth = screenWidth * 0.018f;

            /*
             * pow робить верх ширшим і виразніше
             * відрізняє V-shaped від звичайного cone.
             */
            const float shape =
                std::pow(1.0f - progress, 0.72f);

            halfWidth =
                bottomWidth
                + (topWidth - bottomWidth) * shape;
            break;
        }

        case TornadoType::MultiVortex: {
            // Межі всієї головної циркуляції.
            const float topWidth = screenWidth * 0.25f;
            const float bottomWidth = screenWidth * 0.13f;

            halfWidth =
                topWidth * (1.0f - progress)
                + bottomWidth * progress;
            break;
        }
    }

    const float breathing =
        std::sin(time * 0.8f - progress * 5.0f)
        * screenWidth * 0.004f;

    return std::max(1.2f, halfWidth + breathing);
}
float getCenterMovement(
    TornadoType type,
    float progress,
    int screenWidth,
    float time
) {
    float strength = 1.0f;

    switch (type) {
        case TornadoType::Rope:
            strength =
                2.0f + screenWidth * 0.025f;
            break;

        case TornadoType::Cone:
            strength =
                1.0f + screenWidth * 0.008f;
            break;

        case TornadoType::Cylinder:
            strength =
                0.5f + screenWidth * 0.003f;
            break;

        case TornadoType::VShaped:
            strength =
                0.8f + screenWidth * 0.006f;
            break;

        case TornadoType::MultiVortex:
            strength =
                0.6f + screenWidth * 0.004f;
            break;
    }

    // Верх рухається трохи більше, ніж низ.
    strength *= 0.45f + (1.0f - progress) * 0.55f;

    return std::sin(
        time * 0.65f + progress * 4.0f
    ) * strength;
}

int main(int argc, char* argv[]) {
    const TornadoType tornadoType =
        parseTornadoType(argc, argv);

	std::signal(SIGINT, handleSignal);
	std::signal(SIGTERM, handleSignal);	

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
                const float centerMovement =
    getCenterMovement(
        tornadoType,
        progress,
        screenWidth,
        time
    );

const float centerX =
    screenWidth / 2.0f + centerMovement;

const float halfWidth =
    getHalfWidth(
        tornadoType,
        progress,
        screenWidth,
        time
    ); 
		const float pixelX = static_cast<float>(x);
const float distance = std::abs(pixelX - centerX);

bool insideTornado = distance <= halfWidth;

float textureCenterX = centerX;
float textureHalfWidth = halfWidth;

if (tornadoType == TornadoType::MultiVortex) {
    insideTornado = false;

    constexpr int vortexCount = 3;
    constexpr float pi = 3.14159265f;

    /*
     * У верхній частині вихори зливаються
     * в одну широку циркуляцію.
     */
    if (progress < 0.38f && distance <= halfWidth) {
        insideTornado = true;
    }

    for (int vortex = 0; vortex < vortexCount; ++vortex) {
        const float phase =
            time * 2.0f
            + progress * 2.6f
            + vortex * (2.0f * pi / vortexCount);

        /*
         * Через sin видно умовний рух вихорів
         * по орбіті в 2D-проєкції.
         */
        const float orbitRadius =
            halfWidth * (0.30f + progress * 0.28f);

        const float vortexCenterX =
            centerX + std::sin(phase) * orbitRadius;

        const float vortexHalfWidth =
            std::max(
                1.2f,
                screenWidth
                    * (0.014f
                       + (1.0f - progress) * 0.018f)
            );

        const float vortexDistance =
            std::abs(pixelX - vortexCenterX);

        if (vortexDistance <= vortexHalfWidth) {
            insideTornado = true;
            textureCenterX = vortexCenterX;
            textureHalfWidth = vortexHalfWidth;
            break;
        }
    }
}

if (!insideTornado) {
    frame += ' ';
    continue;
}

const float localX =
    (pixelX - textureCenterX) / textureHalfWidth;

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
