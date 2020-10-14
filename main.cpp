#include <iostream>
#include <thread>


using namespace std::chrono_literals;


int main() {
    const int imageWidth = 256;
    const int imageHeight = 256;

    std::cout << "P3\n" << imageWidth << " " << imageHeight << "\n255\n";

    for (int j = imageHeight - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << std::flush;

        // uncomment the line below to slow the rendering and see the progress bar
        // std::this_thread::sleep_for(50ms);

        for (int i = 0; i < imageWidth; ++i) {
            int R = static_cast<int>(i);
            int G = static_cast<int>(j);
            int B = static_cast<int>(0);

            std::cout << R << " " << G << " " << B << "\n";
        }
    }

    std::cerr << "\nDone\n";

    return 0;
}
