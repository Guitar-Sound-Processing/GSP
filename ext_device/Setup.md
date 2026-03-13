# Effect Setups

Some or all the effects can be associated to a previously stored setup configuration in setup.txt file. This file shall contain the setup name following the effect command for that setup in each line:

```setup_name effect_command```

Here is some examples:

```
overdrive1 ovd 1 1 0.2 
vibrat_1 vbt 1 10 1 5
myequalizer eql 1 0.5 .8 1
overdrive2 ovd 1 1 0.8 
```

Any effect setup can be selected by the ```set``` command:

```set mysetup```

where mysetup is one of the available setups stored in the setup.txt file. Some valid examples are:

```
set overdrive1
set myequalizer
```
