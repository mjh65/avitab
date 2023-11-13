# AviTab In-game Panel for Microsoft Flight Simulator 2020

Avitab is available for use with Microsoft Flight Simulator 2020 (MSFS).

Avitab for MSFS operates as a client & server system.
The Avitab user interface (the client) is implemented as a MSFS in-game panel.
The panel communicates with the Avitab **I**n-**G**ame **P**anel **S**erver (IGPS) application (the server)
which is a Windows executable running on the same PC as Microsoft Flight Simulator.

## Installation

It is recommended to remove previous versions before installing a new one.
The Avitab in-game panel and the server must be installed as a pair
and will most likely not work if they are not from the same release version.

If MSFS is running it will need to be restarted after Avitab is installed.
It is recommended to stop MSFS before removing a previous version of the Avitab in-game panel.

The installation zip contains 3 items. The first is a directory called ``avitab-ingamepanel-avitab``.
This should be copied to the ``Community`` directory of your MSFS installation.
The second item is a directory called ``avitab-igps``.
This can be copied to any convenient location (eg your home directory).
The third item is this README, which we assume you have already found.

## Usage

The Avitab IGPS application must be running in order for the in-game panel to work.
Go to the location where you installed ``avitab-igps``, and open this directory.
Start the app ``AviTab-msfs-igps.exe`` which will be found in this directory.
A window will appear showing the Avitab display.
Note that this is a mirror display which does not respond to mouse interactions.
It can be minimised and/or put into the background.

Windows Defender Firewall may open a dialog when ``AviTab-msfs-igps.exe`` is first run.
You should allow access from the private network so that the Avitab in-game panel can communicate with the Avitab IGPS.

After installation an Avitab icon will appear in the MSFS toolbar, accessible once a flight is started.
Clicking the icon will toggle an in-game panel that displays the familiar Avitab display.
If the Avitab IGPS is not running then a no-access symbol will be drawn with a warning message.

The Avitab IGPS can be started before or after the flight is started.
However it is recommended to start the application first.

## Release Notes

### Release FS_IGP_0001

This is first operable prototype, intended for early testing by a limited number of interested flyers.

This version implements a crude mouse button handler.
Fast mouse clicks can be missed.
It is recommended to hold the mouse button down for half a second or so to ensure the click has been observed.
(Related to this, the ``Notes`` app is not really usable for recording mouse-written notes, sorry!)
Improving this behaviour is currently on the top of the TODO list.

This version implements an unoptimised screen redrawing protocol.
Some latency may be noticeable compared to the behaviour of the XPlane plugin.
Improving this is currently high on the TODO list.

This version does not display the locations of other aircraft in the simulation, whether AI or other live
online players. We hope to implement this feature relatively soon.

The Avitab IGPS is still strongly based on the XPlane plug-in code base,
and all NAV data is loaded from the XPlane installation.
If you do not have an XPlane installation then this release is probably not for you, but thanks for your interest.
Our roadmap heads towards a future where the XPlane dependency is removed.
Perhaps you could wait there until we arrive?, we promise to be as quick as we can.

The Avitab IGPS window is a mirror of what is shown in the in-game panel.
However it does not respond to any mouse events, and we consider this may be confusing.
We will probably change the window to report a small number of messages which may be useful for debugging in the future.

Thanks for your interest.
