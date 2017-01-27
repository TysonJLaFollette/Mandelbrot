/*Serial mandelbrot generator by Tyson J. LaFollette A00987957
CS3100 section 001, fall 2017 */
#include <iostream>
#include <fstream>
//#include <functional>
#include<vector>
#include <chrono>
#include<numeric>
#include<cmath>
#include <thread>

struct Color {
        int red;
        int green;
        int blue;
    };

void plot(std::vector< std::vector<Color> > &renderArray, int x, int y, Color color) {
    renderArray.at(y).at(x) = color;
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
	    for (int j=0; j<x2-x1;j++) {
            Color color;
            newRow.push_back(color);
        }
        renderArray.push_back(newRow);
	}
}

void threadedGeneratePartial(std::vector<std::vector<Color>> &renderArray, double scale, int maxIterations, int x1, int y1, int x2, int y2){
    for (int i = y1; i <= y2; i++) {
        for (int j = x1; j<x2; j++) {
            Color pixelColor = getPixelColor(j,i,scale,maxIterations);
            plot(renderArray, j, i, pixelColor);
        }
    }
}
std::vector<std::vector<Color>> threadedFourGenerateImage(double scale, int maxIterations, int x1, int y1, int x2, int y2){
    //divide image into numthreads parts vertically.
    int partitionHeight = (y2-y1)/4;
    std::vector<std::vector<Color>> renderArray;
    prepBuffer(renderArray,scale,x1,y1,x2,y2);
    std::thread t0(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1,x2,partitionHeight-1);
    std::thread t1(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight,x2,partitionHeight*2-1);
    std::thread t2(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*2, x2,partitionHeight*3-1);
    std::thread t3(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*3,x2,partitionHeight*4-1);
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    return renderArray;
}
std::vector<std::vector<Color>> threadedEightGenerateImage(double scale, int maxIterations, int x1, int y1, int x2, int y2){
    //divide image into numthreads parts vertically.
    int partitionHeight = (y2-y1)/8;
    std::vector<std::vector<Color>> renderArray;
    prepBuffer(renderArray,scale,x1,y1,x2,y2);
    std::thread t0(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1,x2,partitionHeight-1);
    std::thread t1(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight,x2,partitionHeight*2-1);
    std::thread t2(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*2,x2,partitionHeight*3-1);
    std::thread t3(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*3,x2,partitionHeight*4-1);
    std::thread t4(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*4,x2,partitionHeight*5-1);
    std::thread t5(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*5,x2,partitionHeight*6-1);
    std::thread t6(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*6,x2,partitionHeight*7-1);
    std::thread t7(threadedGeneratePartial,std::ref(renderArray),scale,maxIterations,x1,y1 + partitionHeight*7,x2,partitionHeight*8-1);
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    return renderArray;
}
std::vector<std::vector<Color>> generateImage(double scale, int maxIterations, int x1, int y1, int x2, int y2) {
    std::vector<std::vector<Color>> renderArray;
    prepBuffer(renderArray, scale, x1, y1, x2, y2);
    //loop through the given space, checking each pixel's divergence.
    for (int i = y1; i < y2; i++){
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
    std::cout << timestorun << " iterations. ";
    for(int i = 0; i < runtimes.size(); i++) {
		std::cout << runtimes.at(i) << "ms ";
	}
	std::cout << "\nAverage: " << mean << "ms. Standard Deviation: " << standardDeviation << "ms.\n";
}

void testFunction(){
    generateImage(200,255,0,0,700,400);
}
void threadedFourTestFunction(){
	threadedFourGenerateImage(200,255,0,0,700,400);
}
void threadedEightTestFunction(){
	threadedEightGenerateImage(200,255,0,0,700,400);
}
int main() {
    //Algorithm designed after pseudocode from wikipedia.
    std::cout << "Running Serial algorith tests...\n";
    averageAndDeviationOfFunction([=](){testFunction();},10);
    std::cout << "Running 4 thread algorithm tests...\n";
    averageAndDeviationOfFunction([=](){threadedFourTestFunction();},10);
    std::cout << "Running 8 thread algorithm tests...\n";
    averageAndDeviationOfFunction([=](){threadedEightTestFunction();},10);
    writeFile(threadedFourGenerateImage(200,255,0,0,700,400));
    //writeFile(generateImage(200,255,0,0,700,400));
    std::cout << "See mandelbrot.ppm for image.\n";
}
