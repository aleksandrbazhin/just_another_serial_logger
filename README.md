# Just another serial logger
Serial monitor for plotting and logging. Main features:
- simple and convenient serial monitor
- realtime plotting
- saving data to CSV
- (planned) ability to parse different message formats fo real-time plotting and data analysis

![ja_sl_0 0 4](https://user-images.githubusercontent.com/875308/60352385-12aa9e00-9a0b-11e9-8370-0023d8e8ed56.png)

It is somewhat like serial monitor in Arduino IDE but faster, without IDE and with ability to save data to CSV format files for data analysis.

Current version 0.0.4:
- basic plotting, parsing, saving
- supports only one format for logging from serial "name1 value, name2 value \r\n": name1 and name2 become row headers in CSV
- plots with QCustomPlot
- only default arduino settings for serial communication (you can set baud rate though)
- no message sending

win 64 binaries for 0.0.4 are in releases section

Roadmap:
- add support for user-defined parsers including regular expressions 
- add sending messages to serial device
- add settings for different serial connection parameters
- add builds for differnet systems

If you are interested in some functionality, make an issue!
