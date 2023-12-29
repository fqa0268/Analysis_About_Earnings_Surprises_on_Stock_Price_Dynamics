#include <stdexcept>
#include <algorithm>
#include <map>
#include <string>
#include "utils.h"
using namespace std;

string ConvertDate(string date)
{
	map<string, string> month_map = {
		{"Jan", "01"},
		{"Feb", "02"},
		{"Mar", "03"},
		{"Apr", "04"},
		{"May", "05"},
		{"Jun", "06"},
		{"Jul", "07"},
		{"Aug", "08"},
		{"Sep", "09"},
		{"Oct", "10"},
		{"Nov", "11"},
		{"Dec", "12"}
	};
	
	string day = date.substr(0, date.find_first_of('-'));
	if (day.size() == 1) {
	    day = "0" + day;
	}
	date.erase(0, date.find_first_of('-')+1);
	string month = date.substr(0, date.find_first_of('-'));
	string year = date.substr(date.find_last_of('-')+1);
	return "20" + year + "-" + month_map[month] + "-" + day;
}

// overload Vector + Vector
Vector operator+(const Vector& vec1, const Vector& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must be of the same size."); 
    }
    Vector ret(vec1.size()); 
    for (size_t i = 0; i < vec1.size(); i ++) {
        ret[i] = vec1[i] + vec2[i];
    }
    return ret;
}

// overload Vector - Vector
Vector operator-(const Vector& vec1, const Vector& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must be of the same size."); 
    }
    Vector ret(vec1.size()); 
    for (size_t i = 0; i < vec1.size(); i ++) {
        ret[i] = vec1[i] - vec2[i];
    }
    return ret;
}

Vector operator*(const Vector& vec1, const Vector& vec2) {
    Vector ret(vec1.size());
    for (size_t i = 0; i < vec1.size(); i ++) {
        ret[i] = vec1[i] * vec2[i];
    }
    return ret;
}

// overload for Vector-Scalar multiplication
Vector operator*(const Vector& vec1, const double& d) {
    Vector tmp(vec1.size(), d);
    return vec1 * tmp;
}

Vector operator*(const double& d, const Vector& vec1) {
    Vector tmp(vec1.size(), d);
    return tmp * vec1;
}

// create dot Product for two Vectors
double dotProduct(const Vector& vec1, const Vector& vec2) {
    if (vec1.size() != vec2.size()) {
        throw std::invalid_argument("Vectors must be of the same size."); 
    }
    double ret = 0;
    for (size_t i = 0; i < vec1.size(); i ++) {
        ret += (vec1[i] * vec2[i]);
    }
    return ret;
}

Matrix transpose(const Matrix& mx1) {
    if (!mx1.size()) {
        throw std::invalid_argument("Matrix is empty."); 
    }
    Matrix ret(mx1[0].size(), Vector(mx1.size()));
    for (size_t i = 0; i < mx1.size(); i ++) {
        for (size_t j = 0; j < mx1[0].size(); j ++) {
            ret[j][i] = mx1[i][j];
        }
    }
    return ret;
}

Matrix operator+(const Matrix& mx1, const Matrix& mx2) {
    if (mx1.size() != mx2.size() || (mx1.size() && mx2.size() && mx1[0].size() != mx2[0].size())) {
        throw std::invalid_argument("Matrices must be of the same size."); 
    }
    Matrix ret(mx1.size());
    for (size_t i = 0; i < mx1.size(); i ++) {
        ret[i].resize(mx1[0].size());
        for (size_t j = 0; j < mx1[0].size(); j ++) {
            ret[i][j] = mx1[i][j] + mx2[i][j];
        }
    }
    return ret;
}

Matrix operator-(const Matrix& mx1, const Matrix& mx2) {
    if (mx1.size() != mx2.size() || (mx1.size() && mx2.size() && mx1[0].size() != mx2[0].size())) {
        throw std::invalid_argument("Matrices must be of the same size."); 
    }
    Matrix ret(mx1.size());
    for (size_t i = 0; i < mx1.size(); i ++) {
        ret[i].resize(mx1[0].size());
        for (size_t j = 0; j < mx1[0].size(); j ++) {
            ret[i][j] = mx1[i][j] - mx2[i][j];
        }
    }
    return ret;
}

Matrix operator*(const Matrix& mx1, const Matrix& mx2) {
    Matrix ret(mx1.size());
    for (size_t i = 0; i < mx1.size(); i ++) {
        ret[i].resize(mx1[0].size());
        for (size_t j = 0; j < mx1[0].size(); j ++) {
            ret[i][j] = mx1[i][j] * mx2[i][j];
        }
    }
    return ret;
}

Matrix operator*(const Matrix& mx1, const double& d) {
    Matrix tmp(mx1.size(), Vector(mx1[0].size(), d));
    return mx1 * tmp;
}

Matrix operator*(const double& d, const Matrix& mx1) {
    Matrix tmp(mx1.size(), Vector(mx1[0].size(), d));
    return tmp * mx1;
}
