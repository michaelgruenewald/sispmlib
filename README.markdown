sispmlib
========

*sispmlib* is a small Windows library to talk to *SIS-PM*-style 
usb-controlled multiple socket outlets. It has a C++ API and is also 
useable in scripts and batch files using a *rundll32* interface.

Building
--------

Compiling is easy, just open the project in Visual Studio or run 
`msbuild`. However, you need Windows platform headers **and** a somehow 
current version of the *Windows DDK*.

C++ API
-------

(tbd)

Rundll32 API
------------

(tbd)

Examples:

    rundll32 libsispm.dll,TurnOn 1
    rundll32 libsispm.dll,TurnOff 3
    rundll32 libsispm.dll,Toggle 4
