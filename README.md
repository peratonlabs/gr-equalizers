# gr-equalizers 

gr-equalizers is intended to extend the equalizer functionality of gnuradio by the following:
- Add data-aided training sequence based adaptive filtering
- Generalize the equalizer structures and separate from the underlying adaptive algorithm such that new algorithms can be implemented without creating duplicate blocks
- Other general performance improvements

As you will notice, it is based heavily on the code of the gnuradio LMS and CMA decision directed equalizers

## Installation
```
mkdir build
cd build
cmake ..
make install
```

## Modes of Operation

The linear/decision-feedback equalizer blocks operate in 3 states:

1. IDLE
    - Apply the current taps to the incoming signal with no updating to the current taps
2. TRAINING
    - Adapt the filter according to a specified training sequence that lines up with a tag in the incoming stream
3. DD
    - Perform decision directed adaptation based on the expected constellation of the signal

If a training sequence and training start tag are given, then the equalizer will begin adapting the filter at the sample corresponding to the specified tag as it is received.  

If the adapt_after_training flag is set, then the equalizer will continue adapting according to the specified constellation after the training sequence has passed

If no training sequence or no tag is specified, then the equalizer will operate in Decision Directed mode (as in current GNU Radio equalizers)

## TODO

- [x] QA tests for the linear equalizer block
- [x] Add a decision feedback equalizer block
- [ ] Add custom initial weights

