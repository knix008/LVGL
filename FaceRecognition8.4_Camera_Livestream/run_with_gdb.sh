#!/bin/bash
gdb -batch -ex "set pagination off" -ex "run" -ex "thread apply all bt" -ex "quit" --args ./face_recognition
