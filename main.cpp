#include <iostream>
#include <gmpxx.h>
#include <vector>
#include <algorithm>
#include <thread>
#include <future>

std::vector<int> seed_prime(const int max) {
	std::vector<int> v = {2, 3};
	int p = 5;
	while (p < max) {
		v.push_back(p);
		for (int i = 0; i < v.size()-1; ++i) {
			if ((p % v[i]) == 0) {
				v.pop_back();
				break;
			}
		}
		p+=2;
	}
	return v;
}

bool lucas_lehmer_test(const int p) {
	if (p == 2) {
		return true;
	}
	mpz_class s = 4;
	mpz_class mer = ((mpz_class)1 << p)-1;
	for (int i = 0; i < p-2; ++i) {
		s = s*s-2;
		while (s > mer) {
			s = (s & mer)+(s>>p);
		}
		if (s == mer) {
			s = 0;
		}
		// s%=mer;
	}
	return s == 0;
}

void node(const std::vector<int>& seeds, const int idx, std::vector<int>& result, const int number_of_threads) {
	for (int i = idx; i < seeds.size(); i+=number_of_threads) {
		if (lucas_lehmer_test(seeds[i])) {
			result.push_back(seeds[i]);
		}
	}
}

int node_for_async(const std::vector<int>& seeds, const int idx, std::vector<int>& result, const int number_of_threads) {
	for (int i = idx; i < seeds.size(); i+=number_of_threads) {
		if (lucas_lehmer_test(seeds[i])) {
			result.push_back(seeds[i]);
		}
	}
	return result.size();
}

void multi_thread() {
	const int number_of_threads = 4;
	std::vector<int> seeds = seed_prime(10000);
	std::vector<std::vector<int> > v(number_of_threads);
	std::vector<std::thread> threads;
	for (int i = 0; i < number_of_threads; ++i) {
		threads.push_back(std::thread([seeds, i, &v]{node(seeds, i, std::ref(v[i]), number_of_threads);}));
	}
	for (std::thread &th: threads) {
		th.join();
	}
	std::vector<int> result;
	for (std::vector<int> &vec: v) {
		result.insert(result.end(), vec.begin(), vec.end());
	}
	std::sort(result.begin(), result.end());
	for (int &x: result) {
		std::cout << x << '\n';
	}
}

void multi_async() {
	const int number_of_threads = 4;
	std::vector<int> seeds = seed_prime(10000);
	std::vector<std::vector<int> > each_result(number_of_threads);
	std::vector<std::future<int> > threads;
	for (int i = 0; i < number_of_threads; i++) {
		threads.push_back(std::async(std::launch::async, [seeds, i, &each_result]{return node_for_async(seeds, i, each_result[i], number_of_threads);}));
	}
	for (size_t i = 0; i < threads.size(); i++) {
		threads[i].get();
	}
	std::vector<int> result;
	for (std::vector<int> &vec: each_result) {
		result.insert(result.end(), vec.begin(), vec.end());
	}
	std::sort(result.begin(), result.end());
	for (int &x: result) {
		std::cout << x << '\n';
	}
}

void single() {
	std::vector<int> seeds = seed_prime(10000);
	std::vector<int> result;
	node(seeds, 0, std::ref(result), 1);
	for (int &x: result) {
		std::cout << x <<'\n';
	}
}

int main(int argc, char const *argv[]) {
	// single();
	multi_async();
	// multi_thread();
	return 0;
}
