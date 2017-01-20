/*Serial mandelbrot generator by Tyson J. LaFollette A00987957
CS3100 section 001, fall 2017 */
#include <iostream>
#include <fstream>
#include <functional>
#include<vector>
#include <chrono>
#include<numeric>
#include<cmath>

struct Color {
        int red;
        int green;
        int blue;
    };

void plot(std::vector< std::vector<Color> > &renderArray, int x, int y, Color color) {
    renderArray.at(y).push_back(color);
}

Color getPixelColor(int pixelx, int pixely, double scale, int maxIterations){
    //use linear interpolation to find the correct complex number.
    double x0 = ((pixelx/scale) - 2.5); //scaled x coordinate of pixel(scaled to lie in the Mandelbrot X scale(-2.5, 1))
    double y0 = ((pixely/scale) - 1); //scaled y coordinate of pixel(scaled to lie in the Mandelbrot Y scale(-1, 1))
    double x = 0.0;
    double y = 0.0;
    int iteration = 0;
    while (x*x + y*y < 2 * 2  &&  iteration < maxIterations) {
        double xtemp = x*x - y*y + x0;
        y = 2 * x*y + y0;
        x = xtemp;
        iteration = iteration + 1;
    }
    Color color;
    color.red = color.green = color.blue = std::log(iteration)*255/std::log(maxIterations);
    return color;
}
void prepBuffer(std::vector< std::vector<Color> > &renderArray, double scale, int x1, int y1, int x2, int y2){
    renderArray.reserve(y2-y1);
	for (int i = 0; i < y2-y1; i++) {
	    std::vector<Color> newRow;
	    newRow.reserve(x2-x1);
        renderArray.push_back(newRow);
	}
}

std::vector<std::vector<Color>> generateImage(double scale, int maxIterations, int x1, int y1, int x2, int y2) {
    std::vector<std::vector<Color>> renderArray;
    prepBuffer(renderArray, scale, x1, y1, x2, y2);
    //loop through the given space, checking each pixel's divergence.
    for (int i = y1; i < y2; i++){
        if (i%50 == 0){std::cout << "Rendering row: " << i << "\t\t\r";}
        for (int j = x1; j < x2; j++){
            Color pixelColor = getPixelColor(j, i, scale, maxIterations);
            plot(renderArray, j-x1, i-y1, pixelColor); //j is current x coordinate. i is current y coordinate.
        }
    }
    return renderArray;
}
void writeFile(std::vector<std::vector<Color>> renderArray) {
    std::cout << "Writing file!";
    std::ofstream fout("mandelbrot.ppm");
    if (fout.is_open()) {
        fout << "P3\n" << renderArray.at(0).size() << " " << renderArray.size() << " 255" << "\n";
        for (int i = 0; (unsigned)i < renderArray.size(); i++) {
            if (i%50 == 0){std::cout << "Writing row: " << i << "\t\t\r";}
            for (int j = 0; (unsigned)j < renderArray.at(0).size(); j++) {
                fout << renderArray.at(i).at(j).red << " " << renderArray.at(i).at(j).green << " " << renderArray.at(i).at(j).blue << " ";
            }
            fout << "\n";
        }
        fout.close();
        return;
    }
}

template < typename F>
auto timeFunction(F functiontotime){
    auto starttime = std::chrono::steady_clock::now();
    functiontotime();
    auto endtime = std::chrono::steady_clock::now();
    auto timetaken = endtime - starttime;
    return std::chrono::duration_cast<std::chrono::milliseconds>(timetaken);
}

template < typename F>
void averageAndDeviationOfFunction(F functiontotime, int timestorun) {
    auto mean = 0.0;
    auto numeratorSum = 0.0;
    std::vector<double> runtimes;
    for (int i = 0; i < timestorun; i++) {
        runtimes.push_back(timeFunction([=](){functiontotime();}).count());
    }
    mean = std::accumulate(runtimes.begin(), runtimes.end(), 0)/(double)timestorun;
    for (int i = 0; i < timestorun; i++) {
        numeratorSum += std::pow(runtimes.at(i) - mean,2);
    }
    auto standardDeviation = std::sqrt((numeratorSum)/timestorun);
    std::cout << timestorun << " iterations. Average: " << mean << "ms. Standard Deviation: " << standardDeviation << "ms.\n";
}

void testFunction(){
    generateImage(200,255,0,0,700,400);
}
int main() {
    //Algorithm designed after pseudocode from wikipedia.
    averageAndDeviationOfFunction([=](){testFunction();},10);
    std::cout << "See mandelbrot.ppm for image.\n";
}
