# stxetxcut
Unidirectional serial port communication with medical equipments

Many medical equipments (Hematology analyser, ABG analyser, Electrolyte analyser) work by sending data on serial port  

One sample Data is enclosed between \<STX\> and \<ETX\>
  
This program takes serial port data and create one file for each block between \<STX\> and \<ETX\>
  
Some equipments are erratic in no sending \<ETX\>. That problem is taken care of by setting alarm for 10 second.

If \<ETX\> is not received in 10 sec after \<STX\>, file is closed
  
# Compile as follows

gcc stxetxcut.c -o stxetxcut

# Instructions ( Linux )

Connect equipment via serial port

create a folder /root/inbox

find your serial port (e.g /dev/ttyUSB0)

go to folder where exacutable file is located

run following command

cat /dev/ttyUSB0 |./stxetxcut

it should produce one file for each analysed sample in /root/inbox folder


# Help to understand working
to check inbox every 2 sec

command: while (ls -l) do sleep 2; done

to Test alarm by following command

cat|./a.out

to send 2nd \<STX\>, press ctrl+shift+u (underlined u is seen), then write 002


