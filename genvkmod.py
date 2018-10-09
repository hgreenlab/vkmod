#!/usr/bin/python
import pyth
import pyth.env
fp = open("template.hpp", 'r');
str_cpp = fp.read()
fp.close()

t = str_cpp.split("/*<AUTOGEN>*/")


#print(mod(str_cpp))

pyth.setenv(pyth.env)

fp = open("include/vkmod2.hpp", 'w')
fp.write(
pyth.mod(str_cpp)
)
fp.close()
