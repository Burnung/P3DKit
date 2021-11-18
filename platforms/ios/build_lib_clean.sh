if [ -f ../../CMakeCache.txt ]
then
    rm ../../CMakeCache.txt
fi
if [ -d build ]
then
    rm -rf build/
fi
if [ -d ../../CMakeFiles ]
then
    rm -rf ../../CMakeFiles/
fi
if [ -f libmmu_lib.a ]
then
    rm libmmu_lib.a
fi
