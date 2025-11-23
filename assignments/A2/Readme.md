## compiling and running
To compile just run
```bash
make
```
This should compile all the files. To install the files, run

```bash
make A2Qx-install
```
NOTE: x here is just a replacement for the number you will put in for the question being marked.

## Hall Sensor Measurement

The Hall effect was sensor  was used to detect the presence of a magnet and its distance from the sensor. It gave different outputs depending on the distance from the magnet.

### Experimentation with Parameters

To ensure reliable detection, I experimented with different settings:

- Noice in reading: There wasn't much noice so the hysteris was not needed. instead, using sliding window, the noice was removed giving almost a steady value per distance without much change.
- Speed and efficiency: Using Threshold and sample size, I was able to smooth out the change of animation from one box size to another. The threshold made sure that display changes only if the change in previous sum of values read vs the current sum of values read is atleast as much as the threshold. And the sample size helped me smooth the animation.

#### Observations

- The Hall sensor reliably detected the magnet at close proximity.
- Different Threshold and sample size gave different and weird outputs.

### Conclusion

After testing, I selected:

- THRESHOLD 500
- SAMPLE_SIZE 10 

This setup ensures accurate, stable detection of the Hall sensor pulses without much noice.

## Fan Measurement

The fan was weird to control at first. Due to the friction, the fan would not start at low speeds but was able to go to that speed from higher speed. The lowest I set the fan to was 5% since anything lower than that was too slow to keep the fan running.

For the buttons, some hysterisis was needed to remove the noice, so I used the same hysterisis from previous assignment since that had already been very effective.

I wasn't able to fully finish the assignment so that is all I was able to do.