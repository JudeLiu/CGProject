# Contents
- Report.pdf: report of project 1
- src/: source codes of the project
  - main.cpp: main function

(I can't send .exe file through email. I was warned by the mail system when I tried to do so.)

# How to use
The code depends on no external libraries, so everything you need to prepare is a C++ compiler, g++ for example.

## Example of compile
```sh
g++ main.cpp --std=c++11 -O3 -o cornellbox
```

```--std=c++11``` is neccessary because I used C++ 11 features. The rest arguments are self-explanatory.

The executable has an optional argument, which controls the quality of the image. But higher quality means longer time. I used 20000 to produce the final image, which cost about 5 hour with one CPU. 200 or higher can be acceptable. 100 or less will have quick glance of the result but relatively low quality. The default value for that is 100.


```sh
cornellbox.exe [SAMPLE_PER_PIXEL]
```

The result image has .ppm format. ppm(portable PixMap) format saves plain RGB value of each pixel, which makes it easy to generator images in C++.

To view the image, you can download a ppm reader from Internet. The viewer I use is [IrfanView](https://www.irfanview.com/). (I tried to include the binary in zip, but I get warned as I explained before)