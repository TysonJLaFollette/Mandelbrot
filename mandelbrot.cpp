/*Threaded mandelbrot generator by Tyson J. LaFollette A00987957
CS3100 section 001, fall 2017 */
#include <iostream>
#include <fstream>
#include <functional>
#include<vector>
#include <chrono>
#include<numeric>
#include<cmath>
#include <thread>
#include <mutex>

struct Color {
        int red;
        int green;
        int blue;
    };

void plot(std::vector< std::vector<Color> > &renderArray, int x, int y, Color color) {
    renderArray.at(y).at(x) = color;
}

Color getPixelColor(int pixelx, int pixely, double scale){
	int maxIterations = 255;
    //use linear interpolation to find the correct complex number.
    //std::cout << "Interpolating.\n";
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
void prepBuffer(std::vector< std::vector<Color> > &renderArray, int height, int width){
    renderArray.reserve(height);
	for (int i = 0; i < height; i++) {
	    std::vector<Color> newRow;
	    newRow.reserve(width);
	    for (int j=0; j<width;j++) {
            Color color;
            newRow.push_back(color);
        }
        renderArray.push_back(newRow);
	}
}

void generatePartial(std::vector<std::vector<Color>> &renderArray, double scale, int x1, int y1, int x2, int y2){
	//std::cout << "Generating partial.\n";
    for (int i = y1; i < y2; i++) {
        for (int j = x1; j<x2; j++) {
			//std::cout << "Pixel (" << j << "," << i << ").\n";
            Color pixelColor = getPixelColor(j,i,scale);
            //std::cout << "Plotting.\n";
            plot(renderArray, j, i, pixelColor);
        }
    }
}

std::vector<std::vector<Color>> newGenerateImage(int numThreads) {
	double scale = 200;
	int width = scale * 3.5;//Mandelbrot set lies between x = -2.5 and x=1. 3.5 units wide.
	int height = scale * 2;//Also lies between y = -1 and y = 1. 2 units tall.
    std::vector<std::vector<Color>> renderArray;
    prepBuffer(renderArray, height, width);
    
    //std::cout << "Buffer prepared.\n";
    std::thread threads[numThreads];
    int partitionHeight = (height)/numThreads;
    std::cout << partitionHeight << " pixel partitions.\n";
    for(int i = 0; i < numThreads; i++) {
		//std::cout << "Starting partition " << i << ". ";
		int x1 = 0;
		int y1 = i*partitionHeight;
		int x2 = width;
		int y2 = ((i+1)*partitionHeight);
		//std::cout << "Area: (" << x1 << "," << y1 << ") to (" << x2 << "," << y2 << ").\n";
		threads[i] = std::thread(generatePartial,std::ref(renderArray),scale,x1,y1,x2,y2);
	}
	for (int i = 0; i < numThreads; i++) {
		threads[i].join();
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

auto timeFunction(std::function<void(void)> functiontotime){
	auto starttime = std::chrono::steady_clock::now();
	functiontotime();
	auto endtime = std::chrono::steady_clock::now();
	auto timetaken = endtime - starttime;
	return std::chrono::duration_cast<std::chrono::milliseconds>(timetaken);
}

void averageAndDeviationOfFunction(std::function<void(void)> functiontotime, int timestorun) {
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
    std::cout << timestorun << " iterations. ";
    for(unsigned int i = 0; i < runtimes.size(); i++) {
		std::cout << runtimes.at(i) << "ms ";
	}
	std::cout << "\nAverage: " << mean << "ms. Standard Deviation: " << standardDeviation << "ms.\n";
}

void testFunction(){
    newGenerateImage(1);
}
void testFunction2(){
    newGenerateImage(4);
}
int main() {
    //Algorithm designed after pseudocode from wikipedia.
    //std::cout << "Running Serial algorith tests...\n";
    //averageAndDeviationOfFunction([=](){testFunction();},10);
    //std::cout << "Running 4 thread algorithm tests...\n";
    //averageAndDeviationOfFunction([=](){threadedFourTestFunction();},10);
    //std::cout << "Running 8 thread algorithm tests...\n";
    //averageAndDeviationOfFunction([=](){threadedEightTestFunction();},10);
    //writeFile(threadedFourGenerateImage(200,255,0,0,700,400));
    //averageAndDeviationOfFunction([=](){testFunction();},10);
    averageAndDeviationOfFunction([=](){testFunction2();},10);
    //writeFile(newGenerateImage(4));
    //std::cout << "Time taken: " << timeFunction([=](){testFunction2();}).count() << "ms.\n";
    std::cout << "See mandelbrot.ppm for image.\n";
}
