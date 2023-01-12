all:preload.so preload2.so def.so

preload.so: preload.cpp
	hipcc preload.cpp -o preload.so --shared -fPIC

preload2.so: preload2.cpp
	hipcc preload2.cpp -o preload2.so --shared -fPIC -Iinih/
    

def.so: def.cpp
	hipcc def.cpp -o def.so --shared -fPIC


