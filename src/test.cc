#include <iostream>
#include <span>
#include <string_view>
#include "Option.hh"

static Option<bool> Distinct("distinct", "number of distinct queries", OptionType::Command);
static Option<bool> Help("--help", "display the help message", OptionType::Command);
static Option<std::string> Lol("lol", "useless option", OptionType::Opt);
static Option<std::string> File("File", "the file to give", OptionType::Positional, position::End);

void handleOptions(void) {
	if (Help) {
		help();
		std::exit(0);
	}
	if (Distinct) {
		// Do smthg
	}
	if (Lol) {
		std::cout << Lol.getValue() << '\n';
	}
}

int main(int argc, const char *argv[])
{
	std::span<const char *> args(argv, argc);
	CLParse(args, "test");

	handleOptions();

	return 0;
}
