
# Idea

Generate dashboard by a description of which controls must
be present.

This is a simple 3-step process:

- Step 1: specify in a file what signals you want to display.
- Step 2: Generate an aweful lot of handlers, setters, getters and application code
- Step 3: Compile the generated app, run it, benefit :)

## C++

Generate a Qt/C++ application

## Rust

Generate Rust GUI?


## Python

Generate GUI on the fly with PyQt5?

# Building demo

    $ cd .
    $ python generate_dashboard.py
    $ cd genapp
    $ qmake dashboard1.pro
    $ make
    $ ./dashboard1

# MQTT testing

Start a MQTT server:

    $ mosquitto -p 13001

Start a subscriber to all topics:

    $ mosquitto_sub  -p 13001 -t "#"

Send a MQTT message:

    $ mosquitto_pub -p 13001 -t "/b/bla" -m "bla bla2222"
