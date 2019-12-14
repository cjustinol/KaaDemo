from ctypes import *
so_file = "./my_function.so"
my_functions = CDLL(so_file)
my_temperature = my_functions.getTemperatures()
print(my_temperature)
