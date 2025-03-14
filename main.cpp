#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <thread>
#include <optional>
#include <algorithm>

//TODO scaling fix
const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 900;
//TODO MAybe dynamic
int BAR_WIDTH = 5;
int SPACING = 0;
int PADDINGTOP = 50;
int PADDINGBOT = 20;

sf::Color barColor = sf::Color::Red;
sf::Color outlineColor = sf::Color::White;
sf::Color backgroundColor = sf::Color::Black;

int RENDERDELAY = 0;

const std::string SampleDataFile= "sampleData.txt"; //filename for input data

/*Data input for the algorithems
  !!only!! gets changed when new SampleData is created
  does not get changed by algorhithems working on it as input
  has to reflect the content of SampleDataFile
*/
std::vector<int> samples;


//does the sorting stuff and display stuff dunu
void bubbleSort(sf::RenderWindow*);

// sleepSort
void sleepSort(sf::RenderWindow*);

void randomSort(sf::RenderWindow*);
//generates n Numbers in SampleData file in range min - max
bool createSampleData(const double& min, const double& max,const int& n);

/** displays user Dialog and choices what to do
 * return vals:
 */
int mainConsoleDialog(sf::RenderWindow*);

// sub Dialog of mainConsoleDialog
// Displays Dialog for selection and execution of single alorithems
int algoConsoleDialog(sf::RenderWindow*);

int settingsDialog();

//load Samples from file to local vector
void loadSamples();

//renders sample vector als diagramm
void renderSample(sf::RenderWindow* window);


/**
 * wilde idee
 * listener (observer Pattern) aus dem samples vektor machen (wrappen)
 *
 */
int main() {

    loadSamples();
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Cooles Fenster für sachen");
    // deactivate its OpenGL context
    window.display();
    (void)window.setActive(false);

    std::thread consoleThread(&mainConsoleDialog, &window);

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()){
                window.close();
                return 0;
            }
        }
    }

    consoleThread.join();
    return 0;
}

//TODO Schoener machen
void renderSample(sf::RenderWindow* window) {
    if (samples.empty()) return;
    //window->setActive(true);
    window->clear(backgroundColor);

    //max height we can display on
    const int maxHeight = window->getSize().y - PADDINGBOT - PADDINGTOP;
    //highest Value in sample
    const int maxValue = *std::max_element(samples.begin(), samples.end());
    //scaling for height display
    const float scalingFactor = static_cast<float>(maxHeight) / maxValue;
    //margin between bars
    const float distance = BAR_WIDTH + SPACING;

    float height = 0;
    for (size_t i = 0; i < samples.size(); ++i) {
        height = scalingFactor * samples[i];

        //defines a BAR_WIDTH x height Rectangle
        sf::RectangleShape bar(sf::Vector2f(BAR_WIDTH, height));
        //positions rectangel at
        bar.setPosition({i * distance,   WINDOW_HEIGHT -height -PADDINGBOT});
        bar.setFillColor(barColor);
        bar.setOutlineThickness(0.05);
        bar.setOutlineColor(outlineColor);
        window->draw(bar);
    }

    window->display();
}

void bubbleSort(sf::RenderWindow* window) {

    auto start = std::chrono::high_resolution_clock::now();
    int n = samples.size();
    bool swapped;

    for (int i = 0; i < n - 1; i++) {
            swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                if (samples[j] > samples[j + 1]) {
                    std::swap(samples[j], samples[j + 1]);
                    swapped = true;
                }
                //render the change
                renderSample(window);
                if (RENDERDELAY >0) {
                    sf::sleep(sf::milliseconds(RENDERDELAY));
                }
            }
        // If no two elements were swapped, then break
        if (!swapped)break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Bubble Sort took " << elapsed.count() << " seconds." << std::endl;
}

void sleepSort(sf::RenderWindow*) {
    return;
}

void randomSort(sf::RenderWindow*){
    return;
}

