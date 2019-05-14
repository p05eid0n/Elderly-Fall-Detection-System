/*Processing sketch to process and interpret serial data received via bluetooth module. Serves as the central reference for doctors/ hospitals giving
alerts for possible falls, loss of consciousness and also number of steps taken by patient. Also requires active Internet service to send message via 3rd party server.
*/
// for serial data interpretation

import processing.serial.*;
Serial myPort;

/*Temboo is a cloud-based platform with 2000+ processes for APIs, databases, and more. The Processing + Temboo library enables us to connect
with TWILIO, a cloud communication company that allows developers to send/receive SMS and phone calls programmatically. This solution is cost efficient,
effective and is an alternate to a bulky, power hungry GSM module. */
import com.temboo.core.*;
import com.temboo.Library.Twilio.SMSMessages.*;

int Para = 2;                       // parameters -- pitch and roll
float[] values = new float[Para+2]; //total of 4 serial variables


String status = "Stable";
int flag = 0;

// Create a session with your Temboo account details

TembooSession session = new TembooSession("p05eidon", "myFirstApp", "UOJZDUuaZfTCnt6zxnZ7h9QAUpDSU7YG");

void setup()
{

// List all the available serial ports:
printArray(Serial.list());

// Port [0] is generally found to be the incoming bluetooth serial line, if no USB COM is active:
myPort = new Serial(this, Serial.list()[0], 9600);

// don't generate a serialEvent() until you get a newline character:
myPort.bufferUntil('\n');
background(0);
// initialize pitch:
values[0] = 0;

// initialize roll:
values[1] = 0;

// state variable
values[2] = 0;

// steps variable
values[3] = 0;
}

// empty draw() loop
void draw()
{
}

// Everything occurs in the serialEvent() loop
void serialEvent(Serial myPort) // get the ASCII string:
{

// Send SMS if fall Confirmed/ Distress
if(values[2] ==5)
{
if(flag==0)
runSendSMSChoreo();
flag = 1;
}


String inputString = myPort.readStringUntil('\n');

if (inputString != null)
{
// trim off any whitespace:
inputString = trim(inputString);

// splits the serial string on the delimiters and assigns it to a float array, here the delimiter can be a ,--[comma] , --[space] , \t--[tab]
values = float(splitTokens(inputString, ", \t"));
//check if you recieved the whole packet and map the data


print(values[2]);
if(values[2]==5)
status = "FALL confirmed";
else if(values[2] ==4)
status = "FALL sensed";
else if(values[2] == 0)
status = "Stable";

}
}
// Function to generate SMS
void runSendSMSChoreo()
{
// Create the Choreo object using your Temboo session
SendSMS sendSMSChoreo = new SendSMS(session);
// Set credential
sendSMSChoreo.setCredential("qwerty1");
// Set inputs
// Run the Choreo and store the results
SendSMSResultSet sendSMSResults = sendSMSChoreo.run();
// Print results
println(sendSMSResults.getResponse());
}
