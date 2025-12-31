# LiteCom IR Communication System – *Engineering Design Class Project* - 1st Place of 19 teams
### Sep 2025 - Dec 2025
![LiteCom main pic](https://github.com/ibyteibit/LiteCom/blob/main/pics/LiteCom.jpg)

## Description
- An IR-based communication system designed to protect workers on a noisy/dangerous shop floor by limiting movement
- Users can type a message of 20 chars on the keyboard to be sent to another module across the room
- Two buttons allow the user to turn and aim the transmit/receive module towards the target device
- The user can view the last 5 messages received by toggling the up and down arrows

![LiteCom closeup](https://github.com/ibyteibit/LiteCom/blob/main/pics/LiteComCloseup.jpg)

## Materials (for one device) & Tools
### Electrical Components:
- Arduino UNO
- Miniature PS2 Keyboard
- 20x4 LCD Display
- Green LED
- 38kHz IR Transmitter
- 38kHz IR Receiver
- Pushbutton x2
- On-Off Switch
- 9G Micro Servo Motor
- 12V AC/DC Adapter
- USB-PS2 adapter & PS2 Extension Cable
### Electrical Misc:
- Spool wire
- Heat-shrink tubing
- Solder
- Electrical tape
- Twist caps
### Mechanical:
- 3D Printed and Laser cut housing
- Screws and nuts
### Tools:
- Heat gun
- Soldering iron
- Screwdriver
- Wire cutters/strippers
- Super glue
- 3D printer and laser cutter

![LiteCom Circuit Diagram](https://github.com/ibyteibit/LiteCom/blob/main/pics/LiteComCircuit.png)

## Methodology
- Designed IR protocol from scratch, converting each char into 8-bit ASCII codes spaced with an 8-bit pattern used for clock sync and char align between the two devices
- Employed C-style strings to store and manipulate long messages in the Arduino’s limited memory
- Integrated the full-sized keyboard through much trial and error and research on the PS2Keyboard library
- Secured all electrical connections with solder and twist caps
- Led team of 4 to win 1st place out of 19 teams

![LiteCom Code Flowchart](https://github.com/ibyteibit/LiteCom/blob/main/pics/LiteComFlowchart.png)
