# Linear Archive Viewer and Extractor (L.A.V.E.)

This utility views and extracts the contents of game asset archives.

## Currently supported game archives

- **rcs.img** (DAVILEX "Raser" titles: Autobahn Raser II / M25 Racer a.k.a. London Racer / Paris-Marseille Racing)
- **\*.img** (GTA III / Vice City) 

## Features

- View archive contents
- Bidirectional sort according to the following attributes: Filename (case sensitive and insensitve), file offset, file size, filename extension (useful for grouping)
- Extract single files or multiple files at once

## External dependencies

- [wxWidgets](https://github.com/wxWidgets/wxWidgets) as its widget toolkit.

## Build environment

This project is build using [CMake](https://cmake.org/) on MS Windows. 

## Screenshots

![The view](/images/screenshot01.png)
![Sorting entries by filename](/images/screenshot02.png)


## Background

The original purpose of this utility was to test my archive API. Both, the *Autobahn Raser II* and *GTA* archive functionality are implemented using this API. 
Since at this point I am mostly interested in reverse engineering and writing data converters, no functionality to create these archives is currently planned. 
Since I have also reversed the structures for archives of *Lego Racers*, *Saints Row 2* and *Lego Island 2*[^1], functionality for these might be added in the future.

[^1]: By which I do not claim to have reversed them first! Since I like reversing, though, I did actually reverse them myself. 

