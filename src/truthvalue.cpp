#include "truthvalue.h"

bool TruthValue::operator==(const TruthValue a) const {
	return value == a.value;
}

TruthValue TruthValue::operator&&(const TruthValue a) const {
	switch (value) {
		case FALSE:
			return FALSE;
		case TRUE:
			return a;
		case X:
			return (a.value == FALSE) ? FALSE : X;
	}
}

TruthValue TruthValue::operator||(const TruthValue a) const {
	switch (value) {
		case FALSE:
			return a;
		case TRUE:
			return TRUE;
		case X:
			return (a.value == TRUE) ? TRUE : X;
	}
}

TruthValue TruthValue::operator^(const TruthValue a) const {
	if (value == X || a.value == X)
		return X;
	return bool(value) ^ bool(a.value);
}

TruthValue TruthValue::operator!() const {
	switch (value) {
		case X:
			return X;
		case FALSE:
			return TRUE;
		case TRUE:
			return FALSE;
	}
}

char TruthValue::toChar() const {
	switch (value) {
		case X:
			return 'x';
		case FALSE:
			return '0';
		case TRUE:
			return '1';
	}
}