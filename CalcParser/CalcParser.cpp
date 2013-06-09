#include <stdio.h>
#include <conio.h>
#include <vector>
#include <string>
#include <stack>
#include <iostream>

using namespace std;

float factorial(float n)
{
	return (n == 1.0f || n == 0.0f) ? 1.0f : factorial(n - 1.0f) * n;
}

class CToken
{
public:
	CToken(float _fNumber, bool _bIsNumber) : fNumber(_fNumber), bIsNumber(true) {};
	CToken(int _nToken) : nToken(_nToken), bIsNumber(false) {};

	union {
		float fNumber;
		int nToken;
	};

	bool bIsNumber;
};

bool ParseString(const string _sInput, vector<CToken> &Tokens)
{
	int nStartPos = -1;
	bool bGoingFunc = false;

	for (int i = 0; i < (int)_sInput.size(); i++)
	{
		bool bIsNumber = (_sInput[i] >= '0' && _sInput[i] <= '9');
		bool bIsOnlyFunc = ((_sInput[i] >= 'a' && _sInput[i] <= 'z') || (_sInput[i] >= 'A' && _sInput[i] <= 'Z'));
		bGoingFunc = bIsOnlyFunc || bGoingFunc;

		if (!bGoingFunc && (nStartPos != -1 || (i < (int)_sInput.size() - 1 && _sInput[i + 1] >= '0' && _sInput[i + 1] <= '9')))
		{
			bIsNumber = bIsNumber || (_sInput[i] == '.');
		}

		if (nStartPos == -1 && bIsOnlyFunc)
			nStartPos = i;
		else if (nStartPos != -1 && !bIsOnlyFunc && bGoingFunc)
		{
			if (_sInput[i] != '(')
			{
				return false;
			}

			string func = _sInput.substr(nStartPos, i - nStartPos);

			int pFuncAddr = 0;

			if (!func.compare("cos"))
			{
				float (*fp)(float a) = cos;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("sin"))
			{
				float (*fp)(float a) = sin;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("tan"))
			{
				float (*fp)(float a) = tan;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("abs"))
			{
				float (*fp)(float a) = abs;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("acos"))
			{
				float (*fp)(float a) = acos;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("asin"))
			{
				float (*fp)(float a) = asin;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("atan"))
			{
				float (*fp)(float a) = atan;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("ceil"))
			{
				float (*fp)(float a) = ceil;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("exp"))
			{
				float (*fp)(float a) = exp;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("floor"))
			{
				float (*fp)(float a) = floor;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("log"))
			{
				float (*fp)(float a) = log;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("log10"))
			{
				float (*fp)(float a) = log10;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("fact"))
			{
				float (*fp)(float a) = factorial;
				pFuncAddr = (int)fp;
			}
			else if (!func.compare("sqrt"))
			{
				float (*fp)(float a) = sqrt;
				pFuncAddr = (int)fp;
			} else {
				return false;
			}

			Tokens.push_back(CToken(pFuncAddr));

			nStartPos = -1;
			bGoingFunc = false;
		}

		if (nStartPos == -1 && bIsNumber)
			nStartPos = i;
		else if (nStartPos != -1 && !bIsNumber && !bGoingFunc)
		{
			Tokens.push_back(CToken((float)atof(_sInput.substr(nStartPos, i - nStartPos).c_str()), true));
			nStartPos = -1;
		}

		if (!bIsNumber && !bGoingFunc)
		{
			Tokens.push_back(CToken(_sInput[i]));
		}
	}

	if (nStartPos != -1)
	{
		Tokens.push_back(CToken((float)atof(_sInput.substr(nStartPos, _sInput.size() - nStartPos).c_str()), true));
	}

	return true;
}

#define is_operator(c)  (c == '+' || c == '-' || c == '/' || c == '*' || c == '%' || c == '!' || c == '^' || c == '~')

