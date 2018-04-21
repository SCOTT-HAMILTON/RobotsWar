#include "BoolExprReader.h"
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cmath>

BoolExprReader::BoolExprReader(const std::string &pexpr) :
    expr(pexpr)
{
    expr.erase(remove_if(expr.begin(), expr.end(), isspace), expr.end());
}

BoolExprReader::~BoolExprReader()
{
    //dtor
}

bool BoolExprReader::operator()(){
    float testnum = std::atof(expr.c_str());
    if (!std::isnan(testnum)){
        if (testnum == 1)return true;
        return false;
    }
}
