#include <fstream>
#include <iostream>
#include <string>
#include <vector>

void get_input(const char* fname, std::vector<std::string>& ir) {
	std::ifstream infile(fname);
	if (infile.is_open()) {
		std::string line;
		while (std::getline(infile, line)) {
			ir.push_back(line);
		}
	}
}

// Integer division 

int main(int argc, char** argv) {
	constexpr int dial_size{100};

	std::vector<std::string> ir;

	int current_loc{50};
	int sum_of_zeroes{0};

	get_input(argv[1], ir);	
	
	for (auto& i : ir) {
		int gross_distance{std::stoi(i.substr(1, std::string::npos))};
		int net_distance{gross_distance % (dial_size)};

		sum_of_zeroes += (gross_distance / (dial_size));

		switch (i[0]) {
			case 'L':
				if (current_loc == 0) {
					current_loc = 100;
				}
				if ((current_loc - net_distance) <= 0) {
					sum_of_zeroes++;
				}

				current_loc = (((dial_size) - net_distance) + current_loc) % (dial_size);
				break;

			case 'R':
				if ((current_loc + net_distance) >= (dial_size)) {
					sum_of_zeroes++;
				}

				current_loc = (current_loc + net_distance) % (dial_size);
				break;
		}
	}

	std::cout << sum_of_zeroes << std::endl;
}
