# Compile
to compile the code and install it, just run:
```bash
make a3q1-install
```

# Experiments
At first, when i configured everything to have it run, I realized that my code isn't sending the correct values. This was not because the sending and receiving were wrong, but this was due to the TC1, TCC0, and EIC running at different frequencies. I had to put all of them under the same frequency so I assigned all of them the same general clock having them run at the same speed.

This however, did not fully resolve the conflict, I still ran into problems were the time between rise and fall which tells me the duty cycle allowing me to know which bit is sent, had problems since the moment an event occurs is not the moment it is captures. The event was always captured at random intervals after the event occured. This meant I had to work around it by having TC1 and TCC0 overflaw at the same 8-bit value. This would mean that no matter when the event was captured, even if I had an overflaw, I could calculate the exact time between rise and fall. Sometimes, I would have slightly different values. E.g.: If my start duty cycle is 50 and TC1 sends exactly that, TCC0 might capture 52 or 49. I had to make sure that it was taken into count. I applied and error rate. Since each of my duty cycles have a difference of atleast 50, I decided to make the error rate 25. Half of 50 to make sure that it worked great. However, I realised with different values that I could reduce the value since all of my peripherals are running on the same general clock. This makes the difference between rise and fall very minimal. Meaning my error rate could be at 10 and I would still be fine.

I did have another small problem with TC1 sending the same duty cycle multiple times. All I had to do was introduce an empty bit which means that the sender has sent one byte and now wait until you see the next bit. This worked great and fixed all the problems.

After this my code ran perfectly between loopback and with a different microcontroller. At this point in time, I realised that I had not disabled double buffering, however with the simple implementation, I didn't need to disable it. The send and receive worked great.