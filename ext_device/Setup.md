# Effect Setups

Some or all the effects can be associated to a previously stored setup configuration in setup.txt file. This file shall contain the setup identification name following the Effect Command for that setup in each line:

```setup_id effect_command```

Here is some examples stored in setup.txt file:

```
overdrive1 ovd 1 1 0.2 
vibrat_1 vbt 1 10 1 5
myequalizer eql 1 0.5 .8 1
overdrive2 ovd 1 1 0.8 
```

Any effect setup can be selected with the ```set``` command:

```set mysetup```

where mysetup is one of the available setups stored in the setup.txt file. Some valid examples are:

```
set overdrive1
set myequalizer
```

The command ```set``` without setup_id causes a printout of the available setup ids on setup.txt file. For example:

```set```

>  ->overdrive1</br>
>  ->vibrat_1</br>
>  ->myequalizer</br>
>  ->overdrive2</br>

However, if the ```set``` (without ```setup_id```) command is inserted in any Chain Profile then DS answers with unknown command:

>  -> ?


