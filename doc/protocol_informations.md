# Honda pgm-fi K-line protocol informations

## Initialization sequence
The ECU will not respond until it has received the correct initialisation sequence:
* Pull the K-line low for 70msec
* Return the K-line to the high state for 120msec
* Send “Wakeup” message: FE 04 FF FF. No response is expected
* Wait 200msec
* Send “Initialise” message: 72 05 00 F0 99
* The ECU should respond with: 02 04 00 FA


## Example
Basically the ECU simply responds to requests for information.
It doesn’t care if the response is not heard.

A typical request is of the form:
'''
72 07 72 11 00 14 F0 (in hexadecimal)
72: the destination code (I think?)
07: the number of characters in the message including the checksum
72: the type of query (“give me data from memory?”)
11: the data table number
00: the starting register within data table 11
14: the finishing register within data table 11
F0: the checksum
The ECU will respond with a similar format:
02 1A 72 11 00 XX XX XX …XX YY
02: the destination code (I think?)
1A: the number of characters in the message (in this case 26 decimal)
72: the type of query (“data from memory?”)
11: the data table number
00: the starting register
XX: the data
YY: the checksum
'''
The checksum is calculated differently to the ISO OBDII protocol. It goes like this:
* Sum all the values of the characters together, and take that value from 100 (hex).
* If the sum is greater than 100 hex, just use the last two characters (in hex).

So, the request message above (in hex) would be:

0x72 + 0x07 + 0x72 + 0x11 + 0x00 + 0x14 = 0x110

Use only the last two characters, 0x10

0x100 – 0x10 = 0xF0 = the checksum F0

The response checksum is calculated similarly.

There are other query types (the third character in the request sequence). The ones I know to exist are 71, 72, 73 and 74. Code 71 requests all of the table data, rather than a subset:

72 05 71 00 18 requests all of the data in table 00

I don’t know what a code 73 or 74 request does.
