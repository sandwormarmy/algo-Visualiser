#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int BAR_WIDTH = 5;
const std::string SampleData= "sampleData.txt";



//generates n Numbers in SampleData file in range min - max
bool createSampleData(const double& min, const double& max,const int& n);



/** displays user Dialog and choices what to do
 * return vals:
 */
int consoleDialog();

//does the sorting stuff and display stuff dunu
void bubbleSort(sf::RenderWindow&);

// sleepSort
void sleepSort(sf::RenderWindow&);

int main() {


    std::vector<int> values = { 300, 200, 400, 100, 500, 600 };


    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Window text");

    // Main loop
    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {

            //Close event
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        // Start the bubble sort and visualization
        bubbleSort(window);
    }

    return 0;
}

void bubbleSort(std::vector<int>& values, sf::RenderWindow& window) {
    int n = values.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (values[j] > values[j + 1]) {
                std::swap(values[j], values[j + 1]);

                // Visualization
                window.clear();
                for (int k = 0; k < n; ++k) {
                    sf::RectangleShape bar(sf::Vector2f(BAR_WIDTH, values[k]));
                    bar.setFillColor(sf::Color::Green);
                    bar.setPosition(sf::Vector2f(k * BAR_WIDTH, WINDOW_HEIGHT - values[k]));
                    window.draw(bar);
                }
                window.display();
                sf::sleep(sf::milliseconds(50)); // Pause for visualization
            }
        }
    }
}

bool createSampleData(const double& min, const double& max, const int& n) {

    std::ofstream outFile(SampleData);
    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return false;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);


    for (int i = 0; i < n; ++i) {
        int randomValue = distrib(gen);
        outFile << randomValue << "\n";
    }
    outFile<<std::endl;

    // Close the file
    outFile.close();
    return true;
}
