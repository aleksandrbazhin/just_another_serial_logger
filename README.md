# Just another serial logger
Serial monitor for plotting and logging into text CSV files. Main features:
- simple and convenient serial monitor
- realtime plotting
- (planned) ability to parse different message formats fo real-time plotting and data analysis

![image](https://user-images.githubusercontent.com/875308/59978313-7e14fa00-961e-11e9-85e7-6b39b526c167.png)

It is somewhat like serial monitor in Arduino IDE but faster, without IDE and with ability to save data to CSV format files for data analysis in spreadsheets or other languages.

Current version 0.0.3:
- basic plotting, parsing, saving
- supports only one format for logging from serial "name1 value, name2 value \r\n": name1 and name2 become row headers in CSV
- chart with QtCharts
- only default arduino settings for serial communication (you can set baud rate though)
- no message sending

win 64 binaries for 0.0.3: https://github.com/aleksandrbazhin/just_another_serial_logger/releases/download/v0.0.3/just_another_serial_logger.0.0.3.zip

Roadmap:
- add support for user-defined parsers including regular expressions 
- add sending messages to serial device
- make pretty charts with QCustomPlot
- add settings for different serial connection parameters
- add builds for differnet systems

If you are interested in some functionality, make an issue!
