#include "analysis.h"
#include "parser.h"
#include "simulation.h"
#include <fstream>
#include <iostream>

int main(int argc, char **argv) {
	if (argc != 2) {
		std::cerr << "Syntax: " << argv[0] << " <input file>" << std::endl;
		return 1;
	}

	std::string filename = argv[1];
	ast::Module module;
	try {
		// Will be automatically closed because of RAII
		std::ifstream file_str(filename);
		if (file_str.fail()) {
			std::cerr << "Failed to read file." << std::endl;
			return 1;
		}
		FileParser parser(file_str);
		module = parser.finalize();
	} catch (std::string &e) {
		std::cerr << "An error occurred while parsing " + filename + ": " << e << std::endl;
		return 1;
	}

	std::cout << "Please select a mode of operation ([S]imulation/[A]nalysis, default: S): ";
	char choice;
	if (std::cin.peek() == '\n')
		choice = 'S';
	else
		std::cin >> choice;

	switch (choice) {
		case 'S':
		case 's':
			try {
				simulation::run(module);
			} catch (std::string &e) {
				std::cerr << "An error occurred while evaluating the circuit: " + e << std::endl;
				return 1;
			}
			break;
		case 'A':
		case 'a':
			try {
				analysis::run(module);
			} catch (std::string &e) {
				std::cerr << "An error occurred while analyzing the circuit: " + e << std::endl;
				return 1;
			}
			break;
		default:
			std::cout << "Invalid choice." << std::endl;
			return 1;
	}
}
