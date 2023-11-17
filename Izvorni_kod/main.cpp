#include <random>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;
using namespace chrono;

int bottom_up_solution(const std::vector<int>& prices, int N)
{
    std::vector<int> partial_solutions;
    partial_solutions.push_back(0);
    for (size_t i = 1; i <= N; i++)
    {
        auto i_solution = -1;
        for (size_t j = 0; j < i; j++)
            i_solution = max(i_solution, partial_solutions[j] + prices[i - j]);
        partial_solutions.push_back(i_solution);
    }

    return partial_solutions[N];
}

int memoized_recursion(const std::vector<int>& prices, int n, std::vector<int>& partial_solutions)
{
    if (partial_solutions[n] == -1)
    {
        auto n_solution = -1;
        for (size_t i = 1; i <= n; i++)
            n_solution = max(n_solution, memoized_recursion(prices, n - i, partial_solutions) + prices[i]);
        partial_solutions[n] = n_solution;
    }

    return partial_solutions[n];
}

int recursion(const std::vector<int>& prices, int n)
{
    if (n == 0)
        return 0;

    auto n_solution = -1;
    for (size_t i = 1; i <= n; i++)
        n_solution = max(n_solution, recursion(prices, n - i) + prices[i]);

    return n_solution;
}

int main()
{
    vector<string> functionNames;
//    functionNames.push_back("recursion");
    functionNames.push_back("memoized");
    functionNames.push_back("bottom_up");
    size_t N_range_start = 10000;
    size_t N_range_end = 12000;
    size_t N_step = 100;// (N_range_end - N_range_start + 1) / 10;
    int repetitions = 3;

    random_device seed;
    mt19937 generator(seed());

    decltype(high_resolution_clock::now()) start;
    decltype(high_resolution_clock::now()) stop;
    decltype((duration_cast<milliseconds>(stop - start)).count()) execution_time;
    double average_execution_time;

    vector<int> prices;

    ofstream output_file("../Rezultati_mjerenja/"
        + to_string(N_range_start) + "_"
        + to_string(N_range_end) + "_"
        + to_string(N_step) + "_"
        + to_string(repetitions) + ".txt");
    if (!output_file.is_open())
    {
        cerr << "Output file couldn't be opened." << endl;
        return 1;
    }

    for (size_t N = N_range_start; N <= N_range_end; N += N_step)
    {
        cout << N << endl;
        output_file << setw(15) << setfill(' ') << N;
        uniform_int_distribution<mt19937::result_type> distribution(1, N * 1.5);
        prices.clear();
        prices.push_back(0);
        for (size_t i = 1; i <= N; i++)
            prices.push_back(distribution(generator));
        sort(prices.begin(), prices.end());

        for (const string& functionName : functionNames)
        {
            average_execution_time = 0;

            for (int i = 0; i < repetitions; i++)
            {
                if (functionName == "recursion")
                {
                    start = high_resolution_clock::now();
                    recursion(prices, N);
                    stop = high_resolution_clock::now();

                }
                else if (functionName == "memoized")
                {
                    start = high_resolution_clock::now();
                    vector<int> partial_solutions(N + 1, -1);
                    partial_solutions[0] = 0;
                    memoized_recursion(prices, N, partial_solutions);
                    stop = high_resolution_clock::now();
                }
                else if (functionName == "bottom_up")
                {
                    start = high_resolution_clock::now();
                    bottom_up_solution(prices, N);
                    stop = high_resolution_clock::now();
                }

                execution_time = duration_cast<milliseconds>(stop - start).count();
                average_execution_time += execution_time;
            }
            average_execution_time /= repetitions;

            output_file << setw(15) << setfill(' ') << average_execution_time;
        }

        output_file << endl;
    }

    output_file.close();

    return 0;
}
