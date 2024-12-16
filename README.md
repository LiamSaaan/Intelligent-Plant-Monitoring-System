# Plant Disease Detection with Google Drive Integration

This project automates the process of downloading the latest plant image from a specified Google Drive folder, analyzing it using a pre-trained machine learning model, and predicting the condition of the plant (healthy or diseased).

---

## Features

- **Google Drive Integration**: Automatically fetches the latest image from a specified folder in Google Drive.
- **Plant Disease Detection**: Uses an ensemble model (Xception and DenseNet) to classify plant conditions into:
  - Healthy
  - Multiple Diseases
  - Rust
  - Scab
- **Easy-to-Use**: Just configure your Google Drive credentials and run the script.

---

## Requirements

1. **Python**: Version 3.8 or higher.
2. **Dependencies**: Listed in `requirements.txt`.
3. **Google Drive Credentials**: A `credentials.json` file for authenticating with the Google Drive API.
4. **Model File**: A pre-trained `model.h5` file for plant disease detection.

---

## Setup Instructions

### Step 1: Clone the Repository

```bash
git clone https://github.com/your-username/plant-disease-detection.git
cd plant-disease-detection
```

### Step 2: Install Dependencies

Create a virtual environment and install the required packages:

```bash
python -m venv venv
source venv/bin/activate  # For Linux/macOS
venv\Scripts\activate   # For Windows

pip install -r requirements.txt
```

### Step 3: Set Up Google Drive API

1. Go to the [Google Cloud Console](https://console.cloud.google.com/).
2. Create a new project and enable the **Google Drive API**.
3. Create credentials for the API and download the `credentials.json` file.
4. Place the `credentials.json` file in the project directory.

### Step 4: Download the Model File

Download the pre-trained `model.h5` file and place it in the project directory.

---

## Running the Script

1. Ensure you are in the virtual environment:
   ```bash
   source venv/bin/activate  # For Linux/macOS
   venv\Scripts\activate   # For Windows
   ```

2. Run the script:
   ```bash
   python main.py
   ```

3. The script will:
   - Authenticate with Google Drive.
   - Fetch the latest image from the `ESP32-CAM` folder.
   - Save the image locally.
   - Analyze the image using the model.
   - Print the prediction result.

---

## File Structure

```
plant-disease-detection/
|— credentials.json         # Google Drive API credentials
|— model.h5                # Pre-trained model file
|— requirements.txt        # Required Python packages
|— main.py                # Main script
|— utils.py               # Utility functions for image processing and prediction
|— README.md              # Project documentation
```

---

## Example Output

When you run the script, you will see output like:

```
Downloading latest file: plant_image.jpg
File 'plant_image.jpg' downloaded to ./plant_image.jpg
Analyzing the plant image...
The plant has Rust with 85% prediction.
```

---

## Additional Notes

- **Scheduling**: Use cron jobs (Linux/macOS) or Task Scheduler (Windows) to run the script daily.
- **Error Handling**: Ensure your Google Drive folder contains images to avoid errors.
- **Extending the Model**: Replace the `model.h5` file with a newer model for improved predictions.

---

## License

This project is licensed under the MIT License. Feel free to use, modify, and distribute the code.

---

## Contributing

Contributions are welcome! Please open an issue or submit a pull request to suggest improvements.

