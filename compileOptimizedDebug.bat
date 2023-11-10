cls
g++ -O2 -march=native -ffunction-sections -Wl,-Map=output.map -flto .\noSDLmain.cpp -o ray