#ifndef BOOLEXPRREADER_H
#define BOOLEXPRREADER_H

#include <iostream>
#include <string>

class BoolExprReader
{
public:
    BoolExprReader(const std::string &expr);
    bool operator()();
    virtual ~BoolExprReader();

private:
    std::string expr;
};

#endif // BOOLEXPRREADER_H
