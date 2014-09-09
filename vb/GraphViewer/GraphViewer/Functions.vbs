Option Explicit

Const e = 2.71828182845905
Const pi = 3.14159265358979

Public Function Sec(X)
	Sec = 1 / Cos(X)
End Function

Public Function Cosec(X)
	Cosec = 1 / Sin(X)
End Function

Public Function Cotan(X)
	Cotan = 1 / Tan(X)
End Function

Public Function Arcsin(X)
	Arcsin = Atn(X / Sqr(-X * X + 1))
End Function

Public Function Arccos(X)
	Arccos = Atn(-X / Sqr(-X * X + 1)) + 2 * Atn(1)
End Function

Public Function Arctan(X)
	Arctan = Atn(X)
End Function

Public Function Arcsec(X)
	Arcsec = Atn(X / Sqr(X * X - 1)) + Sgn(X - 1) * (2 * Atn(1))
End Function

Public Function Arccosec(X)
	Arccosec = Atn(X / Sqr(X * X - 1)) + (Sgn(X) - 1) * (2 * Atn(1))
End Function

Public Function Arccotan(X)
	Arccotan = Atn(X) + 2 * Atn(1)
End Function

Public Function HSin(X)
	HSin = (Exp(X) - Exp(-X)) / 2 
End Function

Public Function HCos(X)
	HCos = (Exp(X) + Exp(-X)) / 2
End Function

Public Function HTan(X)
	HTan = (Exp(X) - Exp(-X)) / (Exp(X) + Exp(-X))
End Function

Public Function HSec(X)
	HSec = 2 / (Exp(X) + Exp(-X))
End Function

Public Function HCosec(X)
	HCosec = 2 / (Exp(X) - Exp(-X))
End Function

Public Function HCotan(X)
	HCotan = (Exp(X) + Exp(-X)) / (Exp(X) - Exp(-X))
End Function

Public Function HArcsin(X)
	HArcsin = Log(X + Sqr(X * X + 1))
End Function

Public Function HArccos(X)
	HArccos = Log(X + Sqr(X * X - 1))
End Function

Public Function HArctan(X)
	HArctan = Log((1 + X) / (1 - X)) / 2
End Function

Public Function HArcsec(X)
	HArcsec = Log((Sqr(-X * X + 1) + 1) / X)
End Function

Public Function HArccosec(X)
	HArccosec = Log((Sgn(X) * Sqr(X * X + 1) +1) / X)
End Function

Public Function HArccotan(X)
	HArccotan = Log((X + 1) / (X - 1)) / 2
End Function

Public Function LogN(X, N)
	LogN = Log(X) / Log(N)
End Function

Public Function Fac(X)
	Dim C
	Dim R

	R = 1
	For C = 2 To X
		R = R * C
	Next
	Fac = R
End Function
