# orbital-simulator <br>

Satellite Orbit Simulator Commands - Linux filesystem - Ubuntu (WSL 2)

## compilation:
g++ src/main.cpp src/orbital-mechanics.cpp -Iinclude -o orbital-simulator
<br><br>
./orbital-simulator
<br>

## .venv activation:
source .venv/bin/activate 
<br>

## .venv verification: 
python -c "import pandas; import matplotlib; print('ready')"
<br>

## visualization code via .png output: 
python plot_orbit.py
<br><br>
The current orbit.png is the most recent simulation that was visualized.
