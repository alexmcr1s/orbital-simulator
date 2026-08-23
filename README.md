# orbital-simulator <br>

Satellite Orbit Simulator Commands - Linux filesystem - Ubuntu (WSL 2)

## .venv activation:
source .venv/bin/activate 
<br>

## compilation:
g++ src/main.cpp src/orbital-mechanics.cpp -Iinclude -o orbital-simulator
./orbital-simulator
<br>

## .venv verification: 
python -c "import pandas; import matplotlib; print('ready')"
<br>

## visualization code via .png output: 
python plot_orbit.py
<br>
