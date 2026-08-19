# orbital-simulator

Satellite Orbit Simulator Commaands
Linux filesystem - Ubuntu (WSL 2)

compilation:

src code:
g++ src/main.cpp src/orbital-mechanics.cpp -Iinclude -o orbital-simulator
./orbital-simulator

.venv activation:
source .venv/bin/activate

.venv verification: 
python -c "import pandas; import matplotlib; print('ready')"

visualization code via .png output: 
python plot_orbit.py
