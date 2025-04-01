# I2C Motor Controller (ESP32 + Raspberry Pi)

This repository provides tools to control an **ESP32-based motor driver** via **I2C communication** from a **Raspberry Pi**.

There are two modes:
- **Single Command Mode**: Send a one-time or continuous command
- **Streaming Mode**: Continuously send motor values in a background thread for a fixed duration

---

## 🧰 Install dependencies
Make sure you have the required libraries for I2C support:

```bash
sudo apt-get install i2c-tools libi2c-dev build-essential
```

---

## 🛠️ Compile
```bash
g++ -o i2c_send main.cpp -li2c
g++ -o i2c_stream_thread i2c_stream_thread.cpp -pthread
```

---

## 🚀 Usage

### 🔹 `i2c_send`: Single command
Send a single I2C command to the ESP32 with a movement mode.

```bash
./i2c_send <mode> <valueMotorLeft> <valueMotorRight>
```

- `<mode>` is a hex code (without the `0x`), e.g.:
  - `44`: **continuous** movement (value is reapplied every loop on ESP32)
  - `55`: **one-time** movement (value applied once, then stopped in the next loop)

#### Example: Continuous movement
```bash
./i2c_send 44 100 -100
```

#### Example: One-shot movement
```bash
./i2c_send 55 80 80
```

---

### 🔹 `i2c_stream_thread`: Stream motor commands in a thread

Sends repeated motor values every 100 ms for **10 seconds** using a separate thread.

```bash
./i2c_stream_thread <valueMotorLeft> <valueMotorRight>
```

#### Example:
```bash
./i2c_stream_thread 120 -120
```

This mode is useful when the ESP32 is configured to interpret `0x55` as a **streaming mode** that stops the motors if no values are received after a timeout.

---

## 🔗 Useful links

- [Linux I2C dev interface documentation](https://www.kernel.org/doc/Documentation/i2c/dev-interface)
- [StackOverflow: I2C SMBus undeclared issue](https://stackoverflow.com/questions/57836694/i2c-mistake-i2c-smbus-read-byte-data-was-not-declared-in-this-scope)
