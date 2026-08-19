# orbital-simulator

## Satellite Orbit Simulator Commands - Linux filesystem - Ubuntu (WSL 2)

## Compilation:

src code:
g++ src/main.cpp src/orbital-mechanics.cpp -Iinclude -o orbital-simulator <br>
./orbital-simulator <br>

## .venv activation:
source .venv/bin/activate <br> 

## .venv verification: 
python -c "import pandas; import matplotlib; print('ready')"

## visualization code via .png output: 
python plot_orbit.py
