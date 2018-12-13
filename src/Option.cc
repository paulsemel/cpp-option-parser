#include <cstdlib>
#include <iostream>
#include "Option.hh"

CommandLineParser  GlobalParser  __attribute__ ((init_priority (2000)));

// BasicOption methods

BasicOption::BasicOption(const char *Name, const char *Description,
			 enum OptionType Type, enum position pos)
	: Name(Name), Desc(Description), Initialized(false), Type(Type) {
	GlobalParser.addOption(this, pos);
}

void BasicOption::toHelpRepr(void) {
	printf("%-20s - %s\n", Name.data(), Desc.data());
}

// Option<Alias>

Option<Alias>::Option(const char *Name, const char *Description, BasicOption *alias)
	: BasicOption(Name, Description, alias->Type, NoPosition), Alias(alias) {}

void Option<Alias>::isPresent(void) {
	BasicOption::isPresent();
	Alias->isPresent();
};

// Option<bool>

Option<bool>::Option(const char *Name, const char *Description,
		     enum OptionType Type, enum position pos)
	: BasicOption(Name, Description, Type, pos) {}

// Option<std::string>

Option<std::string>::Option(const char *Name, const char *Description,
			    enum OptionType Type, enum position pos)
	: BasicOption(Name, Description, Type, pos) {}

void Option<std::string>::addArgument(const std::string_view Arg) {
	Value = Arg.data();
}

bool Option<std::string>::hasArg(void) {
	return Value.empty();
}

std::string_view Option<std::string>::getValue() {
	return Value;
}

void Option<std::string>::toHelpRepr(void) {
	std::string s(Name);
	s += " <value>";
	printf("%-20s - %s\n", s.c_str(), Desc.data());
}

// Option<int>

Option<int>::Option(const char *Name, const char *Description,
			    enum OptionType Type, enum position pos)
	: BasicOption(Name, Description, Type, pos), isSet(false) {}

void Option<int>::addArgument(const std::string_view Arg) {
	char *endPtr = nullptr;
	Value = strtol(Arg.data(), &endPtr, 10);
	// Here we should throw an error if there has been a problem in the
	// number conversion.
	// FIXME: throw an error
	isSet = true;
}

bool Option<int>::hasArg(void) {
	return !isSet;
}

int Option<int>::getValue() {
	return Value;
}

void Option<int>::toHelpRepr(void) {
	std::string s(Name);
	s += " <N>";
	printf("%-20s - %s\n", s.c_str(), Desc.data());
}

// CommandLineParser

BasicOption *CommandLineParser::getMatchingOption(std::string_view OptName) {
	for (BasicOption *O : Commands) {
		if (O->Name == OptName)
			return O;
	}
	return nullptr;
}

void CommandLineParser::handleError(std::string_view Arg) {
	// FIXME: come one, we need to find a proper way to issue the error..
	std::exit(1);
}

void CommandLineParser::addOption(BasicOption *O, enum position pos) {
	if (pos == position::End) {
		if (endPosArg)
			std::exit(1);
		else endPosArg = O;
	} else {
		Commands.push_back(O);
	}
}

void CommandLineParser::parseCommandLine(std::span<const char *> args) {
	args = args.subspan(1);
	for (auto It = args.begin(); It != args.end(); It++) {
		BasicOption *Opt = getMatchingOption(*It);
		if (!Opt) {
			if (It == std::prev(args.end()) && endPosArg) {
				endPosArg->isPresent();
				if (endPosArg->hasArg())
					endPosArg->addArgument(*It);
				continue;
			}
			handleError(*It);
		}
		Opt->isPresent();
		if (Opt->hasArg()) {
			auto J = std::next(It);
			for (; J != args.end() &&
				!getMatchingOption(*J) && Opt->hasArg(); J++)
				Opt->addArgument(*J);

			if (Opt->hasArg())
				handleError("");
			if (J == args.end())
				return;
			It = std::prev(J);
		}
	}
}

CommandLineParser::CommandLineParser() noexcept
	: endPosArg(nullptr) {
}

void CommandLineParser::generateHelp(void) {
	std::cout << ProgName << " [command] [options]";
	if (endPosArg)
		std::cout << " " << endPosArg->Name;
	std::cout << "\n\n";
	std::cout << "Command list :\n";
	for (BasicOption *O : Commands)
		if (O->Type == OptionType::Command)
			O->toHelpRepr();
	std::cout << "\nOption list :\n";
	for (BasicOption *O : Commands)
		if (O->Type == OptionType::Opt)
			O->toHelpRepr();

	if (endPosArg) {
		std::cout << "\nPositional argument :\n";
		printf("%-20s - %s\n", endPosArg->Name.data(),
		       endPosArg->Desc.data());
	}
}

void CLParse(std::span<const char *> args, const char *ProgName) {
	GlobalParser.setProgramName(ProgName);
	GlobalParser.parseCommandLine(args);
}

void help(void) {
	GlobalParser.generateHelp();
}
