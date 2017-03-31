# Thor Manual Controller

This is a very simple web UI for remote-controlling stepper motors. 

## Dependencies

To compile the motor step server

- libmicrohttpd-dev
- pigpio

## Setup

- Install over an octopi disk image
- Compile motor step server

```
sudo apt-get install libmicrohttpd-dev pigpio
git clone [this repo]
cd server
make
```

- Install octopi plugin 

```
ln -s ~/thor_joint_controller/workshop_octoprint_plugin/ ~/.octoprint/plugins/workshop_octoprint_plugin
```