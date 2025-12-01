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

int main(int argc, char** argv) {
	constexpr int dial_size{99};

	std::vector<std::string> ir;

	int current_loc{50};
	int sum_of_zeroes{0};

	get_input(argv[1], ir);	
	
	for (auto& i : ir) {
		int gross_distance{std::stoi(i.substr(1, std::string::npos))};
		int net_distance{gross_distance % (dial_size + 1)};

		switch (i[0]) {
			case 'L':
				current_loc = (((dial_size + 1) - net_distance) + current_loc) % (dial_size + 1);
				break;

			case 'R':
				current_loc = (current_loc + net_distance) % (dial_size + 1);
				break;
		}

		if (current_loc == 0) {
			sum_of_zeroes++;
		}
	}

	std::cout << sum_of_zeroes << std::endl;
}
