#include <iostream>
#include <cmath>
#include <random>
#include <vector>

// Function to generate normally distributed random numbers
double generateGaussianNoise(double mean, double stddev) {
    static std::mt19937 generator(std::random_device{}());
    std::normal_distribution<double> distribution(mean, stddev);
    return distribution(generator);
}

// Function to calculate the payoff of a Black-Scholes call option
double callOptionPayoff(double S, double K) {
    return std::max(S - K, 0.0);
}

// Function to calculate the payoff of a Black-Scholes put option
double putOptionPayoff(double S, double K) {
    return std::max(K - S, 0.0);
}

// Monte Carlo simulation for European option pricing
std::pair<double, double> monteCarloOptionPricing(double S0, double K, double r, double sigma, double T, int numSimulations, bool isCallOption) {
    double payoffSum = 0.0;

    for (int i = 0; i < numSimulations; ++i) {
        // Generate a random price path
        double ST = S0 * std::exp((r - 0.5 * sigma * sigma) * T + sigma * std::sqrt(T) * generateGaussianNoise(0.0, 1.0));

        // Calculate the payoff for this path
        double payoff = isCallOption ? callOptionPayoff(ST, K) : putOptionPayoff(ST, K);

        // Accumulate the payoff
        payoffSum += payoff;
    }

    double averagePayoff = payoffSum / static_cast<double>(numSimulations);
    double discountedPayoff = std::exp(-r * T) * averagePayoff;
    return {averagePayoff, discountedPayoff};
}

double calculateTotalProfit(double averagePayoff, double discountedPayoff) {
    return averagePayoff - discountedPayoff;
}
int main() {
    // Option parameters
    double S0 = 100.0;          // Initial stock price
    double K = 100.0;           // Strike price
    double r = 0.05;            // Risk-free rate
    double sigma = 0.2;         // Volatility
    double T = 1;               // Time to maturity (1 year)
    int numSimulations = 100000;  // Number of simulations

    // Calculate option prices
    auto [callAveragePayoff, callDiscountedPayoff] = monteCarloOptionPricing(S0, K, r, sigma, T, numSimulations, true);
    auto [putAveragePayoff, putDiscountedPayoff] = monteCarloOptionPricing(S0, K, r, sigma, T, numSimulations, false);

    // Calculate total profits
    double callTotalProfit = calculateTotalProfit(callAveragePayoff, callDiscountedPayoff);
    double putTotalProfit = calculateTotalProfit(putAveragePayoff, putDiscountedPayoff);

    // Adjust prices based on standard contract size (100 shares)
    double contractMultiplier = 100.0;
    double callPriceContract = callDiscountedPayoff * contractMultiplier;
    double putPriceContract = putDiscountedPayoff * contractMultiplier;
    double callTotalProfitContract = callTotalProfit * contractMultiplier;
    double putTotalProfitContract = putTotalProfit * contractMultiplier;
    // Output the results
    std::cout<< "\n" << std::endl;
    std::cout << "Based on standard contract size (" << int(contractMultiplier) << " shares)" << std::endl;
    std::cout << "European Call Option Price: $" << callPriceContract << std::endl;
    std::cout << "European Put Option Price: $" << putPriceContract << std::endl;
    std::cout << "\n" << std::endl;
    std::cout <<"Call Option Total Profit: $" <<callTotalProfitContract << std::endl;
    std::cout <<"Put Option Total Profit: $" << putTotalProfitContract << std::endl;
    return 0;
}
