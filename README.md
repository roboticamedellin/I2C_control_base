

Install dependencies:
```
sudo apt-get install i2c-tools libi2c-dev build-essential
```

Compile:
```
g++ -o i2c_send main.cpp -li2c
```

Usage:
```
./i2c_send <valueMotorLeft> <valueMotorRight>
```

Example:
```
./i2c_send 100 -100
```