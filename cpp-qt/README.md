# Cortex C++ Examples

These examples show how to call the Cortex API from C++, using the Qt framework.

## Prerequisites

You must install the Qt framework. You can get it for free at [www.qt.io](https://www.qt.io).  
These examples were compiled and tested with Qt 5.9

## How to compile

The easiest way to compile these sources is to use the Qt Creator IDE. It is included in the Qt framework.

From Qt Creator, open the **cortexexamples.pro** project file. Then you can compile and run the examples directly from the IDE.

## Configure your Emotiv id and client id

To run these examples successfully, you need to edit **SessionCreator.cpp**, located in the **cortexclient** folder.

At the top of this file, you need to set the client id and client secret of your Cortex app.  
To get a client id and a client secret, you must connect to your Emotiv account on emotiv.com and create a Cortex app.

In the same file, you also need to set your Emotiv id and password.

## Code structure

There is a static library and a few executables. Each is located in its own folder.

### cortexclient

This library contains most of the source code.

The most important class is CortexClient, this is the one you should read first in order to understand these examples. It connects to Cortex using a web socket, and provides methods to call most of the Cortex APIs.

Then, you should look at HeadsetFinder and SessionCreator. These 2 classes contains the logic to list the headsets connected to your device, and then open a session with a headset.

The DataStreamExample class contains the logic to subscribe to any data stream of a headset.

### motion

This example opens a session with the first Emotiv headset it can find, and displays its motion data stream for 30 seconds.

### facialexpressions

This example opens a session with the first Emotiv headset it can find, and displays its facial expressions data stream for 30 seconds.

### training

This example opens a session with the first Emotiv headset it can find, and then ask you to train 3 mental commands.

If your training is successful, then you can test your mental command skills with the mentalcommand example.

### mentalcommand

This example opens a session with the first Emotiv headset it can find, and displays its mental command data stream for 30 seconds.

### eeg

**WARNING: running this example will debit a session from your Emotiv license.**

This example opens a session with the first Emotiv headset it can find, and displays its eeg data stream for 30 seconds.

To run this example, you need to edit the **main.cpp** file and set your Emotiv license.

## Signals and slots in Qt

If you are not familiar with Qt programing, you should probably read this article about [signals and slots](http://doc.qt.io/qt-5/signalsandslots.html) first.  
In a nutshell, signals and slots in Qt are an implementation of the [observer design pattern](https://en.wikipedia.org/wiki/Observer_pattern).
* The subjects emit signals.
* The observers implement slots.
* A signal can be connected to one or more slots.
* Slots and signals can be connected and disconnected at runtime.
