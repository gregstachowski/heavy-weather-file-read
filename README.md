# heavy-weather-file-read

Read history.dat binary files from Heavy Weather weatherstation
---------------------------------------------------------------

Data file format:

Records are 36 bytes long. Data is written in the same units specified in
Setup within Heavy Weather, in a mixture of 4-byte floats and integers.
Dewpoint and windchill are not stored in the binary file, and are calculated
on the fly for the text file.

Bytes:

      0 -  3 : These bytes probably specify the units used in the file.
      4 -  7 : Date, unsigned 32-bit long integer, in seconds,
               (probably) from 1.1.190[01].
      8 - 11 : Pressure, 32-bit float.
     12 - 15 : Wind speed, 32-bit float.
     16      : Wind direction, 8-bit integer (unsigned char), compass points
               clockwise from N (0=N, 1=NNE, 2=NE, 3=ENE, ... 14=NW, 15=NNW).
     17 - 19 : Blank (all zeros).
     20 - 23 : Total rainfall, 32-bit float.
     24 - 27 : Indoor temperature, 32-bit float.
     28 - 31 : Outdoor temperature, 32-bit float.
     32      : Indoor humidity, 8-bit unsigned integer.
     33      : Blank (reserved?).
     34      : Outdoor humidity, 8-bit unsigned integer.
     35      : Blank (reserved?).

"No data" markers:
------------------
Note: these markers are empirically deduced from the data file and may
not be completely correct.

     Wind speed (also affects direction): 00 00 4C 42
     Outdoor temperature: 52 38 A2 42 (= 81.1 deg. C)
     Outdoor humidity: 6E (> 100%)

Copyright (C) G.S.Stachowski, 2003.08.29

-------------------------------------------------------------------------------

TBDW:
-----
--"no data" markers for all sensors
-- first four bytes - automatic unit interpretation?
-- bugs in temperature, pressure and time code? :
Record 413: Indoor temp i s 23.5, read as 0.0.
Record   3: Date in txt file is sensible (= rec. 2), read as 244.