bool createSampleData(const double& min, const double& max, const int& n) {

    std::ofstream outFile(SampleDataFile);

    if (!outFile) {
        std::cout  << "Error: Unable to open file for writing." << std::endl;
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
    std::cout <<"successfuly wrote to "<<SampleDataFile<<std::endl;
    return true;
}

int mainConsoleDialog(sf::RenderWindow* window) {
    int input = -1;

    while(true){
        input = -1;
        std::cout << " what do you want to do? \n" <<
        "0: exit\n" <<
        "1: Generate new Sample Data\n"<<
        "2: Animate spezific Algorithem\n"<<
        "3: Animate everything\n"<<
        "9: Settings\n"<<std::endl;
        std::cin >> input;

        switch(input){
            //exit
            case 0:
                return 0;
            break;

            // generate Numbers
            case 1:
            {
                int min = 0;
                int max = 0;
                int n = 0;
                std::cout <<"min number:" << std::endl;
                std::cin >> min;
                std::cout <<"max number:" << std::endl;
                std::cin >> max;
                std::cout <<"how many numbers:" << std::endl;
                std::cin >> n;

                // create Data load into local array and render changes
                createSampleData(min,max,n);
                loadSamples();
                renderSample(window);
            break;
            }
            //generate spezific Algo
            case 2:
                algoConsoleDialog(window);
            break;
            //generate "all" Algos
            case 3:
                // TODO run all maybe dosnt work will see
                    std::cout << "nö"<<std::endl;
            //settings
            case 9:
                settingsDialog();
            break;
            default:
                std::cout << "Digga was soll das mach doch einfach vernünftig"<<std::endl;
        }
    }
}

int algoConsoleDialog(sf::RenderWindow* window){
    int input = -1;

    while(true){
        input = -1;
        std::cout << " what do you want to do? \n" <<
        "0: back\n" <<
        "1: Bubble Sort\n"<<
        "2: Sleep Sort \n"<<
        "3: Random Sort"<<std::endl;

        std::cin >> input;

        switch(input){
            //exit
            case 0:
                return 0;
            break;

            //Bubble sort
            case 1:
                bubbleSort(window);
            break;
            
            case 2:
                sleepSort(window);
            break;

            case 3:
                randomSort(window);
            break;

            default:
                std::cout << "Digga was soll das mach doch einfach vernünftig"<<std::endl;
        }       
    }
}

//TODO error handling
//TODO input controll
int settingsDialog() {
    int input = -1;

    while (true) {
        input = -1;
        std::cout << "What do you want to do?\n"
                  << "0: Back\n"
                  << "1: Change Background Color\n"
                  << "2: Change Bar Color\n"
                  << "3: Change Bar-Outline Color\n"
                  << "4: Change Padding Top\n"
                  << "5: Change Padding Bottom\n"
                  << "6: Change Bar Width\n"
                  << "7: Change Spacing\n"
                  << "8: Change render Delay time (in ms)"
                  << std::endl;


        std::cin >> input;
        switch (input) {
            case 0:
                return 0;
            case 1:
                std::cout << "Enter RGB values for Background Color: ";
                std::cin >> backgroundColor.r >> backgroundColor.g >> backgroundColor.b;
            break;
            case 2:
                std::cout << "Enter RGB values for Bar Color: ";
                std::cin >> barColor.r >> barColor.g >> barColor.b;
            break;
            case 3:
                std::cout << "Enter RGB values for Bar-Outline Color: ";
                std::cin >> outlineColor.r >> outlineColor.g >> outlineColor.b;
            break;
            case 4:
                std::cout << "Enter new Padding Top: ";
                std::cin >> PADDINGTOP;
            break;
            case 5:
                std::cout << "Enter new Padding Bottom: ";
                std::cin >> PADDINGBOT;
            break;
            case 6:
                std::cout << "Enter new Bar Width: ";
                std::cin >> BAR_WIDTH;
            break;
            case 7:
                std::cout << "Enter new Spacing: ";
                std::cin >> SPACING;
            break;
            case 8:
                std::cout << "Enter new Render Delay Time: ";
                std::cin >> RENDERDELAY;
            default:
                std::cout << "Invalid option. Please try again." << std::endl;
        }
    }
}

void loadSamples(){
    std::ifstream inFile(SampleDataFile);

    if (!inFile) {
        std::cerr << "Error: Unable to open file " << SampleDataFile << std::endl;
        return;
    }

    int value;
    samples.clear();

    while (inFile >> value) {
        samples.push_back(value);
    }

    inFile.close();
    std::cout << "Successfully loaded " << samples.size() << " samples from " << SampleDataFile << std::endl;
}