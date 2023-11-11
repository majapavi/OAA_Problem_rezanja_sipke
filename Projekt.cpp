#include <random>       //random_device, mt19937, uniform_int_distribution
#include <chrono>       //precizno vrijeme: high_resolution_clock 
#include <iostream>     //cout, endl
#include <limits.h>     //INT_MIN
#include <algorithm>    //sort
#include <thread>       //sleep
#include <fstream>      //ofstream
#include <numeric>      //accumulate
#include <string>       //to_string
using namespace std;
using namespace chrono;

int recursion(const vector<int>& prices, int n)
{
//    this_thread::sleep_for(milliseconds(50));
    if (n == 0)
        return 0;

    auto n_solution = -1;
    for (size_t i = 1; i <= n; i++)
        n_solution = max(n_solution, recursion(prices, n - i) + prices[i]);

    return n_solution;
}

int memoized_recursion(const vector<int>& prices, int n, vector<int>& partial_solutions)
{
    if (partial_solutions[n] == -1)
    {
        auto n_solution = -1;
        for (size_t i = 1; i <= n; i++)
            n_solution = max(n_solution, recursion(prices, n - i) + prices[i]);
        partial_solutions[n] = n_solution;
    }

    return partial_solutions[n];
}

int bottom_up_solution(const vector<int>& prices, int N)
{
    vector<int> partial_solutions;
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

int main()
{
    size_t N;
    size_t price_range;
    vector<int> prices;     //bilo bi bolje da su ovo long intovi

    random_device seed;
    mt19937 generator(seed());

    ofstream output_file("Results.txt");
    if (!output_file.is_open()) {
        cerr << "Output file couldn't be opened." << endl;
        return 1;
    }

    for (N = 1000; N <= 10000; N += 1)      //101000
    {
        price_range = N * 1.5;
        uniform_int_distribution<mt19937::result_type> distribution(1, price_range);

        decltype(high_resolution_clock::now()) start;
        decltype(high_resolution_clock::now()) stop;
        decltype((duration_cast<nanoseconds>(stop - start)).count()) duration;

        vector<int> durations_recursion;
        vector<int> durations_memoized;
        vector<int> durations_bottom_up;

        double average_recursion;
        double average_memoized;
        double average_bottom_up;

        // u prices je vrijednost stapa duljine i, za indeks i (ide od 0 do N+1)
        //stap duljine 0 ima cijenu 0, ostali imaju rastuce nenegativne cijene

        for (int repetition = 0; repetition < 100; repetition++)
        {
            prices.clear();
            prices.push_back(0);
            for (auto i = 1; i <= N; i++)
                prices.push_back(distribution(generator));
            sort(prices.begin(), prices.end());

            start = high_resolution_clock::now();
            recursion(prices, N);
            stop = high_resolution_clock::now();
            durations_recursion.push_back( duration_cast<milliseconds>(stop - start).count() );

            start = high_resolution_clock::now();
            vector<int> partial_solutions(N + 1, -1);       //ovo mozda izvan petlje deklarirat
            partial_solutions[0] = 0;
            memoized_recursion(prices, N, partial_solutions);
            stop = high_resolution_clock::now();
            durations_memoized.push_back(duration_cast<milliseconds>(stop - start).count());

            start = high_resolution_clock::now();
            bottom_up_solution(prices, N);
            stop = high_resolution_clock::now();
            durations_bottom_up.push_back(duration_cast<milliseconds>(stop - start).count());
        }


        ofstream recursionFile("recursion_" + to_string(N) + ".txt");
        for (const double& duration : durations_recursion) {
            recursionFile << duration << endl;
        }
        recursionFile.close();

        ofstream memoizedFile("memoized_" + to_string(N) + ".txt");
        for (const double& duration : durations_memoized) {
            memoizedFile << duration << endl;
        }
        memoizedFile.close();

        ofstream bottomUpFile("bottom_up_" + to_string(N) + ".txt");
        for (const double& duration : durations_bottom_up) {
            bottomUpFile << duration << endl;
        }
        bottomUpFile.close();


        average_recursion = static_cast<double>(accumulate(durations_recursion.begin(), durations_recursion.end(), 0)) / durations_recursion.size();
        average_memoized = static_cast<double>(accumulate(durations_memoized.begin(), durations_memoized.end(), 0)) / durations_memoized.size();
        average_bottom_up = static_cast<double>(accumulate(durations_bottom_up.begin(), durations_bottom_up.end(), 0)) / durations_bottom_up.size();

        output_file << N << ": " << average_recursion << " " << average_memoized << " " << average_bottom_up << endl;
    }
    output_file.close();

    return 0;
}