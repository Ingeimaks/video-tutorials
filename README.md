# 🧠 ESP32 + Gemini AI - Complete Project Guide

This guide explains in detail how the two ESP32 + Gemini AI projects in this repository work, their differences, and how to use them.

---

## 📋 Project Overview

### 🔹 ESP32 Gemini 1.0 (Basic Version)
**Folder:** `Esp32_Gemini_1_0/`  
**Main file:** `Esp32_Gemini_1_0.ino`

### 🔹 ESP32 Gemini 2.0 (Advanced Version)
**Folder:** `Esp32 Gemini 2.0/GeminiAi2.0/`  
**Main file:** `GeminiAi2.0.ino`

---

## 🚀 ESP32 Gemini 1.0 - Basic Version

### 📌 Main Features
- **Simplicity**: Basic implementation to get started with Gemini AI
- **Functionality**: Send questions via Serial Monitor and receive responses
- **Model**: Uses `gemini-2.0-flash`
- **Configuration**: Fixed parameters in code

### 🔧 How It Works

#### 1. **Wi-Fi Connection**
```cpp
const char* ssid = "";        // Insert Wi-Fi network name
const char* password = "";    // Insert Wi-Fi password
```

#### 2. **Gemini API Configuration**
```cpp
const char* apiKey = "";      // Insert Google Gemini API key
```

#### 3. **API Endpoint**
```cpp
const String endpoint = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + String(apiKey);
```

#### 4. **Operation Flow**
1. **Setup**: Wi-Fi connection and SSL initialization
2. **Loop**: Read input from Serial Monitor
3. **Send**: POST request to Gemini API with question
4. **Parsing**: Extract response from JSON
5. **Output**: Display response on Serial Monitor

#### 5. **Request JSON Structure**
```json
{
  "contents": [
    {
      "parts": [
        {
          "text": "Your question here"
        }
      ]
    }
  ]
}
```

### ⚙️ Configuration and Usage

1. **Prerequisites**:
   - ESP32 board
   - Arduino IDE with libraries: `WiFi`, `WiFiClientSecure`, `HTTPClient`, `ArduinoJson`
   - Google Gemini API key

2. **Setup**:
   - Insert Wi-Fi credentials in `ssid` and `password` variables
   - Insert Gemini API key in `apiKey` variable
   - Upload code to ESP32

3. **Usage**:
   - Open Serial Monitor (115200 baud)
   - Write a question and press ENTER
   - Wait for Gemini's response

---

## 🚀 ESP32 Gemini 2.0 - Advanced Version

### 📌 Main Features
- **Dynamic Parameters**: Modify parameters without recompiling
- **Special Commands**: Command system for runtime configuration
- **Error Handling**: Automatic retry and improved error management
- **Text Cleaning**: Automatic removal of Markdown formatting
- **Monitoring**: Memory control and diagnostics

### 🔧 How It Works

#### 1. **Configurable Parameters**
```cpp
uint32_t maxOutputTokens = 2048;    // Maximum response length
float    temperature     = 0.7;     // Creativity (0.0-2.0)
float    topP            = 0.9;     // Cumulative probability
String   modelRuntime    = "gemini-2.5-flash";  // AI Model
```

#### 2. **Special Commands System**
- `/tokens N` - Set maximum number of tokens (e.g., `/tokens 4096`)
- `/temp X.Y` - Set creativity (e.g., `/temp 0.3`)
- `/topP X.Y` - Set probability (e.g., `/topP 0.95`)
- `/model name` - Change model (e.g., `/model gemini-1.5-flash`)
- `/heap` - Show free memory
- `/help` - Show command list

#### 3. **Advanced Error Handling**
```cpp
const uint8_t MAX_RETRIES = 2;        // Number of attempts
const uint32_t HTTP_TIMEOUT_MS = 25000;  // Extended timeout
```

#### 4. **Automatic Text Cleaning**
The `cleanText()` function removes:
- Code blocks ```...```
- Single backticks `
- Decorative asterisks * and **
- Excessive spaces and newlines

#### 5. **Advanced JSON Structure**
```json
{
  "generationConfig": {
    "maxOutputTokens": 2048,
    "temperature": 0.7,
    "topP": 0.9
  },
  "contents": [
    {
      "parts": [
        {
          "text": "Your question here"
        }
      ]
    }
  ]
}
```

### ⚙️ Configuration and Usage

1. **Initial Setup**:
   - Same libraries as version 1.0
   - Configure `SSID`, `PASSWORD`, `API_KEY`

2. **Basic Usage**:
   - Write questions normally in Serial Monitor
   - Responses are automatically cleaned from formatting

3. **Advanced Usage**:
   ```
   /tokens 4096          # Increase response length
   /temp 0.3             # Reduce creativity for more precise responses
   /model gemini-1.5-pro # Switch to more powerful model
   Explain quantum physics
   ```

---

## 🔄 Version Comparison

| Feature | Gemini 1.0 | Gemini 2.0 |
|---------|------------|------------|
| **Complexity** | Basic | Advanced |
| **Parameters** | Fixed | Dynamic |
| **Commands** | Questions only | Special commands |
| **Error Handling** | Basic | Advanced with retry |
| **Text Cleaning** | No | Yes |
| **Monitoring** | No | Memory and diagnostics |
| **Timeout** | 10 seconds | 25 seconds |
| **Models** | Only gemini-2.0-flash | Runtime changeable |

---

## 🛠️ Common Troubleshooting

### ❌ Connection Errors
- Verify Wi-Fi credentials
- Check internet connection
- Verify API key is valid

### ❌ HTTP Errors
- **HTTP 400**: Malformed request, check JSON
- **HTTP 401**: Invalid or expired API key
- **HTTP 429**: Too many requests, wait
- **HTTP 500**: Google server error, retry

### ❌ JSON Parsing Errors
- Increase `DynamicJsonDocument` buffer size
- Verify response is valid JSON

### ❌ Insufficient Memory
- Use `/heap` to monitor memory
- Reduce `maxOutputTokens`
- Restart ESP32 if necessary

---

## 🔐 Security Notes

⚠️ **IMPORTANT**: 
- **NEVER** publish Wi-Fi credentials or API key on GitHub
- Code uses `setInsecure()` for SSL certificates - **DO NOT** use in production
- For production use, implement proper SSL certificate validation

---

## 📚 Useful Resources

- [Google Gemini API Documentation](https://ai.google.dev/docs)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [ArduinoJson Library](https://arduinojson.org/)
- [Ingeimaks YouTube Channel](https://www.youtube.com/@Ingeimaks)

---

## 📄 License

Both projects are released under MIT license. You can freely modify, distribute and use the code for your projects.

---

*Created by Ingeimaks - August 2025*

✍️ Follow the [Ingeimaks](https://www.youtube.com/@Ingeimaks) channel for new ESP32 projects and other electronics, Arduino and 3D printing content!
