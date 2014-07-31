#ifndef BZR_CONFIG_H
#define BZR_CONFIG_H

#include "Noncopyable.h"

struct json_t;

class Config : Noncopyable
{
public:
    Config();
    ~Config();

    void setBool(const char* name, bool value);
    void setInt(const char* name, int value);
    void setDouble(const char* name, double value);
    void setString(const char* name, const string& value);

    bool getBool(const char* name, bool defaultValue);
    int getInt(const char* name, int defaultValue);
    double getDouble(const char* name, double defaultValue);
    string getString(const char* name, const string& defaultValue);

private:
    void set(const char* name, json_t* value);
    json_t* get(const char* name) const;

    string _path;
    json_t* _root;
};

#endif