#!/bin/bash

echo Images not loaded: `./check.sh q`
echo Images loaded but not used in .xml: `./check2.sh q`
echo Images loaded but not used in .xml or in the code: `./check3.sh q`

