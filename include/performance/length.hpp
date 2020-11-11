#pragma once
#include <chrono>
#include <string>
#ifndef INFO_H417_LENGTH_HPP
#define INFO_H417_LENGTH_HPP

#endif // INFO_H417_LENGTH_HPP

namespace tl {
class length{
public:
    virtual ~length();
    virtual bool count(const std::string& path) = 0;
    virtual void output(uint16_t* count, std::string* duration ) =0;
};
}