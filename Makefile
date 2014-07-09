
PREFIX := /usr/local

SRC :=					\
	Action.cpp			\
	ActionList.cpp		\
	CDriver.cpp			\
	CPPDriver.cpp		\
	Config.cpp			\
	ConfigList.cpp		\
	Error.cpp			\
	FollowSet.cpp		\
	Grammar.cpp			\
	LanguageDriver.cpp	\
	Parser.cpp			\
	Rule.cpp			\
	RuleTable.cpp		\
	State.cpp			\
	StateTable.cpp		\
	Symbol.cpp			\
	SymbolTable.cpp		\
	Util.cpp			\
	ZDriver.cpp			\
	main.cpp			\
#

HEADERS :=				\
	Action.h			\
	ActionList.h		\
	CDriver.h			\
	CPPDriver.h			\
	Config.h			\
	ConfigList.h		\
	Error.h				\
	FollowSet.h			\
	Grammar.h			\
	LanguageDriver.h	\
	Parser.h			\
	Rule.h				\
	RuleTable.h			\
	State.h				\
	StateTable.h		\
	Symbol.h			\
	SymbolTable.h		\
	Util.h				\
	Version.h			\
	ZDriver.h			\
#

OBJS := $(SRC:.cpp=.o)

CXX := g++
CXXWARNS := all comment inline cast-align switch shadow unused cast-qual conversion format multichar missing-braces parentheses pointer-arith sign-compare return-type overloaded-virtual no-ctor-dtor-privacy non-virtual-dtor pmf-conversions sign-promo write-strings
CXXFLAGS := -Os -pipe -fPIC --std=c++11 $(addprefix -W,$(CXXWARNS))

.PHONY: default install

default: lime

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

lime: $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(OBJS) lime

BIN := $(PREFIX)/bin

install: lime
	install -d $(BIN)
	install -s lime $(BIN)

$(OBJS): $(HEADERS)
