# Just another serial logger
Serial monitor for logging into text CSV files. It is just like serial monitor in Arduino IDE but without IDE and with ability to save data to CSV format files to use them for data analysis in spreadsheets or other language.

![photo_2019-06-03_14-48-37](https://user-images.githubusercontent.com/875308/59035797-66fdaa80-88b1-11e9-812b-e220ff4dfe9d.jpg)

Current version 0.0.2:
- basic plotting, parsing, adding time and saving works
- supports only one format for logging from serial "name1 value, name2 value \r\n": name1 and name2 become row headers
- chart are QtCharts, not so great
- only default arduino settings for serial communicaiton (you can set baud rate though)
- no message sending

win 64 binaries for 0.0.2: https://github.com/aleksandrbazhin/just_another_serial_logger/files/3261980/just_another_serial_logger.0.0.2.win64.zip


Roadmap:
- add sending messages to serial device
- make pretty charts with QCustomPlot
- add support for user-defined parsers including regular expressions 
- add settings for different serial connection parameters
- add builds for differnet systems

If you are looking for binaries, check releases.

If you are interested in some functionality, make an issue!