bool ConvertInfixToPostfix(const vector<CToken> _sInput, vector<CToken> &sOutput)
{
	stack<CToken> st;

	auto _OperatorLeftAssoc = [](const int _cInput) -> bool
	{
		return (_cInput == '*' || _cInput == '/' || _cInput == '+' || 
			_cInput == '-' || _cInput == '%' || _cInput == '^' || _cInput == '!');
	};

	auto _OperatorPriority = [](const int _cInput) -> int
	{
		switch(_cInput)
		{
			case '!': case '~':
				return 3;
			case '*': case '/': case '%': case '^':
				return 2;
			case '+': case '-':
				return 1;
		}

		return 0;
	};


	auto _OperatorArgumentCount = [](const int _cInput) -> int
	{
		if (_cInput == '*' || _cInput == '/' || _cInput == '+' || _cInput == '-' || _cInput == '%' || _cInput == '^')
		{
			return 2;
		}

		return 1;
	};

	for (const CToken &c : _sInput)
	{
		// If the token is a number, then add to the output.
		if (c.bIsNumber)
			sOutput.push_back(c);
		// If the token is a function, then push it onto the stack.
		else if (c.nToken > 256)
			st.push(c);
		// If the token is an operator.
		else if (is_operator(c.nToken))
		{
			while (!st.empty())
			{
				int cTmp = st.top().nToken;
				if (is_operator(cTmp) && 
					((_OperatorLeftAssoc(c.nToken) && _OperatorPriority(c.nToken) <= _OperatorPriority(cTmp)) ||
					_OperatorPriority(c.nToken) < _OperatorPriority(cTmp)))
				{
					sOutput.push_back(st.top());
					st.pop();
				} else
					break;
			}

			st.push(c);
		}
		else if (c.nToken == '(')
			st.push(c);
		else if (c.nToken == ')')
		{
			bool bSuccess = false;
			while (!st.empty())
			{
				int cTmp = st.top().nToken;
				if (cTmp == '(')
				{
					bSuccess = true;
					break;
				} else {
					sOutput.push_back(st.top());
					st.pop();
				}
			}

			if (!bSuccess)
				return false;

			st.pop();

			if (!st.empty())
			{
				if (st.top().nToken > 256)
				{
					sOutput.push_back(st.top());
					st.pop();
				}
			}
		}
	}

	while (!st.empty())
	{
		int cTmp = st.top().nToken;
		if (cTmp == '(' || cTmp == ')')
			return false;
			
		sOutput.push_back(st.top());
		st.pop();
	}

	return true;
}

bool CalculateExpressionFromPostfix(const vector<CToken> _PosfixTokens, float &fResult)
{
	stack<CToken> st;

	auto _OperatorArgumentCount = [](const int _cInput) -> int
	{
		if (_cInput == '*' || _cInput == '/' || _cInput == '+' || _cInput == '-' || _cInput == '%' || _cInput == '^')
		{
			return 2;
		}

		return 1;
	};

	for (const CToken &c : _PosfixTokens)
	{
		if (c.bIsNumber)
			st.push(c);
		else {
			int nArgs = _OperatorArgumentCount(c.nToken);
			if ((int)st.size() < nArgs)
				return false;

			float fTransitResult = 0.0f;

			if (c.nToken > 256)
			{
				float (*pf)(float) = (float (*)(float))c.nToken;
				fTransitResult = (*pf)(st.top().fNumber);

				st.pop();
			} else {
				if (nArgs == 1)
				{
					int nValue = (int)st.top().fNumber;
					switch (c.nToken)
					{
					case '~':
						fTransitResult = (float)(~nValue);
						break;
					case '!':
						fTransitResult = (float)(!nValue);
						break;
					};
				} else {
					float fFirstValue = st.top().fNumber;
					st.pop();
					float fSecondValue = st.top().fNumber;
					st.pop();

					switch (c.nToken)
					{
					case '+':
						fTransitResult = fSecondValue + fFirstValue;
						break;
					case '-':
						fTransitResult = fSecondValue - fFirstValue;
						break;
					case '/':
						fTransitResult = fSecondValue / fFirstValue;
						break;
					case '*':
						fTransitResult = fSecondValue * fFirstValue;
						break;
					case '%':
						fTransitResult = (float)((int)fSecondValue % (int)fFirstValue);
						break;
					case '^':
						fTransitResult = pow(fSecondValue, fFirstValue);
						break;
					};
				}
			}

			st.push(CToken(fTransitResult, true));
		}
	}

	if (!st.empty())
	{
		fResult = st.top().fNumber;
		st.pop();

		return true;
	}

	return false;
}

bool CalculateExpression(const string _sInput, float &fResult)
{
	vector<CToken> InfixTokens;
	vector<CToken> PostfixTokens;

	if (!ParseString(_sInput, InfixTokens))
		return false;

	if (!ConvertInfixToPostfix(InfixTokens, PostfixTokens))
		return false;

//#define NEED_DEBUG 1
#ifdef NEED_DEBUG
	printf("%s\n", _sInput.c_str());

	for (CToken &tok : InfixTokens)
	{
		if (tok.bIsNumber)
			printf("%0.2f", tok.fNumber);
		else if (tok.nToken < 256)
			printf("%c", tok.nToken);
		else
			printf("f");
	}
	printf("\n");

	for (CToken &tok : PostfixTokens)
	{
		if (tok.bIsNumber)
			printf("%0.2f ", tok.fNumber);
		else if (tok.nToken < 256)
			printf("%c ", tok.nToken);
		else
			printf("f");
	}
	printf("\n");
#endif
#undef NEED_DEBUG

	if (!CalculateExpressionFromPostfix(PostfixTokens, fResult))
		return false;

	return true;
}

int main()
{
	string str;
	//str = "cos(5.43 * (sin(53.43 * 4 * log(53 + 1.43)) + 2)) + 45.0";
	getline(cin, str);

	float fResult = 0.0f;
	if (!CalculateExpression(str, fResult))
	{
		printf("Error occured...\n");

		_getch();
		return 0;
	}

	printf("%0.10f\n", fResult);

	_getch();
	return 0;
}