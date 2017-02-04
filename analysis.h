#include <functional>
#include <chrono>
#include<numeric>
#include<cmath>
#include<vector>
#ifndef ANALYSIS
#define ANALYSIS

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

#endif
