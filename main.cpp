#include <fstream>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <thread>
#include <optional>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <unistd.h>

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

// sorting algos
template <typename T>
void bubbleSort(sf::RenderWindow*,std::vector<T>&);
template <typename T>
void bongoSort(sf::RenderWindow*,std::vector<T>&);
template <typename T>
void quickSort(sf::RenderWindow*,std::vector<T>&);


/** displays user Dialog and choices what to do
 * return vals:
 */
int mainConsoleDialog(sf::RenderWindow*);

// sub Dialog of mainConsoleDialog
// Displays Dialog for selection and execution of single alorithems
int algoConsoleDialog(sf::RenderWindow*);

int settingsDialog();

//generates n Numbers in SampleData file in range min - max
bool createSampleData(const double& min, const double& max,const int& n);

//load Samples from file to local vector
void loadSamples();

//renders sample vector als diagramm
template <typename Iterator>
void renderSample(sf::RenderWindow*,Iterator begin, Iterator end);


int main() {

    loadSamples();
    sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Cooles Fenster für sachen");

    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

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
/*
 *optional parameter input if null global sample is used
 **/

template <typename Iterator>
void renderSample(sf::RenderWindow* window, Iterator begin, Iterator end) {
    (void)window->setActive(true);
    if (begin == end ) {return;}

    window->clear(backgroundColor);

    const int maxHeight = window->getSize().y - PADDINGBOT - PADDINGTOP;
    const int maxValue = *(std::max_element(begin, end));
    const float scalingFactor = maxHeight / maxValue;
    const float distance = BAR_WIDTH + SPACING;

    for (size_t i = 0; i < std::distance(begin, end); ++i) {
        float height = scalingFactor * *(begin + i);

        sf::RectangleShape bar(sf::Vector2f(BAR_WIDTH, height));
        bar.setPosition({i * distance, WINDOW_HEIGHT - height - PADDINGBOT});
        bar.setFillColor(barColor);
        bar.setOutlineThickness(0.05);
        bar.setOutlineColor(outlineColor);
        window->draw(bar);
    }

    window->display();
    (void)window->setActive(false);
}

template <typename T>
void bubbleSort(sf::RenderWindow* window,std::vector<T>& data) {

    const auto start = std::chrono::high_resolution_clock::now();
    const unsigned int n = data.size();

    for (int i = 0; i < n - 1; i++) {
            bool swapped = false;
            for (int j = 0; j < n - i - 1; j++) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                    swapped = true;
                }
                //render the change
                renderSample(window,data.begin(),data.end());
                if (RENDERDELAY >0) {
                    sf::sleep(sf::milliseconds(RENDERDELAY));
                }
            }
        // If no two elements were swapped, then break
        if (!swapped)break;
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> elapsed = end - start;

    std::cout << "Bubble Sort took " << elapsed.count() << " seconds." << std::endl;
}

template <typename  T>
void bongoSort(sf::RenderWindow* window,std::vector<T>& vec){
    while (!std::is_sorted(vec.begin(), vec.end())){
        std::shuffle(samples.begin(),samples.end(),std::default_random_engine(0));
        renderSample(window,samples.begin(),samples.end());
    }
}


template <typename T>
T partition(std::vector<T>& vec, T low, T high,sf::RenderWindow* window) {
    int pivot = vec[high];

    int i = low-1;

    for (int j = low; j <= high-1; j++) {
        if (vec[j] <= pivot) {
            i++;
            std::swap(vec[j], vec[i]);
            renderSample(window,samples.begin(),samples.end());
        }
    }
    std::swap(vec[i+1], vec[high]);
    renderSample(window,samples.begin(),samples.end());
    return i+1;
}

template <typename  T>
void quickSortrecursiv(sf::RenderWindow* window,std::vector<T>& vec,T low, T high) {
    if (low < high) {
        int pivot = partition(vec, low, high,window);

        quickSortrecursiv(window, vec, low, pivot-1);
        quickSortrecursiv(window, vec, pivot+1, high);
    }
}

template <typename  T>
void quickSort(sf::RenderWindow* window,std::vector<T>& vec) {
    quickSortrecursiv(window,vec,0,static_cast<int>(vec.size()-1));
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
                renderSample(window,samples.begin(), samples.end());
            break;
            }
            //generate spezific Algo
            case 2:
                algoConsoleDialog(window);
            break;
            //generate "all" Algos
            case 3: {
                //TODO implementaion
                //n = anzahl zu zeigender algos
                //jedem algo einen bereich zuweisen von x,y koordinaten(einafach alle auf selbe y macht das einfacher) jeder gleiche größe; dann view dazu erstellen
                //viewports erstellen gleiche größe possitionierung abhängig von fenster größe
                //render funktion den offset mitgeben algo 1 = 0 offset; algo 2 500 offset(bei bereichsgröße von 500)
                //1 offset für x achse am einfachsten sondst 2 für x und y wenn nötig

                //kurz jeder algo wir auf einem bestimmten bereich gerendert; jeder viewport zeigt dann auf einen bereich

               // !! wichtig threadsicherheit gewährleisten !! set active beachten
                // sample data muss kopiert werden entweder funktion überladen oder vector parameter ; if vector == null then use sample)

                (void)window->setActive(true);
                sf::View leftView(sf::FloatRect({0, 0}, {500,500}));
                sf::View rightView(sf::FloatRect({500, 0}, {500, 500}));

                leftView.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}));
                rightView.setViewport(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}));
                window->setView(leftView);
                sf::CircleShape circle( 100); // Größe relativ zum Fenster
                circle.setFillColor(sf::Color::Red);
                circle.setPosition({0, 0});
                window->draw(circle);

                window->setView(rightView);
                sf::RectangleShape rectangle(sf::Vector2f(100, 200));
                rectangle.setFillColor(sf::Color::Blue);
                rectangle.setPosition({600, 100});
                window->draw(rectangle);

                window->display();
                break;
            }
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
        "2: Bongo Sort \n"<<
        "3: Random Sort"<<
        "4: quickSort"<<std::endl;
        std::cin >> input;

        switch(input){
            //exit
            case 0:
                return 0;
            break;

            //Bubble sort
            case 1:
                bubbleSort(window,samples);
            break;

            case 2:
                bongoSort(window,samples);
            break;

            case 3:
                bongoSort(window,samples);
            break;
            case 4:
                quickSort(window,samples);
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

