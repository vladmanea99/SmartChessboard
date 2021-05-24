# SmartChessboard
Project for Software Engineering

We used the following libraries:
1. SFML (version 2.5.1)\
[Link for download.](https://www.sfml-dev.org/download/sfml/2.5.1/)\
We used the *Visual C++ 15 (2017) - 64-bit* version. It's neccessary to place the SFML-2.5.1 root folder in the (C:\).\
This library was used for the graphic part of the application by creating the images of the positions of the chess table.\
If should errors occur by not seeing the Linkers to the root folder of SFML follow [this tutorial](https://www.youtube.com/watch?v=gluzZPH414E) for Visual Studio 2019.
2. Httplib\
[Link where we took the httplib header file from.](https://github.com/yhirose/cpp-httplib)\
Header that creates the server and endpoints we used in the project. It should already be integrated in the project; only thing that might be needed is linking the *.h* file situated in the *headers* folder in our program through the project if it isn't already linked.
3. OpenSSL\
[Link for download.](https://tecadmin.net/install-openssl-on-windows/)\
Httplib requires OpenSSL installed to be able to function.
Currently on this website there is available only a newer version than the one that we used at the time of making the project. We used version *1.1.1j*, available version is *1.1.1k*.\
The installed folder of OpenSSL should be in (C:\Program Files).\
If the project does not have the folders linked in *Visual Studio 2019*:
* In *Configuration Properties* -> *C/C++* -> *Additional Include Directories* add "C:\Program Files\OpenSSL-Win64\include".
* In *Configuration Properties* -> *Linker* -> *General* -> *Additional Library Directories* add "C:\Program Files\OpenSSL-Win64\lib".
* In *Configuration Properties* -> *Linker* -> *Input* -> *Additional Dependencies* add "libssl.lib".
4. OpenCV (version 4.1.2 vc14_vc15)\
[Link for tutorial.](https://medium.com/@subwaymatch/opencv-410-with-vs-2019-3d0bc0c81d96)\
We use OpenCV in order to be able to open images rendered and saved in local folder by SFML, in order to be used in creating a base64 string based on the image. Should be installed in (C:\).\
We used the x64/vc15 folders for linking in the project.
5. Base64\
[Link where we took it from.](https://github.com/ReneNyffenegger/cpp-base64.git)\
Library that transforms an OpenCV image into a base64 string. The neccessary files should already be in the project; if not link the *base64.cpp* in *Source Files* folder and *base64.h* in *Header Files* folder.
