sispmlib
========

*sispmlib* is a small Windows library to talk to *SIS-PM*-style 
usb-controlled multiple socket outlets. It has a C++ API and is also 
useable in scripts and batch files using a *rundll32* interface.

Building
--------

Compiling is easy, just open the project in Visual Studio or run `msbuild`. 
However, you need Windows platform headers **and** a somehow current 
version of the *Windows DDK*.

C++ API
-------

(tbd)

Rundll32 API
------------

(tbd)

Examples:

    rundll32 sispmlib.dll,TurnOn 1
    rundll32 sispmlib.dll,TurnOff 3
    rundll32 sispmlib.dll,Toggle 4

License
-------

This program is free software: you can redistribute it and/or modify it 
under the terms of the GNU Lesser General Public License as published by 
the Free Software Foundation, either version 3 of the License, or (at your 
option) any later version. 

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public 
License for more details.

You should have received a copy of the GNU Lesser General Public License 
along with this program.  If not, see <http://www.gnu.org/licenses/>.
