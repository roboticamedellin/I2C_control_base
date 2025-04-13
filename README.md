# I2C Motor Controller (ESP32 + Raspberry Pi)

This repository provides tools to control an **ESP32-based motor driver** via **I2C communication** from a **Raspberry Pi**.

There are two modes:
- **Single Command Mode**: Send one-time or continuous movement commands.
- **Streaming Mode**: Continuously send motor values for a set duration using a background thread.

---

## 🧰 Install dependencies
Make sure you have the required libraries for I2C support:

```bash
sudo apt-get install i2c-tools libi2c-dev build-essential
```

---

## 🛠️ Compile
```bash
g++ -o i2c_send send_command.cpp -li2c
g++ -o i2c_stream_thread i2c_stream_thread.cpp -li2c -pthread
```

---

## 🚀 Usage

### 🔹 `i2c_send`: Single command
Sends one motor command to the ESP32.

```bash
./i2c_send <mode> <valueMotorLeft> <valueMotorRight>
```

- `<mode>` is a hex code (without the `0x`), e.g.:
  - `44`: **continuous** movement (reapplied every loop)
  - `55`: **one-time** movement (applied once, then stopped)

#### Example: Continuous movement
```bash
./i2c_send 44 100 -100
```

#### Example: One-shot movement
```bash
./i2c_send 55 80 80
```

---

### 🔹 `i2c_stream_thread`: Continuous streaming with optional duration

Sends I2C commands every 100 ms using a separate thread for a fixed number of seconds.

```bash
./i2c_stream_thread <valueMotorLeft> <valueMotorRight> [duration_seconds]
```

- The `duration_seconds` parameter is **optional**.
- If omitted, it defaults to **5 seconds**.

#### Example: Default 5 seconds
```bash
./i2c_stream_thread 120 -120
```

#### Example: Custom 8-second stream
```bash
./i2c_stream_thread 120 -120 8
```

> The ESP32 will automatically stop the motors if streaming stops (e.g., after timeout).

---

## 🔗 Useful links

- [Linux I2C dev interface documentation](https://www.kernel.org/doc/Documentation/i2c/dev-interface)
- [StackOverflow: I2C SMBus undeclared issue](https://stackoverflow.com/questions/57836694/i2c-mistake-i2c-smbus-read-byte-data-was-not-declared-in-this-scope)
