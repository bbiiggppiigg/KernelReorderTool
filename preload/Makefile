srcs := preload.so #preload2.so preload3.so purebd_preload.so base_preload.so
all: $(srcs)

%.so: %.cpp
	hipcc $^ -o $@ --shared -fPIC -Iinih/

clean:
	rm *.so

ifeq ("x","y")
srcs := preload2.so preload3.so purebd_preload.so base_preload.so
preload3.so: preload3.cpp
	hipcc -DPRINT_DETAIL=1 preload3.cpp -o preload3.so --shared -fPIC -Iinih/

preload.so: preload2.cpp
	hipcc -DPRINT_DETAIL=1 preload2.cpp -o preload.so --shared -fPIC -Iinih/

preload2.so: preload2.cpp
	hipcc preload2.cpp -o preload2.so --shared -fPIC -Iinih/

purebd_preload.so: purebd_preload.cpp
	hipcc -DPRINT_DETAIL=1 purebd_preload.cpp -o purebd_preload.so --shared -fPIC -Iinih/

base_preload.so: base_preload.cpp
	hipcc base_preload.cpp -o base_preload.so --shared -fPIC -Iinih/
endif

