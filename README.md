# VotingMachine

 Voting Machine using DE1-SoC board with NIOS II/f Processor 

  FEATURES
- Can record votes for upto three candidates using push buttons from DE1-SoC board
- Added functionality to cast votes using Touch Screen interface
- Showing real time details(time, latitude, longitude) about every vote using GPS chip 
- Real time results to be printed on the console
- Ensuring only one vote could be casted by assinging voting ID's and recording them from 8 switches with 127 different 
  possibilities
- Transferring real time voting data to a central database using a WiFi chip 
- Added interactive graphics on the touch to display candidate information

  How to RUN the voting machine
- Install and open Quartus 15.0 
- git clone the repository to the PC
- Open the DE1_SoC project
- Open Eclipse from Quartus
- Create a new file with BSP template
- Add the .sopcinfo file, name the project and click finish
- Open your projects folder and hello_world.c file
- Replace the code in the hello_world.c file with the code in voting_machine.c in the repository
- Generate BSP, clean and build the project 
- Run Quartus Programmer and .sof file
- Run the project from the Eclipse
- Enjoy your voting!!
  