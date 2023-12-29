#pragma once
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

enum groupType{BEAT, MEET, MISS};

typedef vector<double> Vector;
typedef vector<Vector> Matrix;

void plotResults(int N, Matrix avgCAAR);

string ConvertDate(string date);

// overload and create Vector related ops
Vector operator+(const Vector& vec1, const Vector& vec2);
Vector operator-(const Vector& vec1, const Vector& vec2);
Vector operator*(const Vector& vec1, const Vector& vec);
Vector operator*(const Vector& vec1, const double& d);
Vector operator*(const double& d, const Vector& vec1);
double dotProduct(const Vector& vec1, const Vector& vec2); // or use inner_product from <numeric> lib



// overload and create Matrix related ops
Matrix operator+(const Matrix& mx1, const Matrix& mx2);
Matrix operator-(const Matrix& mx1, const Matrix& mx2);
Matrix operator*(const Matrix& mx1, const Matrix& mx2);
Matrix operator*(const Matrix& mx1, const double& d);
Matrix operator*(const double& d, const Matrix& mx1);
Matrix transpose(const Matrix& mx1);
