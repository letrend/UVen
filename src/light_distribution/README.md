# cython 
```
cython generate_image.py
gcc -shared -pthread -fPIC -fwrapv -O2 -Wall -fno-strict-aliasing -I/usr/include/python3.8 -o generate_image.so generat
e_image.c
python3 -c "import generate_image"
```
# gif
```
convert -delay 50 -loop 0 sweep/*.png -resize 1000x1000 uven2_sweep.gif
```