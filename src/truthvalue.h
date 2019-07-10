#pragma once

class TruthValue {
	enum class Values { X = -1, FALSE = 0, TRUE = 1 };

  public:
	static constexpr Values X = Values::X;
	static constexpr Values TRUE = Values::TRUE;
	static constexpr Values FALSE = Values::FALSE;

  private:
	Values value;

  public:
	TruthValue() : value(X){};
	TruthValue(bool src) : value(src ? TRUE : FALSE){};
	TruthValue(Values src) : value(src) {}

	// This is a "programmer's equality", it does NOT propagate X values unlike
	// the "==" operator in Verilog.
	bool operator==(TruthValue a) const;

	// These could very well be implemented in gates.cpp for the sake of this
	// assignment, but implementing custom operators allow a developer to write
	// arbitrarily complex Boolean expressions in eg. more complex gates.
	// Also, note that the implementation matches the behaviour of Verilog with
	// respect to X values.
	TruthValue operator&&(TruthValue a) const;
	TruthValue operator||(TruthValue a) const;
	TruthValue operator^(TruthValue a) const;
	TruthValue operator!() const;

	char toChar() const;
};
