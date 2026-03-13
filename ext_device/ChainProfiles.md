# Chain Profiles

Chain Profiles are groups of any of the effect or setup commands. Therefore, Chain Profiles may have commands for Effect Configuration, Chain Configuration, Expression Pedal and Effect Setup. The profiles are grouped in lines starting with ```chain``` *chain_name*: 

```
chain [chain_name]
efc [(c)] s p1 p2
efc [(c)] s p1 p2 p3
set mysetup
…
efc [(c)] s p1 p2
pot efc rid1
pot efc rid2
chain [chain_name]
set setup1
set setup2
efc [(c)] s p1 p2 p3
…

Chain Profiles are handled and interpreted by the External Device. They are stored in permanent memory (SSD) in the External Processor (EP) in the chain.txt file (ASCII characters), which sends to DS the complete chain configuration. Any number of chain profiles can be stored in the SSD, as long as there is still available memory on card. A specific chain can be selected by providing the chain command with the profile name to EP serial interface:

```chain [chain_name]```

The chain name can have any number of characters, provided there are no blank spaces, comma or semicolon in the name. However, the maximum length of each line in chain.txt file is restricted to 128 characters. Commands in Chain Profile may include any Effect Commands, Chain Configuration, Interface Commands and Expression Pedal linkage. After receiving a valid chain command, the EP sends a clr command to DS, then locates and retrieves from SSD card the matched Chain Profile and sends the Effect Commands to DS one by one. It is important to know that all the Effect Commands need to specify the (c) parameter to force the effect to be inserted in the chain. If all the c parameters of the effect commands are greater than the maximum number of effects, the effects will be inserted into chain in the same sequence they appear in the Chain Profile.

Some examples of valid Chain Profiles are:

```
chain chain1
cmp (20) 1
ovd (20) 1
dff (20) 1
chain chain2
eqz (20) 1
rvb (20) 1
tml (20) 1
chain chain3
cmp (20) 1
dfb (20) 1
phr (20) 1
oct (20) 1
```

By using a position of 20, it is assured that the effect will be appended at the chain output, resulting therefore of a chain with effects in the same order as they appear in the command sequence. By sending the command

	chain chain1

to the External Processor and using the above file example, it answers with

> <p> ESP32>DS: clr</p>
> <p> ESP32>DS: cmp (20) 1</p>
> <p> ESP32>DS: ovd (20) 1</p>
> <p> ESP32>DS: dff (20) 1</p>

such that ```ESP32>DS``` means that the External Processor has sent the following Effect Command to Daisy Seed.

