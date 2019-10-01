
"protoc.exe" -I=./ --cpp_out=./ ./packet_def.proto 
"protoc.exe" -I=./ --cpp_out=./ ./packet.proto 

copy *.cc ..\TCG-LoginServer\pb
copy *.h ..\TCG-LoginServer\pb

rem copy *.cc ..\testclient\pb
rem copy *.h ..\testclient\pb




pause