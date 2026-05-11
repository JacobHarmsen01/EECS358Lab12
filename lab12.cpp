#include <iostream>
#include <iomanip>
#include <string>
#include <cctype>
#include <cmath>

static const double INVALID = -999999.99;

static double parseUnsignedInt(const std::string& str, int& pos, bool& ok) {
    ok = false;
    double val = 0.0;
    while (pos < (int)str.size() && isdigit((unsigned char)str[pos])) {
        val = val * 10.0 + (str[pos] - '0');
        ++pos;
        ok = true;
    }
    return val;
}

double extractNumeric(const std::string& str) {
    int n = (int)str.size();
    int i = 0;

    while (i < n) {
        while (i < n) {
            char c = str[i];
            if (isdigit((unsigned char)c) || c == '.' || c == '+' || c == '-')
                break;
            ++i;
        }
        if (i >= n) break;


        int sign = 1;

        if (str[i] == '+' || str[i] == '-') {
            int j = i;
            int lastUsableSign = -1;
            while (j < n && (str[j] == '+' || str[j] == '-')) {
                int next = j + 1;
                if (next < n && (isdigit((unsigned char)str[next]) || str[next] == '.')) {
                    lastUsableSign = j;
                }
                ++j;
            }

            if (lastUsableSign == -1) {
                i = j;
                continue;
            }

            sign = (str[lastUsableSign] == '-') ? -1 : 1;
            i = lastUsableSign + 1;  // now pointing at digit or '.'
        }

        bool hasIntDigits   = false;
        bool hasFracDigits  = false;
        bool hasDecimalPoint = false;
        double intPart  = 0.0;
        double fracPart = 0.0;

        {
            bool ok = false;
            intPart = parseUnsignedInt(str, i, ok);
            hasIntDigits = ok;
        }

        if (i < n && str[i] == '.') {
            hasDecimalPoint = true;
            ++i;
            double place = 0.1;
            while (i < n && isdigit((unsigned char)str[i])) {
                fracPart += (str[i] - '0') * place;
                place *= 0.1;
                ++i;
                hasFracDigits = true;
            }
        }

        bool hasAnyDigit = hasIntDigits || hasFracDigits;

        if (hasDecimalPoint && !hasAnyDigit) continue;
        if (!hasAnyDigit)                    continue;


        if (i < n && isalpha((unsigned char)str[i]) &&
            str[i] != 'e' && str[i] != 'E') {
            int after = i + 1;
            if (after < n && (isdigit((unsigned char)str[after]) || str[after] == '.')) {
                return INVALID;
            }
        
        }


        double expVal    = 0.0;
        bool hasExponent = false;

        if (i < n && (str[i] == 'e' || str[i] == 'E')) {
            ++i;  

            int expSign = 1;
            if (i < n && (str[i] == '+' || str[i] == '-')) {
                expSign = (str[i] == '-') ? -1 : 1;
                ++i;
                if (i < n && (str[i] == '+' || str[i] == '-')) return INVALID;
            }

            bool ok = false;
            double rawExp = parseUnsignedInt(str, i, ok);
            if (!ok)                             return INVALID;  // missing exponent digits
            if (i < n && str[i] == '.')          return INVALID;  // decimal in exponent

            expVal = expSign * rawExp;
            hasExponent = true;
        }

    
        if (i < n && str[i] == '.')                  return INVALID;
        if (i < n && isdigit((unsigned char)str[i])) return INVALID;


        double base   = sign * (intPart + fracPart);
        double result = base;

        if (hasExponent) {
            if (expVal > 309 || expVal < -324) return INVALID;

            double power = 1.0;
            int absExp = (int)std::abs(expVal);
            for (int k = 0; k < absExp; ++k) {
                power *= 10.0;
                // Overflow guard: if base is nonzero and power already huge
                if (base != 0.0 && power > 1e307 &&
                    std::abs(base) > 1e308 / power) {
                    return INVALID;
                }
            }
            result = (expVal >= 0) ? base * power : base / power;
        }

        if (std::isinf(result) || std::isnan(result)) return INVALID;

        return result;
    }

    return INVALID;
}

int main() {
    std::string line;
    while (true) {
        std::cout << "Enter a string (or 'END' to quit): ";
        if (!std::getline(std::cin, line)) break;
        if (line == "END") {
            std::cout << "Program terminated." << std::endl;
            break;
        }
        double val = extractNumeric(line);
        if (val == INVALID) {
            std::cout << "Invalid input: no valid floating-point number found\n";
        } else {
            std::cout << "Extracted number: "
                      << std::fixed << std::setprecision(4) << val << "\n";
        }
    }
    return 0;
}
