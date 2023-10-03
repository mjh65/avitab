# AviTab In-game Panel for Microsoft Flight Simulator 2020

Avitab is available for use with Microsoft Flight Simulator 2020 (MSFS).

Avitab for MSFS operates as a client & server system.
The Avitab user interface (the client) is implemented as a MSFS in-game panel.
The panel communicates with the Avitab **I**n-**G**ame **P**anel **S**erver (IGPS) executable (the server)
which is a Windows executable running on the same PC as Microsoft Flight Simulator.

## Installation

It is recommended to remove previous versions before installing a new one.
The Avitab in-game panel and the server must be installed as a pair
and will most likely not work if they are not from the same release version.

If MSFS is running it will need to be restarted after Avitab is installed.
It is recommended to stop MSFS before removing a previous version of the Avitab in-game panel.

The installation zip contains 3 items.
- The first is a directory called ``avitab-ingamepanel-avitab``.
This should be copied to the ``Community`` directory of your MSFS installation.
- The second item is a directory called ``avitab-igps``.
This can be copied to any convenient location (eg your home directory).
- The third item is this README, which we assume you have already found.

Where is the ``Community`` directory?

This depends on where you obtained MSFS and how you installed it.
This [web resource](https://helpdesk.aerosoft.com/hc/en-gb/articles/5023507568925-How-to-locate-the-Community-folder-in-Microsoft-Flight-Simulator)
may be helpful if you are unsure.

## Usage

The Avitab IGPS executable must be running in order for the in-game panel to work.
Go to the location where you installed ``avitab-igps``, and open this directory.
Start the app ``AviTab-msfs-igps.exe`` which will be found in this directory.
A window will appear showing the Avitab display.
Note that this is a mirror display which does not respond to mouse interactions.
It will (currently) not work properly if minimised, but can be put into the background.

Windows Defender Firewall may open a dialog when ``AviTab-msfs-igps.exe`` is first run.
You should allow access from the private network so that the Avitab in-game panel can communicate with the Avitab IGPS.

After installation an Avitab icon will appear in the MSFS toolbar, accessible once a flight is started.
Clicking the icon will toggle an in-game panel that displays the familiar Avitab display.
If the Avitab IGPS is not running then a no-access symbol will be drawn with a warning message.

The Avitab IGPS executable can be started before or after the flight is started.
However it is recommended to start the IGPS executable first, since it still loads its navigation database
from the XPlane installation, and this can take some time.

## Release Notes

### Release FS_IGP_0004 (2023/10/03)

This update:
- fixes a bug with mouse scrollwheel handling,
- integrates latest changes of Avitab baseline code.

### Release FS_IGP_0003 (2023/05/11)

This update:
- integrates release 0.6.2 of the Avitab baseline code,
- adds Chartfox support,
- fixes a bug with the user aircraft orientation on the map overlay.

### Release FS_IGP_0002 (2023/05/07)

This update:
- shows other aircraft (traffic) in the Avitab map application.
Currently traffic information appears to be restricted to other live players (AI aircraft are not shown),
- maintains the 800x480 (5x3) aspect ratio of the Avitab display when the panel is resized.

### Known Quirks and Issues

The Avitab IGPS is still strongly based on the XPlane plug-in code base,
and all NAV data is loaded from the XPlane installation.
If you do not have an XPlane installation then this release is probably not for you, but thank you for your interest.
Our roadmap heads towards a future where the XPlane dependency is removed.
Perhaps you could wait there until we arrive?, we promise to be as quick as we can.

The Avitab IGPS window is a mirror of what is shown in the in-game panel.
However it does not respond to any mouse events, and we agree with you that this is confusing.
We will eventual change the window to show the live Avitab log messages which may be useful for debugging in the future.

The Avitab IGPS executable does not provide images if it is minimized.
It will work correctly if it is put into the background, and this is currently the recommended mode of operation.

Thanks for your interest,  
The Avitab-MSFS developers.
