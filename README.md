

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

Useful links:
- https://www.kernel.org/doc/Documentation/i2c/dev-interface
- https://stackoverflow.com/questions/57836694/i2c-mistake-i2c-smbus-read-byte-data-was-not-declared-in-this-scope
