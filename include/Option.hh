#pragma once

#include <array>
#include <span>
#include <string_view>
#include <vector>


enum position {
	NoPosition,
	End,
};

enum OptionType {
	Command,
	Opt,
	Positional,
};

class BasicOption {
	public:
		std::string_view Desc;
		std::string_view Name;
		bool Initialized;
		enum OptionType Type;
		BasicOption(const char *Name, const char *Description,
			    enum OptionType Type, enum position pos);
		virtual void addArgument(const std::string_view Arg) {};
		virtual void isPresent(void) { Initialized = true; };
		virtual bool hasArg(void) = 0;
		virtual void toHelpRepr(void);
		operator bool() const { return Initialized; };
};

template <class T>
class Option;

struct Alias {
};

template<>
class Option<Alias> : public BasicOption {
	private:
		BasicOption *Alias;
	public:
		Option(const char *Name, const char *Description, BasicOption *alias);
		Option(const Option &) = delete;
		void isPresent(void);
		bool hasArg(void) { return Alias->hasArg(); };
		void addArgument(const std::string_view Arg) { Alias->addArgument(Arg); };
};

template <>
class Option<bool> : public BasicOption {
	public:
		Option(const char *Name, const char *Description,
		       enum OptionType Type, enum position pos = NoPosition);
		Option(const Option &) = delete;
		bool hasArg(void) { return false; };
};

template <>
class Option<std::string> : public BasicOption {
	private:
		std::string_view Value;
	public:
		Option(const char *Name, const char *Description,
		       enum OptionType Type, enum position pos = NoPosition);
		void addArgument(const std::string_view Arg);
		bool hasArg(void);
		std::string_view getValue(void);
		void toHelpRepr(void);
};

template <>
class Option<int> : public BasicOption {
	private:
		int Value;
		bool isSet;
	public:
		Option(const char *Name, const char *Description,
		       enum OptionType Type, enum position pos = NoPosition);
		void addArgument(const std::string_view Arg);
		bool hasArg(void);
		int getValue(void);
		void toHelpRepr(void);
};

class CommandLineParser {
	private:
		std::vector<BasicOption *> Commands;
		BasicOption *endPosArg;
		BasicOption *getMatchingOption(std::string_view OptName);
		void handleError(std::string_view Arg);
		std::string_view ProgName;
	public:
		void addOption(BasicOption *O, enum position pos);
		void parseCommandLine(std::span<const char *> args);
		CommandLineParser() noexcept;
		void generateHelp(void);
		void setProgramName(std::string_view s) { ProgName = s; };
};

void CLParse(std::span<const char *> args, const char *ProgName);
void help(void);
