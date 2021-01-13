# ASB---ECSU---2021-Winter-Project
Abdulsalam Bdeir 
2021 Independent Study Project at Eastern Connecticut State University 

The goal for this project was to learn as much as possible about the ESP8266 and the foundations of blind source separation and deep machine learning in 16 days. The project was broken down into 4 parts:  

Use an ESP8266 to do node to node communication,
learn about deep machine learning, 
learn about blind source separation, and lastly 
to integrate all that I learned in a project that would record the human heartbeat using a microphone and an ESP8266 then send that data wirelessly to a program on another computer that would use BSS and deep machine learning to remove unnecessary noise from the recording and extract the heartbeat of a person. 

I was not able to create an ESP8266 program that could store audio information before sending it to the second half of my program. The issues arose from assuming I could have used the ESP8266's IC2 bus to record enough data to then send it to a program to encode it as a wav file for later. My ESP8266 just didn't have the capacity in memory to store a recording along with its other processes. I just didn't realize my problem until 6 days into trying to get it to work. However, I can say that my goal of learning about the ESP8266, blind source separation, and deep machine learning was a success. Before this project I had only a month's worth of background information on machine learning and no prior experience with ESP8266, the Arduino IDE, or blind source separation. This project taught me a lot on how to communicate between multiple ESP8266s both directly and through WebSockets and TCP. I also learned about what Neural Networks are, how they work and how to use the machine learning library "Shogun" to conduct blind source separation on audio files in Python.
