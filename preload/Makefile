all:preload.so preload2.so

preload.so: preload2.cpp
	hipcc -DPRINT_DETAIL=1 preload2.cpp -o preload.so --shared -fPIC -Iinih/

preload2.so: preload2.cpp
	hipcc preload2.cpp -o preload2.so --shared -fPIC -Iinih/

