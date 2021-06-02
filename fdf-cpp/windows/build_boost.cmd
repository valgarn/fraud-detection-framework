rem https://stackoverflow.com/questions/20265879/how-do-i-build-boost-1-55-with-mingw
bootstrap gcc --with-libraries=python
b2 toolset=gcc --build-type=complete address-model=64 install stage --config=project-config.jam --with-python cxxflags="-D_hypot=hypot"
rem cxxflags="-include cmath "