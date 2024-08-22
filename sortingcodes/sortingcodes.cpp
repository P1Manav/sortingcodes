#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

template <typename T>
string toString(T number) {
    ostringstream ss;
    ss << number;
    return ss.str();
}

vector<int> generateRandomData(int size) {
    vector<int> data(size);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10000);
    generate(data.begin(), data.end(), [&]() { return dis(gen); });
    return data;
}

void bubbleSort(vector<int>& arr) {
    for (size_t i = 0; i < arr.size(); i++) {
        for (size_t j = 0; j < arr.size() - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

void selectionSort(vector<int>& arr) {
    for (size_t i = 0; i < arr.size(); i++) {
        size_t minIndex = i;
        for (size_t j = i + 1; j < arr.size(); j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }
        }
        swap(arr[i], arr[minIndex]);
    }
}

void insertionSort(vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

void quickSort(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j] < pivot) {
                i++;
                swap(arr[i], arr[j]);
            }
        }
        swap(arr[i + 1], arr[high]);
        quickSort(arr, low, i);
        quickSort(arr, i + 2, high);
    }
}

template <typename F>
long long measureSortingTime(F sortingFunction, vector<int> data) {
    auto start = chrono::high_resolution_clock::now();
    sortingFunction(data);
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration_cast<chrono::microseconds>(end - start).count();
}

void drawGraph(sf::RenderWindow& window, const vector<pair<int, long long>>& data, const string& title) {
    float width = 1000;
    float height = 600;
    float margin = 80;

    sf::RectangleShape xAxis(sf::Vector2f(width - 2 * margin, 2));
    xAxis.setPosition(margin, height - margin);
    window.draw(xAxis);

    sf::RectangleShape yAxis(sf::Vector2f(2, height - 2 * margin));
    yAxis.setPosition(margin, margin);
    window.draw(yAxis);

    sf::Font font;
    if (!font.loadFromFile("D:/Projects/sortingcodes/Debug/Fonts/Roboto-Black.ttf")) {
        cout << "Error loading font" << endl;
        return;
    }

    int numXTicks = data.size();
    for (int i = 0; i < numXTicks; i++) {
        float x = margin + (i * (width - 2 * margin) / (numXTicks - 1));
        sf::RectangleShape tick(sf::Vector2f(2, 5));
        tick.setPosition(x, height - margin);
        window.draw(tick);

        sf::Text xValue(toString(data[i].first), font, 12);
        xValue.setFillColor(sf::Color::Black);
        xValue.setPosition(x - 10, height - margin + 5);
        window.draw(xValue);
    }

    int numYTicks = 10;
    long long maxYValue = max_element(data.begin(), data.end(), [](const pair<int, long long>& a, const pair<int, long long>& b) {
        return a.second < b.second;
        })->second;
    for (int i = 0; i <= numYTicks; i++) {
        float y = height - margin - (i * (height - 2 * margin) / numYTicks);
        sf::RectangleShape tick(sf::Vector2f(5, 2));
        tick.setPosition(margin - 5, y);
        window.draw(tick);

        long long yValue = (maxYValue / numYTicks) * i;
        sf::Text yValueText(toString(yValue), font, 12);
        yValueText.setFillColor(sf::Color::Black);
        yValueText.setPosition(margin - 70, y - 10);
        window.draw(yValueText);
    }

    sf::VertexArray lines(sf::LineStrip, data.size());
    for (size_t i = 0; i < data.size(); i++) {
        float x = margin + (i * (width - 2 * margin) / (data.size() - 1));
        float y = height - margin - (data[i].second / static_cast<float>(maxYValue) * (height - 2 * margin));

        lines[i].position = sf::Vector2f(x, y);
        lines[i].color = sf::Color::Red;
    }

    window.draw(lines);

    sf::Text xAxisLabel("Input Size", font, 15);
    xAxisLabel.setFillColor(sf::Color::Black);
    xAxisLabel.setPosition(width / 2 - 40, height - margin + 30);
    window.draw(xAxisLabel);

    sf::Text yAxisLabel("Time (µs)", font, 15);
    yAxisLabel.setFillColor(sf::Color::Black);
    yAxisLabel.setRotation(270);
    yAxisLabel.setPosition(margin - 80, height / 2 + 60);
    window.draw(yAxisLabel);

    sf::Text graphTitle(title, font, 20);
    graphTitle.setFillColor(sf::Color::Black);
    graphTitle.setPosition(width / 2 - 100, margin / 2);
    window.draw(graphTitle);
}

int main() {
    vector<int> sizes = { 10, 100, 1000, 10000 };
    vector<pair<int, long long>> bubbleSortTimes;
    vector<pair<int, long long>> selectionSortTimes;
    vector<pair<int, long long>> insertionSortTimes;
    vector<pair<int, long long>> quickSortTimes;

    for (int size : sizes) {
        vector<int> data = generateRandomData(size);

        bubbleSortTimes.emplace_back(size, measureSortingTime(bubbleSort, data));
        selectionSortTimes.emplace_back(size, measureSortingTime(selectionSort, data));
        insertionSortTimes.emplace_back(size, measureSortingTime(insertionSort, data));

        auto quickSortWrapper = [](vector<int>& arr) { quickSort(arr, 0, arr.size() - 1); };
        quickSortTimes.emplace_back(size, measureSortingTime(quickSortWrapper, data));
    }

    sf::RenderWindow window(sf::VideoMode(800, 600), "Sorting Algorithms Comparison");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        drawGraph(window, bubbleSortTimes, "Bubble Sort");
        drawGraph(window, selectionSortTimes, "Selection Sort");
        drawGraph(window, insertionSortTimes, "Insertion Sort");
        drawGraph(window, quickSortTimes, "Quick Sort");

        window.display();
    }

    return 0;
}
