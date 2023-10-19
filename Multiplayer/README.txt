Author: Jackson Westbrook
PROJECT USES VISUAL STUDIO 2022

folder should contain:

Client:
main.cpp
GameObject.cpp GameObject.h
Platform.cpp Platform.h
MovingPlatform.cpp MovingPlatform.h
Character.cpp Character.h
Timeline.cpp Timeline.h

Server:
main.cpp
GameObject.cpp GameObject.h
Platform.cpp Platform.h
MovingPlatform.cpp MovingPlatform.h
Character.cpp Character.h
Timeline.cpp Timeline.h

Create a new Visual Studio C++ solution.
Right click the solution, click add new project. Name this empty c++ project 'Client'
Right click the solution, and add another project. Name this empty c++ project 'Server'

On the taskbar, if you see 'x86' or 'x32', change this to 'x64'.

Make sure the files are extracting out of the zip.
(Right-click, extract files...)

For the client project, open the client folder
Put the *.cpp files into the source files folder of your client project.
Put the *.h files into the header files folder of your client project.

For the server project, open the server folder
Put the *.cpp files into the source files folder of your server project.
Put the *.h files into the header files folder of your server project.

On the toolbar, click Project>Configure Startup Projects
Change the startup project to the server. You may also change the startup to include multiple. This
will allow you to open a client immdeiately, but this is optional.

After configuring the startup, click the 'Local Windows Debugger' button to start.

To connect a client to the server, right click the client project.
Debug>Start New Instance
This will create a new client. You can create up to 3 clients.

CONTROLS:
(This will only work on the currently focused client/server. Click a window to focus it)
left arrow to move left
right arrow to move right
up arrow to jump
p to pause
, to slow time
. to set to default time
/ to speed up time